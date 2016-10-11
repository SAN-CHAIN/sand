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

#ifndef RIPPLE_APP_MISC_DIVIDEND_MASTER_H_INCLUDED
#define RIPPLE_APP_MISC_DIVIDEND_MASTER_H_INCLUDED

#include <ripple/app/main/Application.h>
#include <beast/utility/Journal.h>
#include <ripple/app/ledger/Ledger.h>

namespace ripple {

    class DividendMaster {
    public:
        typedef enum {
            DivType_Done = 0,
            DivType_Start = 1,
            DivType_Apply = 2
        } DivdendType;
        
        typedef enum {
            DivState_Done = 0,
            DivState_Start = 1
        } DivdendState;
        
//        virtual double changeContributePowerTo(const std::string& account, double power) = 0;
//        virtual double changeContributePowerBy(const std::string& account, double power) = 0;
//        virtual std::pair<double, double> checkContributePower(const std::string& account) = 0;
        
        virtual bool fetchChildren(const std::string &account, std::vector<std::string> &children, int limit = -1) = 0;
        
        virtual bool dumpBalance(uint32_t ledgerIndex, uint64_t balanceTh, uint64_t balanceT) = 0;
        virtual bool calcDividend(uint32_t ledgerIndex, uint64_t dividendCoins) = 0;
        virtual bool launchDividend(uint32_t ledgerIndex, uint64_t dividendCoins) = 0;
        
        virtual Json::Value checkStatus(uint32_t ledgerIndex) = 0;
        
        virtual void checkAndRunTxns() = 0;
        virtual bool signAndRunTx(const std::string &acc_str, uint32_t ledgerIndex, uint64_t coins) = 0;
    };
    
    std::unique_ptr<DividendMaster>
    make_DividendMaster(beast::Journal journal);
}

#endif //RIPPLE_APP_MISC_DIVIDEND_MASTER_H_INCLUDED
