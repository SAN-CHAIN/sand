/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#import "TProtocol.h"
#import "TTransport.h"
#import "TProtocolFactory.h"


@interface TBinaryProtocol : NSObject <TProtocol> {
  id <TTransport> mTransport;
  BOOL mStrictRead;
  BOOL mStrictWrite;
  int32_t mMessageSizeLimit;
}

- (id) initWithTransport: (id <TTransport>) transport;

- (id) initWithTransport: (id <TTransport>) transport
              strictRead: (BOOL) strictRead
             strictWrite: (BOOL) strictWrite;

- (int32_t) messageSizeLimit;
- (void) setMessageSizeLimit: (int32_t) sizeLimit;

@end


@interface TBinaryProtocolFactory : NSObject <TProtocolFactory> {
}

+ (TBinaryProtocolFactory *) sharedFactory;

- (TBinaryProtocol *) newProtocolOnTransport: (id <TTransport>) transport;

@end
