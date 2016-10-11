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

#include <ripple/app/misc/DividendMaster.h>
#include <vector>
#include <string>
#include <iterator>
#include <tuple>
#include <array>
#include <functional>
#include <ripple/rpc/impl/AccountFromString.h>
#include <boost/filesystem.hpp>
#include <fstream>
#include <boost/format.hpp>
#include <mutex>

#define CONTRIBUTE_POWER_MIN    0
#define CONTRIBUTE_POWER_MAX    100
#define DIVIDEND_CACULATE_CYCLE 7
#define COIN_AGE_T_PARAM        (70000LL * SYSTEM_CURRENCY_PARTS)
#define DIVIDEND_BALANCE_MIN    (100LL * SYSTEM_CURRENCY_PARTS)
#define REFERRER_CONTRIBUTE_RATE 0.618

namespace ripple {
    class DividendMasterImpl : public DividendMaster {
    public:
        DividendMasterImpl(beast::Journal journal)
              : m_journal(journal) {
        }
        
//        double changeContributePowerTo(const std::string& account, double power) override {
//            if (!hasPrivateKey()) {
//                return 0;
//            }
//            int8_t old_pow = checkContributePower(account);
//            if (old_pow < CONTRIBUTE_POWER_MIN || old_pow > CONTRIBUTE_POWER_MAX) {
//                return 0;
//            }
//            int8_t new_pow = power;
//            //new_power must between CONTRIBUTE_POWER_MIN and CONTRIBUTE_POWER_MAX
//            if (new_pow > CONTRIBUTE_POWER_MAX) {
//                new_pow = CONTRIBUTE_POWER_MAX;
//            }
//            if (new_pow < CONTRIBUTE_POWER_MIN) {
//                new_pow = CONTRIBUTE_POWER_MIN;
//            }
//            if (new_pow != old_pow) {
//                if (modifyContributePower(account, new_pow)) {
//                    return new_pow;
//                }
//            }
//            return 0;
//        }
//
//        double changeContributePowerBy(const std::string& account, double power) override {
//            if (!hasPrivateKey()) {
//                return 0;
//            }
//            int8_t old_pow = checkContributePower(account);
//            if (old_pow >= CONTRIBUTE_POWER_MIN && old_pow <= CONTRIBUTE_POWER_MAX) {
//                return changeContributePowerTo(account, power + old_pow);
//            } else {
//                return 0;
//            }
//            return 0;
//        }

        std::pair<double, double> checkContributePower(const std::string& account) {
            if (!hasPrivateKey()) {
                return std::pair<double, double>(0, 0);
            }
            auto db = getApp().getTxnDB().checkoutDb();
            boost::optional<double> ctrb;
            soci::indicator ctrbi;
            boost::optional<double> rp;
            soci::indicator rpi;
            auto sql = boost::str(boost::format("SELECT PartsPower, ReferPower FROM AccountReference WHERE Account=\"%s\"") % account.c_str());
            *db << sql, soci::into(ctrb, ctrbi), soci::into(rp, rpi);
            if (!db->got_data () || ctrbi != soci::i_ok || rpi != soci::i_ok) {
                return std::pair<double, double>(1, 1);
            } else {
                return std::pair<double, double>(ctrb.value_or(1), rp.value_or(1));
            }
            return std::pair<double, double>(1, 1);
        }
            
        bool fetchChildren(const std::string &account, std::vector<std::string> &children, int limit) override {
            if (!hasPrivateKey()) {
                return false;
            }
            children.clear();
            auto db = getApp().getTxnDB().checkoutDb();
            boost::optional<std::string> acc;
            soci::indicator acci;
            std::string sql = "";
            if (limit <= 0) {
                sql = boost::str(boost::format("SELECT Account FROM AccountReference WHERE Referee=\"%s\"")
                                 % account.c_str());
            } else {
                sql = boost::str(boost::format("SELECT Account FROM AccountReference \
                                WHERE Referee=\"%s\" LIMIT %d")
                                 % account.c_str()
                                 % limit);
            }
            soci::statement st = (db->prepare << sql, soci::into(acc, acci));
            st.execute();
            while (st.fetch()) {
                if (acci == soci::i_ok) {
                    std::string acc_str = acc.value_or("");
                    if (acc_str != "") {
                        children.push_back(acc_str);
                    }
                }
            }
            return true;
        }
        
        bool dumpBalance(uint32_t ledgerIndex, uint64_t balanceTh, uint64_t balanceT) override {
            getApp().getJobQueue().addJob(jtBALANCE_DUMP,
                                          "DividendMaster::doDumpBalance",
                                          std::bind (
                                                     &::ripple::DividendMasterImpl::doDumpBalance,
                                                     this, ledgerIndex, balanceTh, balanceT));
            return true;
        }

        bool doDumpBalance(uint32_t ledgerIndex, uint64_t balanceTh, uint64_t balanceT) {
            if (!hasPrivateKey()) {
                return false;
            }
            std::lock_guard<std::mutex> lock(m_mutex);

            Ledger::pointer ledger = getApp().getLedgerMaster().getLedgerBySeq(ledgerIndex);
            if (!ledger) {
                return false;
            }
            
            auto db = getApp().getTxnDB().checkoutDb();
            std::string date = getDate(ledger->info().closeTime);
            std::uint32_t t_start = getApp().getOPs().getNetworkTimeNC();
            auto sql_start = boost::str(boost::format("REPLACE INTO PowerDumpLog (DumpDate, CreateTime, Status) VALUES(\"%s\", %d, %d)") % date.c_str() % t_start % 0);
            *db << sql_start;

            std::map<std::string, uint64_t> accountBalances;
            
            std::map<std::string, uint64_t> account_balance;
            ledger->visitStateItems([&](SLE::ref sle){
                if (sle->getType() != ltACCOUNT_ROOT) {
                    return;
                }
                uint64_t balance = sle->getFieldAmount(sfBalance).mantissa();
                if (balance < balanceTh) {
                    return;
                }
                AccountID account = sle->getAccountID(sfAccount);
                account_balance.emplace(toBase58(account), balance);
            });
            
            m_journal.info << "doBlanceDump running: account cnt " << account_balance.size();
            
            for (auto it : account_balance) {
                while (true) {
                    try {
                        const std::string &account = it.first;
                        uint64_t balance = it.second;
                        std::vector<std::string> children;
                        fetchChildren(account, children, -1);
                        uint64_t childrenBalance = 0;
                        for (auto child : children) {
                            if (account_balance.find(child) == account_balance.end()) {
                                continue;
                            }
                            uint64_t childBanlance = account_balance[child];
                            if (childBanlance > balanceT) {
                                childBanlance = balanceT + std::sqrt(childBanlance - balanceT);
                            }
                            childrenBalance += childBanlance;
                        }
                        
                        int64_t calcBalance = balance;
                        if (calcBalance > balanceT) {
                            calcBalance = balanceT + std::sqrt(calcBalance - balanceT);
                        }
                        
                        uint64_t coinAge = calcBalance + REFERRER_CONTRIBUTE_RATE * childrenBalance;
                        std::uint32_t t = getApp().getOPs().getNetworkTimeNC();
                        auto sql = boost::str(boost::format("REPLACE INTO AccountPower \
                                                            (DumpDate, Account, LedgerSeq, MainPower, VicePower, \
                                                            SanAge, Th, TempIndex, CreateTime) \
                                                            VALUES(\"%s\", \"%s\", %d, %d, %d, %d, %d, %d, %d)")
                                              % date.c_str() % account.c_str() % ledgerIndex % balance % childrenBalance
                                              % coinAge % balanceTh % balanceT % t);
                        *db << sql;
                    } catch (std::exception e) {
                        m_journal.warning << "doDumpBalance error: " << e.what();
                        if (dbReconnect()) {
                            continue;
                        } else {
                            m_journal.warning << "doDumpBalance dbReconnect fail";
                            break;
                        }
                    }
                    break;
                }
            }
            
            std::uint32_t t_done = getApp().getOPs().getNetworkTimeNC();
            auto sql_done = boost::str(boost::format("UPDATE PowerDumpLog SET RecordCount=%d, ModifyTime=%d, Status=%d WHERE DumpDate=\"%d\"") % account_balance.size() % t_done % 1 % date.c_str());
            m_journal.info << "doBlanceDump finished whit SQL: " << sql_done;
            *db << sql_done;
            
            return true;
        }
            
        Json::Value checkStatus(uint32_t ledgerIndex) override {
            Json::Value ret;
            Ledger::pointer ledger = nullptr;
            if (ledgerIndex > 0) {
                ledger = getApp().getLedgerMaster().getLedgerBySeq(ledgerIndex);
            } else {
                ledger = getApp().getLedgerMaster().getCurrentLedger();
            }
            if (!ledger) {
                return RPC::make_error(rpcLGR_NOT_FOUND, "no ledger for dividend object");
            }
            
            auto const& dividendObj = ledger->read(keylet::dividend());

            if (dividendObj) {
                //last dividend must done before this launch
                ret["incentive_object"] = dividendObj->getJson(0);
                ret["incentive_status"] = (dividendObj->getFieldU8(sfDividendState) == DividendMaster::DivType_Done) ? "done" : "doing";
            } else {
                ret["incentive_status"] = "unknow, maybe never incentived yet";
            }
            
            return ret;
        }

        bool calcDividend(uint32_t ledgerIndex, uint64_t dividendCoins) override {
            getApp().getJobQueue().addJob(jtCALC_DIVIDEND,
                                          "DividendMaster::doCalcDividend",
                                          std::bind (
                                                     &::ripple::DividendMasterImpl::doCalcDividend,
                                                     this, ledgerIndex, dividendCoins));
            return true;
        }
        
        bool doCalcDividend(uint32_t ledgerIndex, uint64_t dividendCoins) {
            if (!hasPrivateKey()) {
                return false;
            }
            std::lock_guard<std::mutex> lock(m_mutex);
            
            Ledger::pointer ledger = getApp().getLedgerMaster().getLedgerBySeq(ledgerIndex);
            if (!ledger) {
                if (m_journal.error) {
                    m_journal.error << "Ledger " << ledgerIndex << " not found.";
                }
                return false;
            }
            
            m_journal.info << "doCalcDividend begin - ledgerIndex: " << ledgerIndex;
            
            std::vector<std::string> days;
            getRecentDays(days, ledger->info().closeTime, DIVIDEND_CACULATE_CYCLE);
            std::string dividendDate = getDate(ledger->info().closeTime);
            
            auto db = getApp().getTxnDB().checkoutDb();
            
            //0-coin_age, 1-contribute_power, 2-dividend_index, 3-dividend_coins, 4-refer_power
            using DividendDetail = std::tuple<uint64_t, double, uint64_t, uint64_t, double>;
            std::map<std::string, DividendDetail> accountInfo;
            //fill balance of recent 7 days
            for (int i = 0; i < days.size(); i++) {
                boost::optional<std::string> acc;
                soci::indicator acci;
                boost::optional<uint64_t> cg;
                soci::indicator cgi;
                auto sql = boost::str(boost::format("SELECT Account, SanAge FROM AccountPower WHERE DumpDate=\"%s\"") % days[i]);
                soci::statement st = (db->prepare << sql, soci::into(acc, acci), soci::into(cg, cgi));
                st.execute();
                
                while (st.fetch()) {
                    if (acci == soci::i_ok && cgi == soci::i_ok) {
                        std::string acc_str = acc.value_or("");
                        uint64_t coinAge = cg.value_or(0);
                        if (acc_str != "" && coinAge > 0) {
                            AccountID accountID;
                            RPC::accountFromString(accountID, acc_str, true);
                            auto const sle = ledger->read(keylet::account(accountID));
                            uint64_t cur_blc = sle->getFieldAmount(sfBalance).mantissa();
                            if (cur_blc < DIVIDEND_BALANCE_MIN) {
                                continue;
                            }
                            if (accountInfo.find(acc_str) == accountInfo.end()) {
                                accountInfo.emplace(acc_str, DividendDetail());
                                std::get<0>(accountInfo[acc_str]) = 0;
                                std::get<1>(accountInfo[acc_str]) = 1;
                                std::get<2>(accountInfo[acc_str]) = 0;
                                std::get<3>(accountInfo[acc_str]) = 0;
                                std::get<4>(accountInfo[acc_str]) = 1;
                            }
                            //coin age
                            std::get<0>(accountInfo[acc_str]) += coinAge;
                        }
                    }
                }
            }
            
            uint64_t totalIndex = 0;
            for (auto &it : accountInfo) {
                const std::string &account = it.first;
                DividendDetail &dd = it.second;
                //fill contribute power
                std::pair<double, double> power = checkContributePower(account);
                std::get<1>(dd) = power.first;
                std::get<4>(dd) = power.second;
                //dividend index
                std::get<2>(dd) = std::get<0>(dd) * std::get<1>(dd) * std::get<4>(dd);
                totalIndex += std::get<2>(dd);
            }
            
            int account_cnt = 0;
            for (auto &it : accountInfo) {
                //real dividend coins
                std::get<3>(it.second) = (double)dividendCoins / totalIndex * std::get<2>(it.second);
                if (std::get<3>(it.second) > 0) {
                    account_cnt += 1;
                }
            }
            
            m_journal.info << "doCalcDividend running - ledgerIndex: " << ledgerIndex << " account cnt: " << account_cnt;
            
            std::string secret_key = get<std::string>(getConfig()[SECTION_DIVIDEND_ACCOUNT], "secret_key");
            RippleAddress secret = RippleAddress::createSeedGeneric(secret_key);
            RippleAddress generator = RippleAddress::createGeneratorPublic(secret);
            RippleAddress naAccountPrivate = RippleAddress::createAccountPrivate(generator, secret, 0);
            RippleAddress accountPublic = RippleAddress::createAccountPublic(generator, 0);
            //create dividend transaction for each account
            account_cnt = 0;
            for (auto it : accountInfo) {
                if (std::get<3>(it.second) == 0) {
                    continue;
                }
                account_cnt += 1;
                while (true) {
                    try {
                        STTx trans(ttDIVIDEND);
                        trans.setFieldU8(sfDividendType, DividendMaster::DivType_Apply);
                        trans.setFieldU32(sfDividendLedger, ledgerIndex);
                        trans.setFieldU32(sfFlags, tfFullyCanonicalSig);
                        trans.setAccountID(sfAccount, AccountID());
                        AccountID account = *parseBase58<AccountID>(it.first);
                        trans.setAccountID(sfDestination, account);
                        trans.setFieldU64(sfDividendCoins, std::get<3>(it.second));
                        trans.setFieldVL(sfSigningPubKey, accountPublic.getAccountPublic());

                        Serializer s;
                        trans.add (s);
                        //write dividend infomation into database
                        std::uint32_t t = getApp().getOPs().getNetworkTimeNC();
                        std::string txnStr = sqlEscape(s.peekData());
                        std::string sql = boost::str(boost::format("REPLACE INTO AccountIncentive (IncentiveDate, Account,  \
                                               LedgerSeq, ExcitationIndex, PartsPower, SanAge,                              \
                                               Status, Transaction, TotalIncentive, OwnIncentive,                           \
                                               TotalIndex, CreateTime, ModifyTime, ReferPower)                                          \
                                               VALUES(\"%s\", \"%s\", %d, %d, %f, %d, \"%c\", %s, %d, %d, %d, %d, %d, %f)")
                                                         % dividendDate                //DividendDate
                                                         % it.first                    //Account
                                                         % ledgerIndex                 //LedgerSeq
                                                         % std::get<2>(it.second)      //DividendIndex
                                                         % std::get<1>(it.second)      //Contribute
                                                         % std::get<0>(it.second)      //CoinAge
                                                         % 'W'                         //Status
                                                         % txnStr                      //Transaction
                                                         % dividendCoins               //TotalCoins
                                                         % std::get<3>(it.second)      //DividendCoins
                                                         % totalIndex                  //TotalIndex
                                                         % t                           //CreateTime
                                                         % t                           //ModifyTime
                                                         % std::get<4>(it.second)      //ReferPower
                                            ) ;
                        *db << sql;
                    } catch (std::exception e) {
                        m_journal.warning << "doCalcDividend error: " << e.what();
                        if (dbReconnect()) {
                            continue;
                        } else {
                            m_journal.warning << "doCalcDividend dbReconnect fail";
                            break;
                        }
                    }
                    break;
                }
            }
            m_journal.info << "doCalcDividend done - ledgerIndex: " << ledgerIndex << " account cnt: " << accountInfo.size();
            return true;
        }
        
        bool signAndRunTx(const std::string &acc_str, uint32_t ledgerIndex, uint64_t coins) override {
            std::string secret_key = get<std::string>(getConfig()[SECTION_DIVIDEND_ACCOUNT], "secret_key");
            RippleAddress secret = RippleAddress::createSeedGeneric(secret_key);
            RippleAddress generator = RippleAddress::createGeneratorPublic(secret);
            RippleAddress naAccountPrivate = RippleAddress::createAccountPrivate(generator, secret, 0);
            RippleAddress accountPublic = RippleAddress::createAccountPublic(generator, 0);

            STTx trans(ttDIVIDEND);
            trans.setFieldU8(sfDividendType, DividendMaster::DivType_Apply);
            trans.setFieldU32(sfDividendLedger, ledgerIndex);
            trans.setFieldU32(sfFlags, tfFullyCanonicalSig);
            trans.setAccountID(sfAccount, AccountID());
            AccountID account = *parseBase58<AccountID>(acc_str);
            trans.setAccountID(sfDestination, account);
            trans.setFieldU64(sfDividendCoins, coins);
            trans.setFieldVL(sfSigningPubKey, accountPublic.getAccountPublic());
            trans.sign(naAccountPrivate);
            
            try {
                getApp().getOPs().submitTransaction(std::make_shared<STTx>(trans));
                m_journal.info << "signAndRunTx - " << acc_str << "," << ledgerIndex << "," << coins << " ok.";
                return true;
            } catch (std::runtime_error& e) {
                m_journal.warning << "signAndRunTx - " << acc_str << "," << ledgerIndex << "," << coins << " Exception caught - " << e.what ();
                return false;
            }
        }
        
        bool dbReconnect() {
            if (getApp().getTxnDB ().startReconnection ()) {
                try {
                    //std::this_thread::sleep_for (std::chrono::seconds (3));
                    getApp().getTxnDB ().getSession ().reconnect ();
                    getApp().getTxnDB ().finishReconnection ();
                    m_journal.warning << "dbReconnect() Mysql reconncetion success";
                    return true;
                } catch ( std::exception const& e) {
                    m_journal.warning << "dbReconnect() Mysql has gone away. And reconnection failed. " << e.what();
                    getApp().getTxnDB ().finishReconnection ();
                    return false;
                }
            }
            return false;
        }
        
        bool launchDividend(uint32_t ledgerIndex, uint64_t dividendCoins) override {
            if (!hasPrivateKey()) {
                return false;
            }
            std::lock_guard<std::mutex> lock(m_mutex);
            //must sync with network ledger
            if (getApp().getOPs().isNeedNetworkLedger()) {
                m_journal.info << "launchDividend fail, not sync to network.";
                return false;
            }
            
            auto const& curLedger = getApp().getLedgerMaster().getCurrentLedger();
            auto const& dividendObj = curLedger->read(keylet::dividend());
            
            if (dividendObj) {
                m_journal.debug << dividendObj->getJson(1);
                //last dividend must done before this launch
                auto lastDivType = dividendObj->getFieldU8(sfDividendState);
                if (lastDivType != DividendMaster::DivType_Done) {
                    m_journal.info << "launchDividend fail, former dividend not finish yet.";
                    return false;
                }
                //last ledger index must smaller than this launch
                auto lastLedgerIndex = dividendObj->getFieldU32(sfDividendLedger);
                if (lastLedgerIndex >= ledgerIndex) {
                    m_journal.info << "launchDividend fail, luanch index cannot be smaller than last one.";
                    return false;
                }
            }
            
            std::string secret_key = get<std::string>(getConfig()[SECTION_DIVIDEND_ACCOUNT], "secret_key");
            RippleAddress secret = RippleAddress::createSeedGeneric(secret_key);
            RippleAddress generator = RippleAddress::createGeneratorPublic(secret);
            RippleAddress naAccountPrivate = RippleAddress::createAccountPrivate(generator, secret, 0);
            RippleAddress accountPublic = RippleAddress::createAccountPublic(generator, 0);
            
            //sign and submit a status change transaction
            std::shared_ptr<STTx> trans = std::make_shared<STTx>(ttDIVIDEND);
            trans->setFieldU8(sfDividendType, DividendMaster::DivType_Start);
            trans->setFieldU32(sfDividendLedger, ledgerIndex);
            trans->setFieldU32(sfFlags, tfFullyCanonicalSig);
            trans->setAccountID(sfAccount, AccountID());
            trans->setAccountID(sfDestination, AccountID());
            trans->setFieldU64(sfDividendCoins, dividendCoins);
//            trans->setFieldH256(sfDividendHash, base_uint<256>(0));
            trans->setFieldVL(sfSigningPubKey, accountPublic.getAccountPublic());
            trans->sign(naAccountPrivate);

            try {
                getApp().getOPs().submitTransaction(trans);
            } catch (std::runtime_error& e) {
                m_journal.debug << "Exception caught whe send dividend start transaction." << e.what ();
            }
            return true;
        }
            
        //check and run dividen transaction periodically
        void checkAndRunTxns() override {
            if (!hasPrivateKey()) {
                return;
            }
            std::lock_guard<std::mutex> lock(m_mutex);
            
            m_journal.info << "checkAndRunTxns begin";
            
            //must sync with network ledger
            if (getApp().getOPs().isNeedNetworkLedger()) {
                return;
            }
            
            auto const& curLedger = getApp().getLedgerMaster().getCurrentLedger();
            if (!curLedger) {
                return;
            }
            auto const& dividendObj = curLedger->read(keylet::dividend());
            
            //no div obj yet, waiting for first dividend
            if (!dividendObj) {
                return;
            }
            
            m_journal.info << "dividend_object: " << dividendObj->getText();
            
            //only when state is start, should trans send be enabled
            auto divType = dividendObj->getFieldU8(sfDividendState);
            if (divType != DividendMaster::DivType_Start) {
                return;
            }
            
            std::string secret_key = get<std::string>(getConfig ()[SECTION_DIVIDEND_ACCOUNT], "secret_key");
            RippleAddress secret = RippleAddress::createSeedGeneric(secret_key);
            RippleAddress generator = RippleAddress::createGeneratorPublic(secret);
            RippleAddress const& naAccountPrivate = RippleAddress::createAccountPrivate(generator, secret, 0);
            RippleAddress accountPublic = RippleAddress::createAccountPublic(generator, 0);

            //ledger index for current dividend
            auto ledgerIndex = dividendObj->getFieldU32(sfDividendLedger);
            auto db = getApp().getTxnDB().checkoutDb();
            //`LIMIT 256` means send 256 trans every round
            std::string sql = boost::str(boost::format("SELECT Account, Transaction FROM AccountIncentive \
                                           WHERE LedgerSeq=%d and Status=\'W\' LIMIT 256")
                                            % ledgerIndex);
            boost::optional<std::string> acc;
            soci::indicator acci;
            boost::optional<std::string> txn;
            soci::indicator txni;
            Blob rawTxn;
            
            int count = 0;
            soci::statement st = (db->prepare << sql, soci::into(acc, acci), soci::into(txn, txni));
            st.execute();
            while (st.fetch()) {
                count += 1;
                if (acci == soci::i_ok && txni == soci::i_ok) {
                    std::string account = acc.value_or("");
                    rawTxn.assign (txn->begin(), txn->end());
                    SerialIter it(makeSlice(rawTxn));
                    auto trans = std::make_shared<STTx>(it);
                    trans->sign(naAccountPrivate);
                    try {
                        getApp().getOPs().submitTransaction(trans);
                        //update table as done, `D` means done `W` means wait
                        std::uint32_t t = getApp().getOPs().getNetworkTimeNC();
                        std::string sql = boost::str(boost::format("UPDATE AccountIncentive SET Status=\'D\', ModifyTime=%d \
                                                                   WHERE Account=\"%s\" AND LedgerSeq=%d")
                                                     % t % account % ledgerIndex);
                        *db << sql;
                    } catch (std::runtime_error& e) {
                        m_journal.debug << "Exception caught whe send dividend apply transaction." << e.what ();
                    }
                }
            }
    
            uint64_t dividendCoins = dividendObj->getFieldU64(sfDividendCoins);
            //last round count < 256, finish sending transactions
            if (count < 256) {
                try {
                    std::shared_ptr<STTx> trans = std::make_shared<STTx>(ttDIVIDEND);
                    trans->setAccountID(sfAccount, AccountID());
                    trans->setFieldU8(sfDividendType, DividendMaster::DivType_Done);
                    trans->setFieldU32(sfDividendLedger, ledgerIndex);
                    trans->setFieldU32(sfFlags, tfFullyCanonicalSig);
                    trans->setFieldU64(sfDividendCoins, dividendCoins);
//                    trans->setFieldH256(sfDividendHash, base_uint<256>(0));
                    trans->setFieldVL(sfSigningPubKey, accountPublic.getAccountPublic());
                    trans->sign(naAccountPrivate);
                    getApp().getOPs().submitTransaction(trans);
                    m_journal.info << "dividend transactions send finish";
                } catch (std::runtime_error& e) {
                    m_journal.debug << "Exception caught whe send dividend done transaction." << e.what ();
                }
            }
            m_journal.info << "checkAndRunTxns end";
        }
            
    private:
        beast::Journal m_journal;
        std::mutex m_mutex;
            
        bool hasPrivateKey() {
            auto const& dividendAccount = getConfig()[SECTION_DIVIDEND_ACCOUNT];
            std::string secret_key = get<std::string>(dividendAccount, "secret_key");
            return !secret_key.empty();
        }
            
        std::string getDate(std::uint32_t t) {
            std::string date = boost::posix_time::to_iso_string(ptFromSeconds(t));
            std::vector<std::string> fields;
            boost::split (fields, date, boost::is_any_of("T"));
            return fields[0];
        }
        
        bool modifyContributePower(const std::string &account, double power) {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::uint32_t t = getApp().getOPs().getNetworkTimeNC();
            auto db = getApp().getTxnDB().checkoutDb();
            std::string sql = boost::str(boost::format("UPDATE AccountReference SET PartsPower=%f, ModifyTime=%d \
                                                       WHERE Account=\"%s\"")
                                         % power % t % account);
            *db << sql;
            
            return true;
        }
                
        void getRecentDays(std::vector<std::string> &days, std::uint32_t base_time, int count) {
            days.clear();
            for (int i = 1; i <= count; i++) {
                days.push_back(getDate(base_time - 24 * 3600 * i));
            }
            return;
        }
    };
            
    std::unique_ptr<DividendMaster> make_DividendMaster(beast::Journal journal) {
        return std::make_unique<DividendMasterImpl>(journal);
    }

}
