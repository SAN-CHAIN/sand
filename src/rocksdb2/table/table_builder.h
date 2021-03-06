//  Copyright (c) 2013, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#pragma once

namespace rocksdb {

class Slice;
class Status;

// TableBuilder provides the interface used to build a Table
// (an immutable and sorted map from keys to values).
//
// Multiple threads can invoke const methods on a TableBuilder without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same TableBuilder must use
// external synchronization.
class TableBuilder {
 public:
  // REQUIRES: Either Finish() or Abandon() has been called.
  virtual ~TableBuilder() {}

  // Add key,value to the table being constructed.
  // REQUIRES: key is after any previously added key according to comparator.
  // REQUIRES: Finish(), Abandon() have not been called
  virtual void Add(const Slice& key, const Slice& value) = 0;

  // Return non-ok iff some error has been detected.
  virtual Status status() const = 0;

  // Finish building the table.
  // REQUIRES: Finish(), Abandon() have not been called
  virtual Status Finish() = 0;

  // Indicate that the contents of this builder should be abandoned.
  // If the caller is not going to call Finish(), it must call Abandon()
  // before destroying this builder.
  // REQUIRES: Finish(), Abandon() have not been called
  virtual void Abandon() = 0;

  // Number of calls to Add() so far.
  virtual uint64_t NumEntries() const = 0;

  // Size of the file generated so far.  If invoked after a successful
  // Finish() call, returns the size of the final generated file.
  virtual uint64_t FileSize() const = 0;
};

}  // namespace rocksdb
