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

#ifndef RIPPLE_APP_LEDGER_OPENLEDGER_H_INCLUDED
#define RIPPLE_APP_LEDGER_OPENLEDGER_H_INCLUDED

#include <ripple/app/ledger/Ledger.h>
#include <ripple/ledger/CachedSLEs.h>
#include <ripple/ledger/OpenView.h>
#include <ripple/app/misc/CanonicalTXSet.h>
#include <ripple/app/misc/IHashRouter.h>
#include <ripple/basics/Log.h>
#include <ripple/basics/UnorderedContainers.h>
#include <ripple/core/Config.h>
#include <beast/container/aged_unordered_map.h>
#include <beast/utility/Journal.h>
#include <cassert>
#include <mutex>

namespace ripple {

// How many total extra passes we make
// We must ensure we make at least one non-retriable pass
#define LEDGER_TOTAL_PASSES 3

// How many extra retry passes we
// make if the previous retry pass made changes
#define LEDGER_RETRY_PASSES 1

using OrderedTxs = CanonicalTXSet;

//------------------------------------------------------------------------------

/** Represents the open ledger. */
class OpenLedger
{
private:
    beast::Journal j_;
    CachedSLEs& cache_;
    Config const& config_;
    std::mutex mutable modify_mutex_;
    std::mutex mutable current_mutex_;
    std::shared_ptr<OpenView const> current_;

public:
    OpenLedger() = delete;
    OpenLedger (OpenLedger const&) = delete;
    OpenLedger& operator= (OpenLedger const&) = delete;

    /** Create a new open ledger object.

        @param ledger A closed ledger
    */
    explicit
    OpenLedger (std::shared_ptr<
        Ledger const> const& ledger,
            Config const& config, CachedSLEs& cache,
                beast::Journal journal);

    /** Returns a view to the current open ledger.

        Thread safety:
            Can be called concurrently from any thread.

        Effects:
            The caller is given ownership of a
            non-modifiable snapshot of the open ledger
            at the time of the call.
    */
    std::shared_ptr<ReadView const>
    current() const;

    /** Modify the open ledger

        Thread safety:
            Can be called concurrently from any thread.

        `f` will be called as
            bool(ReadView&)

        If `f` returns `true`, the changes made in the
        OpenView will be published to the open ledger.

        @return `true` if the open view was changed
    */
    bool
    modify (std::function<
        bool(OpenView&, beast::Journal)> const& f);

    /** Accept a new ledger.

        Thread safety:
            Can be called concurrently from any thread.

        Effects:

            A new open view based on the accepted ledger
            is created, and the list of retriable
            transactions is optionally applied first
            depending on the value of `retriesFirst`.

            The transactions in the current open view
            are applied to the new open view.

            The list of local transactions are applied
            to the new open view.

            Any failed, retriable transactions are left
            in `retries` for the caller.

            The current view is atomically set to the
            new open view.

        @param ledger A new closed ledger
    */
    void
    accept(std::shared_ptr<Ledger const> const& ledger,
        OrderedTxs const& locals, bool retriesFirst,
            OrderedTxs& retries, ApplyFlags flags,
                IHashRouter& router,
                    std::string const& suffix = "");

    /** Algorithm for applying transactions.

        This has the retry logic and ordering semantics
        used for consensus and building the open ledger.
    */
    template <class FwdRange>
    static
    void
    apply (OpenView& view, ReadView const& check,
        FwdRange const& txs, OrderedTxs& retries,
            ApplyFlags flags, IHashRouter& router,
                Config const& config, beast::Journal j);

private:
    enum Result
    {
        success,
        failure,
        retry
    };

    std::shared_ptr<OpenView>
    create (std::shared_ptr<
        Ledger const> const& ledger);

    static
    Result
    apply_one (OpenView& view, std::shared_ptr<
        STTx const> const& tx, bool retry,
            ApplyFlags flags, IHashRouter& router,
                Config const& config, beast::Journal j);

public:    
    //--------------------------------------------------------------------------
    //
    // TEST CODE
    //
    // Verify that the open ledger has the right contents
    // This is called while holding the master and ledger master mutexes
    bool
    verify (Ledger const& ledger,
        std::string const& suffix = "") const;
};

//------------------------------------------------------------------------------

template <class FwdRange>
void
OpenLedger::apply (OpenView& view,
    ReadView const& check, FwdRange const& txs,
        OrderedTxs& retries, ApplyFlags flags,
            IHashRouter& router, Config const& config,
                beast::Journal j)
{
    for (auto iter = txs.begin();
        iter != txs.end(); ++iter)
    {
        try
        {
            // Dereferencing the iterator can
            // throw since it may be transformed.
            auto const tx = *iter;
            if (check.txExists(tx->getTransactionID()))
                continue;
            auto const result = apply_one(view,
                tx, true, flags, router, config, j);
            if (result == Result::retry)
                retries.insert(tx);
        }
        catch(...)
        {
            JLOG(j.error) <<
                "Caught exception";
        }
    }
    bool retry = true;
    for (int pass = 0;
        pass < LEDGER_TOTAL_PASSES;
            ++pass)
    {
        int changes = 0;
        auto iter = retries.begin();
        while (iter != retries.end())
        {
            switch (apply_one(view,
                iter->second, retry, flags,
                    router, config, j))
            {
            case Result::success:
                ++changes;
            case Result::failure:
                iter = retries.erase (iter);
                break;
            case Result::retry:
                ++iter;
            }
        }
        // A non-retry pass made no changes
        if (! changes && ! retry)
            return;
        // Stop retriable passes
        if (! changes || (pass >= LEDGER_RETRY_PASSES))
            retry = false;
    }

    // If there are any transactions left, we must have
    // tried them in at least one final pass
    assert (retries.empty() || ! retry);
}

//------------------------------------------------------------------------------

// For debug logging

std::string
debugTxstr (std::shared_ptr<STTx const> const& tx);

std::string
debugTostr (OrderedTxs const& set);

std::string
debugTostr (SHAMap const& set);

std::string
debugTostr (std::shared_ptr<ReadView const> const& view);

} // ripple

#endif
