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

#include <BeastConfig.h>
#include <ripple/app/tx/impl/SetRegularKey.h>
#include <ripple/basics/Log.h>
#include <ripple/protocol/TxFlags.h>
#include <ripple/protocol/types.h>

namespace ripple {

std::uint64_t
SetRegularKey::calculateBaseFee ()
{
    auto const sle = view().peek(
        keylet::account(account_));
    if ( sle
            && (! (sle->getFlags () & lsfPasswordSpent))
            && (calcAccountID(mSigningPubKey) == account_))
    {
        // flag is armed and they signed with the right account
        return 0;
    }

    return Transactor::calculateBaseFee ();
}

TER
SetRegularKey::preCheck ()
{
    std::uint32_t const uTxFlags = mTxn.getFlags ();

    if (uTxFlags & tfUniversalMask)
    {
        if (j_.trace) j_.trace <<
            "Malformed transaction: Invalid flags set.";

        return temINVALID_FLAG;
    }

    return Transactor::preCheck ();
}

TER
SetRegularKey::doApply ()
{
    auto const sle = view().peek(
        keylet::account(account_));

    if (mFeeDue == zero)
        sle->setFlag (lsfPasswordSpent);

    if (mTxn.isFieldPresent (sfRegularKey))
    {
        sle->setAccountID (sfRegularKey,
            mTxn.getAccountID (sfRegularKey));
    }
    else
    {
        if (sle->isFlag (lsfDisableMaster))
            return tecMASTER_DISABLED;
        sle->makeFieldAbsent (sfRegularKey);
    }

    return tesSUCCESS;
}

}
