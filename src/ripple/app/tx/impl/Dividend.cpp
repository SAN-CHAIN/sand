//------------------------------------------------------------------------------
/*
 This file is part of rippled: https://github.com/ripple/rippled
 Copyright (c) 2012, 2013 Ripple Labs Inc.
 
 Permission to use, copy, modify, and/or distribute this software for any
 purpose  with  or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.
 
 THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
 MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
//==============================================================================

#include <ripple/app/tx/impl/Dividend.h>
#include <ripple/core/ConfigSections.h>
#include <ripple/app/misc/DividendMaster.h>

namespace ripple {
    
    TER Dividend::preCheck() {
        auto const fee = ctx_.tx.getFieldAmount (sfFee);
        if (!fee.native() || fee != beast::zero) {
            ctx_.journal.warning << "Non-zero fee";
            return temBAD_FEE;
        }

        if (!ctx_.tx.isFieldPresent(sfDividendType)) {
            ctx_.journal.warning << "No dividend type";
            return temBAD_DIV_TYPE;
        }
        
        if (!ctx_.tx.isFieldPresent(sfDividendLedger)) {
            ctx_.journal.warning << "No dividend ledger";
            return temINVALID;
        }
        
        if (!ctx_.tx.isFieldPresent(sfDividendCoins)) {
            ctx_.journal.warning << "No dividend coins";
            return temINVALID;
        }
        
        // check if signing public key is trusted.
        auto const& dividendAccount = getConfig()[SECTION_DIVIDEND_ACCOUNT];
        std::string public_key = get<std::string>(dividendAccount, "public_key");
        if (public_key.empty()) {
            ctx_.journal.warning << "public_key is not configured in dividend_account to check dividend transaction";
            return tefBAD_AUTH;
        }
        
        auto const accountPublic = parseBase58<AccountID>(public_key);
        if (!accountPublic || calcAccountID(RippleAddress::createAccountPublic(ctx_.tx.getSigningPubKey())) != accountPublic) {
            ctx_.journal.warning << "apply: Invalid transaction (bad signature)";
            return temBAD_SIGNATURE;
        }
        
        if ((ctx_.tx.getSequence() != 0) || ctx_.tx.isFieldPresent(sfPreviousTxnID)) {
            ctx_.journal.warning << "Bad sequence";
            return temBAD_SEQUENCE;
        }
        
        return tesSUCCESS;
    }
    
    TER Dividend::checkSeq() {
        return tesSUCCESS;
    }
    
    TER Dividend::checkSign() {
        //TODO check sign here
        return tesSUCCESS;
    }
    
    TER Dividend::payFee() {
        return tesSUCCESS;
    }
    
    TER Dividend::doApply() {
        if (ctx_.tx.getTxnType () == ttDIVIDEND) {
            uint8_t divOpType = ctx_.tx.isFieldPresent(sfDividendType) ? ctx_.tx.getFieldU8 (sfDividendType) : DividendMaster::DivType_Start;
            switch (divOpType) {
                case DividendMaster::DivType_Start: {
                    return startCalc();
                }
                case DividendMaster::DivType_Apply: {
                    return applyTx();
                }
                case DividendMaster::DivType_Done: {
                    return doneApply();
                }
            }
        }
        return temUNKNOWN;
    }
    
    TER Dividend::startCalc() {
        auto const k = keylet::dividend();
        SLE::pointer dividendObject = view().peek(k);
        
        if (!dividendObject) {
            //if no div obj exists yet, it's first dividend
            dividendObject = std::make_shared<SLE>(k);
            view().insert(dividendObject);
        }
        
        auto& tx = ctx_.tx;
        
        j_.info << "(startCalc) Previous dividend object: " << dividendObject->getText();
        
        uint32_t dividendLedger = tx.getFieldU32(sfDividendLedger);
        uint64_t dividendCoins = tx.getFieldU64(sfDividendCoins);
        
        dividendObject->setFieldU8(sfDividendState, DividendMaster::DivState_Start);
        dividendObject->setFieldU32(sfDividendLedger, dividendLedger);
        dividendObject->setFieldU64(sfDividendCoins, dividendCoins);
        view().update(dividendObject);
        
        j_.info << "(startCalc) Current dividend object: " << dividendObject->getText();
        
        return tesSUCCESS;
    }

    TER Dividend::doneApply() {
        auto const k = keylet::dividend();
        SLE::pointer dividendObject = view().peek(k);
        
        //dividend should have existed now
        if (!dividendObject) {
            return temMALFORMED;
        }
        
        j_.info << "(doneApply) Previous dividend object: " << dividendObject->getText();
        
        dividendObject->setFieldU8(sfDividendState, DividendMaster::DivState_Done);
        view().update(dividendObject);
        
        j_.info << "(doneApply) Current dividend object: " << dividendObject->getText();
        
        return tesSUCCESS;
    }
    
    TER Dividend::applyTx() {
        j_.debug << "apply dividend";
        
        auto& tx = ctx_.tx;
        
        const auto& account = tx.getAccountID(sfDestination);
        j_.trace << "des account " << account;
        
        uint64_t divCoins = tx.getFieldU64(sfDividendCoins);
        
        auto sleAccountModified = view().peek(keylet::account(account));
        
        if (sleAccountModified) {
            uint32_t accountDividendLedger = 0;
            if (sleAccountModified->isFieldPresent(sfDividendLedger)) {
                accountDividendLedger = sleAccountModified->getFieldU32(sfDividendLedger);
            }
            // check multi-dividend unexcepted
            if (ctx_.tx.getFieldU32(sfDividendLedger) == accountDividendLedger) {
                j_.warning << "Unexcepted dividend transaction in applyTx().";
                return tefFAILURE;
            }
            if (divCoins > 0) {
                sleAccountModified->setFieldAmount(sfBalance, sleAccountModified->getFieldAmount(sfBalance) + divCoins);
                ctx_.createXRP(divCoins);
            }
            if (tx.isFieldPresent(sfDividendLedger)) {
                std::uint32_t divLedgerSeq = tx.getFieldU32(sfDividendLedger);
                sleAccountModified->setFieldU32(sfDividendLedger, divLedgerSeq);
            }
            view().update(sleAccountModified);
            j_.info << "Dividend Applied:" << sleAccountModified->getText();
        } else {
            j_.warning << "Dividend account not found :" << account;
            return tefBAD_LEDGER;
        }
        
        return tesSUCCESS;
    }
}
