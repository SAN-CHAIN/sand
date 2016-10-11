//------------  ------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2015 Ripple Labs Inc.

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
#include <ripple/ledger/Directory.h>

namespace ripple {

using const_iterator = Dir::const_iterator;

Dir::Dir(ReadView const& view,
        Keylet const& key)
    : view_(&view)
    , root_(key)
    , sle_(view_->read(root_))

{
    if (sle_ != nullptr)
        indexes_ = &sle_->getFieldV256(sfIndexes);
}

auto
Dir::begin() const ->
    const_iterator
{
    auto it = const_iterator(*view_, root_, root_);
    if (sle_ != nullptr)
    {
        it.sle_ = sle_;
        if (! indexes_->empty())
        {
            it.indexes_ = indexes_;
            it.it_ = std::begin(*indexes_);
            it.index_ = *it.it_;
        }
    }

    return it;
}

auto
Dir::end() const  ->
    const_iterator
{
    return const_iterator(*view_, root_, root_);
}

const_iterator
Dir::find(uint256 const& page_key, uint256 const& sle_key) const
{
    if (sle_ == nullptr)
        return end();

    auto it = const_iterator(*view_, root_, keylet::page(page_key, 0));
    if (root_.key == page_key)
    {
        it.sle_ = sle_;
        it.indexes_ = indexes_;
    }
    else
    {
        it.sle_ = view_->read(it.page_);
        if (it.sle_ == nullptr)
            return end();
        it.indexes_ = &it.sle_->getFieldV256(sfIndexes);
    }

    it.it_ = std::find(std::begin(*it.indexes_),
        std::end(*it.indexes_), sle_key);
    if (it.it_ == std::end(*it.indexes_))
        return end();

    it.index_ = *it.it_;
    return it;
}

const_iterator&
const_iterator::operator=(const_iterator const& other)
{
    if (this != &other)
    {
        view_ = other.view_;
        root_ = other.root_;
        page_ = other.page_;
        index_ = other.index_;
        cache_ = other.cache_;
        sle_ = other.sle_;
        indexes_ = other.indexes_;
        it_ = other.it_;
    }
    return *this;
}

bool
const_iterator::operator==(const_iterator const& other) const
{
    if (view_ == nullptr || other.view_ == nullptr)
        return false;

    assert(view_ == other.view_ && root_.key == other.root_.key);
    return page_.key == other.page_.key && index_ == other.index_;
}

const_iterator::reference
const_iterator::operator*() const
{
    assert(index_ != beast::zero);
    if (! cache_)
        cache_ = view_->read(keylet::child(index_));
    return *cache_;
}

const_iterator&
const_iterator::operator++()
{
    assert(index_ != beast::zero);
    if (++it_ != std::end(*indexes_))
    {
        index_ = *it_;
    }
    else
    {
        auto const next =
            sle_->getFieldU64(sfIndexNext);
        if (next == 0)
        {
            page_.key = root_.key;
            index_ = beast::zero;
        }
        else
        {
            page_ = keylet::page(root_, next);
            sle_ = view_->read(page_);
            assert(sle_);
            indexes_ = &sle_->getFieldV256(sfIndexes);
            if (indexes_->empty())
            {
                index_ = beast::zero;
            }
            else
            {
                it_ = std::begin(*indexes_);
                index_ = *it_;
            }
        }
    }

    cache_ = boost::none;
    return *this;
}

const_iterator
const_iterator::operator++(int)
{
    assert(index_ != beast::zero);
    const_iterator tmp(*this);
    ++(*this);
    return tmp;
}

} // ripple
