//------------------------------------------------------------------------------
/*
 This file is part of rippled: https://github.com/ripple/rippled
 Copyright (c) 2012-2014 Ripple Labs Inc.
 
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


#include <BeastConfig.h>
#include <ripple/rpc/Context.h>
#include <ripple/app/main/Application.h>
#include <ripple/app/misc/DividendMaster.h>
#include <ripple/core/ConfigSections.h>

namespace ripple {
    
    Json::Value calculateDividend(uint32_t ledgerIndex, uint64_t coins) {
        // public key is needed to accept dividend transactions.
        std::string public_key = get<std::string> (getConfig()[SECTION_DIVIDEND_ACCOUNT], "public_key");
        if (public_key.empty()) {
            return RPC::make_error (rpcNOT_ENABLED, "public_key missing in cfg.");
        }
        
        auto accountPublic = parseBase58<AccountID>(public_key);
        if (!accountPublic) {
            return RPC::make_error(rpcPUBLIC_MALFORMED, "invalid public_key in cfg.");
        }
        
        // private key is needed to launch dividend.
        std::string secret_key = get<std::string>(getConfig()[SECTION_DIVIDEND_ACCOUNT], "secret_key");
        if (secret_key.empty ()) {
            return RPC::make_error (rpcNOT_ENABLED, "secret_key missing in cfg.");
        }
        
        RippleAddress secret = RippleAddress::createSeedGeneric(secret_key);
        RippleAddress generator = RippleAddress::createGeneratorPublic(secret);
        RippleAddress naAccountPrivate = RippleAddress::createAccountPrivate(generator, secret, 0);
        
        if (calcAccountID(RippleAddress::createAccountPublic (generator, 0)) != accountPublic) {
            return RPC::make_error(rpcBAD_SECRET, "secret_key does not match public_key in cfg.");
        }
        
        auto& dm = getApp().getDividendMaster();
        
        if (!dm.calcDividend(ledgerIndex, coins)) {
            return RPC::make_error (rpcINTERNAL, "Failed to calculate dividend.");
        }
        
        Json::Value ret (Json::objectValue);
        return ret;
    }
    
    Json::Value launchDividend(uint32_t ledgerIndex, uint64_t coins) {
        // public key is needed to accept dividend transactions.
        std::string public_key = get<std::string> (getConfig()[SECTION_DIVIDEND_ACCOUNT], "public_key");
        if (public_key.empty()) {
            return RPC::make_error (rpcNOT_ENABLED, "public_key missing in cfg.");
        }
        
        auto accountPublic = parseBase58<AccountID>(public_key);
        if (!accountPublic) {
            return RPC::make_error(rpcPUBLIC_MALFORMED, "invalid public_key in cfg.");
        }
        
        // private key is needed to launch dividend.
        std::string secret_key = get<std::string>(getConfig()[SECTION_DIVIDEND_ACCOUNT], "secret_key");
        if (secret_key.empty ()) {
            return RPC::make_error (rpcNOT_ENABLED, "secret_key missing in cfg.");
        }
        
        RippleAddress secret = RippleAddress::createSeedGeneric(secret_key);
        RippleAddress generator = RippleAddress::createGeneratorPublic(secret);
        RippleAddress naAccountPrivate = RippleAddress::createAccountPrivate(generator, secret, 0);
        
        if (calcAccountID(RippleAddress::createAccountPublic (generator, 0)) != accountPublic) {
            return RPC::make_error(rpcBAD_SECRET, "secret_key does not match public_key in cfg.");
        }
        
        auto& dm = getApp().getDividendMaster();
        
        if (!dm.launchDividend(ledgerIndex, coins)) {
            return RPC::make_error (rpcINTERNAL, "Failed to launch dividend");
        }

        Json::Value ret (Json::objectValue);
        return ret;
    }
    
    Json::Value signAndRunTx(const std::string &acc_str, uint32_t ledgerIndex, uint64_t coins) {
        std::string public_key = get<std::string> (getConfig()[SECTION_DIVIDEND_ACCOUNT], "public_key");
        if (public_key.empty()) {
            return RPC::make_error (rpcNOT_ENABLED, "public_key missing in cfg.");
        }
        
        auto accountPublic = parseBase58<AccountID>(public_key);
        if (!accountPublic) {
            return RPC::make_error(rpcPUBLIC_MALFORMED, "invalid public_key in cfg.");
        }
        
        // private key is needed to launch dividend.
        std::string secret_key = get<std::string>(getConfig()[SECTION_DIVIDEND_ACCOUNT], "secret_key");
        if (secret_key.empty ()) {
            return RPC::make_error (rpcNOT_ENABLED, "secret_key missing in cfg.");
        }
        
        RippleAddress secret = RippleAddress::createSeedGeneric(secret_key);
        RippleAddress generator = RippleAddress::createGeneratorPublic(secret);
        RippleAddress naAccountPrivate = RippleAddress::createAccountPrivate(generator, secret, 0);
        
        if (calcAccountID(RippleAddress::createAccountPublic (generator, 0)) != accountPublic) {
            return RPC::make_error(rpcBAD_SECRET, "secret_key does not match public_key in cfg.");
        }

        auto& dm = getApp().getDividendMaster();
        
        if (!dm.signAndRunTx(acc_str, ledgerIndex, coins)) {
            return RPC::make_error (rpcINTERNAL, "Failed to run dividend tx");
        }
        
        Json::Value ret (Json::objectValue);
        return ret;
    }
    
    Json::Value checkDividend(uint32_t ledgerIndex) {
        auto& dm = getApp().getDividendMaster();
        return dm.checkStatus(ledgerIndex);
    }
    
    Json::Value doDividend(RPC::Context& context) {
        auto& params = context.params;
        if (!params.isMember(jss::action)) {
            return RPC::missing_field_error(jss::action);
        }
        std::string action = params[jss::action].asString();

        bool bCheck = action == "check";
        bool bLaunch = action == "launch";
        bool bCalculate = action == "calculate";
        bool bTx = action == "tx";
        
        if (!bCheck && !bLaunch && !bCalculate && !bTx) {
            return RPC::make_error(rpcUNKNOWN_COMMAND, to_string("Unknow action - ") + action);
        }

        if (bCheck) {
            uint32_t ledgerIndex = 0;
            if (params.isMember(jss::ledger_index)) {
                ledgerIndex = context.params[jss::ledger_index].asUInt();
            }
            return checkDividend(ledgerIndex);
        } else {
            if (!params.isMember(jss::ledger_index)) {
                return RPC::missing_field_error(jss::ledger_index);
            }
            if (!params.isMember("coins")) {
                return RPC::missing_field_error("coins");
            }
            uint32_t ledgerIndex = context.params[jss::ledger_index].asUInt();
            uint64_t coins = std::stol(context.params["coins"].asString());
            if (bLaunch) {
                return launchDividend(ledgerIndex, coins);
            } else if (bTx) {
                if (!params.isMember(jss::account)) {
                    return RPC::missing_field_error("account");
                }
                std::string account = context.params[jss::account].asString();
                return signAndRunTx(account, ledgerIndex, coins);
            } else {
                return calculateDividend(ledgerIndex, coins);
            }
        }
    }
}
