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
#include <ripple/thrift/HBaseConn.h>
#include <ripple/core/ConfigSections.h>
#include <ripple/protocol/SystemParameters.h>
#include <string>

#if RIPPLE_THRIFT_AVAILABLE
#include <ripple/app/misc/DividendMaster.h>
#endif

namespace ripple {
    Json::Value doContributePower(RPC::Context& context) {
        return RPC::make_error(rpcINTERNAL, "not implemented");
//#ifndef RIPPLE_THRIFT_AVAILABLE
//        return RPC::make_error(rpcNOT_SUPPORTED, "only server compiled with hbase client support this rpc");
//#else
//        auto& params = context.params;
//        if (!params.isMember(jss::account)) {
//            return RPC::missing_field_error(jss::account);
//        }
//        std::string account = params[jss::account].asString();
//        
//        if (!params.isMember(jss::action)) {
//            return RPC::missing_field_error(jss::action);
//        }
//        
//        std::string action = params[jss::action].asString();
//        
//        if (action != "check" && action != "change_by" && action != "change_to") {
//            return RPC::make_error(rpcSRC_ACT_NOT_FOUND, to_string("Unknow action - ") + action);
//        }
//        
//        double power = 0;
//        if (action == "change_by" || action == "change_to") {
//            //power param is needed
//            if (!params.isMember ("power")) {
//                return RPC::missing_field_error("power");
//            }
//            power = params["power"].asDouble();
//            //private key is needed
//        }
//        
//        //check account existance before doing any action
//        AccountID accountID;
//        if (auto jv = RPC::accountFromString(accountID, account)) {
//            return rpcError (rpcACT_NOT_FOUND);
//        } else {
//            std::shared_ptr<ReadView const> ledger;
//            auto result = RPC::lookupLedger(ledger, context);
//            if (!ledger) {
//                return rpcError (rpcLGR_NOT_FOUND);
//            }
//            if (!ledger->exists(keylet::account (accountID))) {
//                return rpcError (rpcACT_NOT_FOUND);
//            }
//        }
//
//        std::vector<std::string> children;
//        //do action by contribute_power_manager
//        auto& dm = getApp().getDividendMaster();
//        if (action == "change_by") {
//            power = dm.changeContributePowerBy(account, power);
//        } else if (action == "change_to") {
//            power = dm.changeContributePowerTo(account, power);
//        } else {
//            power = dm.checkContributePower(account).first;
//            dm.fetchChildren(account, children, 10);
//        }
//        
//        //power < 0 means error occurred
//        if (power >= 0) {
//            Json::Value ret (Json::objectValue);
//            ret[jss::account] = account;
//            ret[jss::action] = action;
//            ret["power"] = power;
//            ret["referrer"] = Json::Value(Json::arrayValue);
//            for (auto child : children) {
//                ret["referrer"].append(child);
//            }
//            return ret;
//        } else {
//            return RPC::make_error(rpcINTERNAL, "action fail");
//        }
//#endif
    }
}
