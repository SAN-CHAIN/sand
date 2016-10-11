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

namespace ripple {

Json::Value doBalanceDump(RPC::Context& context) {
    if (!context.params.isMember(jss::ledger_index)) {
        return RPC::missing_field_error(jss::action);
    }
    if (!context.params.isMember("threshold")) {
        return RPC::missing_field_error("threshold");
    }
    if (!context.params.isMember("temp_index")) {
        return RPC::missing_field_error("temp_index");
    }
    
    uint32_t ledgerIndex = context.params[jss::ledger_index].asUInt();
    uint64_t threshold = std::stol(context.params["threshold"].asString());         //Value Accounts Threshold Value
    uint64_t temp_index = std::stol(context.params["temp_index"].asString());       //Temperature Index

    auto& dm = getApp().getDividendMaster();
    if (dm.dumpBalance(ledgerIndex, threshold, temp_index)) {
        Json::Value ret (Json::objectValue);
        ret[jss::ledger_index] = ledgerIndex;
        return ret;
    } else {
        return RPC::make_error(rpcINTERNAL, "action fail");
    }
}

}
