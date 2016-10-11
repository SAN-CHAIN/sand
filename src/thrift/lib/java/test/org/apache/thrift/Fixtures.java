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

package org.apache.thrift;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;

import thrift.test.Bonk;
import thrift.test.CompactProtoTestStruct;
import thrift.test.HolyMoley;
import thrift.test.Nesting;
import thrift.test.OneOfEach;

public class Fixtures {
  public static final OneOfEach oneOfEach;
  public static final Nesting nesting;
  public static final HolyMoley holyMoley;
  public static final CompactProtoTestStruct compactProtoTestStruct;

  // These byte arrays are serialized versions of the above structs.
  // They were serialized in binary protocol using thrift 0.6.x and are used to
  // test backwards compatibility with respect to the standard scheme.
  public static final byte[] persistentBytesOneOfEach = new byte[] { (byte) 0x02, (byte) 0x00,
    (byte) 0x01, (byte) 0x01, (byte) 0x02, (byte) 0x00, (byte) 0x02,
    (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x03, (byte) 0xD6,
    (byte) 0x06, (byte) 0x00, (byte) 0x04, (byte) 0x69, (byte) 0x78,
    (byte) 0x08, (byte) 0x00, (byte) 0x05, (byte) 0x01, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x0A, (byte) 0x00, (byte) 0x06,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x01, (byte) 0x65,
    (byte) 0xA0, (byte) 0xBC, (byte) 0x00, (byte) 0x04, (byte) 0x00,
    (byte) 0x07, (byte) 0x40, (byte) 0x09, (byte) 0x21, (byte) 0xFB,
    (byte) 0x54, (byte) 0x44, (byte) 0x2D, (byte) 0x18, (byte) 0x0B,
    (byte) 0x00, (byte) 0x08, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x0D, (byte) 0x4A, (byte) 0x53, (byte) 0x4F, (byte) 0x4E,
    (byte) 0x20, (byte) 0x54, (byte) 0x48, (byte) 0x49, (byte) 0x53,
    (byte) 0x21, (byte) 0x20, (byte) 0x22, (byte) 0x01, (byte) 0x0B,
    (byte) 0x00, (byte) 0x09, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x2E, (byte) 0xD3, (byte) 0x80, (byte) 0xE2, (byte) 0x85,
    (byte) 0xAE, (byte) 0xCE, (byte) 0x9D, (byte) 0x20, (byte) 0xD0,
    (byte) 0x9D, (byte) 0xCE, (byte) 0xBF, (byte) 0xE2, (byte) 0x85,
    (byte) 0xBF, (byte) 0xD0, (byte) 0xBE, (byte) 0xC9, (byte) 0xA1,
    (byte) 0xD0, (byte) 0xB3, (byte) 0xD0, (byte) 0xB0, (byte) 0xCF,
    (byte) 0x81, (byte) 0xE2, (byte) 0x84, (byte) 0x8E, (byte) 0x20,
    (byte) 0xCE, (byte) 0x91, (byte) 0x74, (byte) 0x74, (byte) 0xCE,
    (byte) 0xB1, (byte) 0xE2, (byte) 0x85, (byte) 0xBD, (byte) 0xCE,
    (byte) 0xBA, (byte) 0xEF, (byte) 0xBF, (byte) 0xBD, (byte) 0xE2,
    (byte) 0x80, (byte) 0xBC, (byte) 0x02, (byte) 0x00, (byte) 0x0A,
    (byte) 0x00, (byte) 0x0B, (byte) 0x00, (byte) 0x0B, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x06, (byte) 0x62, (byte) 0x61,
    (byte) 0x73, (byte) 0x65, (byte) 0x36, (byte) 0x34, (byte) 0x0F,
    (byte) 0x00, (byte) 0x0C, (byte) 0x03, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x03, (byte) 0x01, (byte) 0x02, (byte) 0x03,
    (byte) 0x0F, (byte) 0x00, (byte) 0x0D, (byte) 0x06, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x01,
    (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x03, (byte) 0x0F,
    (byte) 0x00, (byte) 0x0E, (byte) 0x0A, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x01,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x03, (byte) 0x00 };
  public static final byte[] persistentBytesNesting = new byte[] { (byte) 0x0C, (byte) 0x00,
    (byte) 0x01, (byte) 0x08, (byte) 0x00, (byte) 0x01, (byte) 0x00,
    (byte) 0x00, (byte) 0x7A, (byte) 0x69, (byte) 0x0B, (byte) 0x00,
    (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x13,
    (byte) 0x49, (byte) 0x20, (byte) 0x61, (byte) 0x6D, (byte) 0x20,
    (byte) 0x61, (byte) 0x20, (byte) 0x62, (byte) 0x6F, (byte) 0x6E,
    (byte) 0x6B, (byte) 0x2E, (byte) 0x2E, (byte) 0x2E, (byte) 0x20,
    (byte) 0x78, (byte) 0x6F, (byte) 0x72, (byte) 0x21, (byte) 0x00,
    (byte) 0x0C, (byte) 0x00, (byte) 0x02, (byte) 0x02, (byte) 0x00,
    (byte) 0x01, (byte) 0x01, (byte) 0x02, (byte) 0x00, (byte) 0x02,
    (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x03, (byte) 0xD6,
    (byte) 0x06, (byte) 0x00, (byte) 0x04, (byte) 0x69, (byte) 0x78,
    (byte) 0x08, (byte) 0x00, (byte) 0x05, (byte) 0x01, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x0A, (byte) 0x00, (byte) 0x06,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x01, (byte) 0x65,
    (byte) 0xA0, (byte) 0xBC, (byte) 0x00, (byte) 0x04, (byte) 0x00,
    (byte) 0x07, (byte) 0x40, (byte) 0x09, (byte) 0x21, (byte) 0xFB,
    (byte) 0x54, (byte) 0x44, (byte) 0x2D, (byte) 0x18, (byte) 0x0B,
    (byte) 0x00, (byte) 0x08, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x0D, (byte) 0x4A, (byte) 0x53, (byte) 0x4F, (byte) 0x4E,
    (byte) 0x20, (byte) 0x54, (byte) 0x48, (byte) 0x49, (byte) 0x53,
    (byte) 0x21, (byte) 0x20, (byte) 0x22, (byte) 0x01, (byte) 0x0B,
    (byte) 0x00, (byte) 0x09, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x2E, (byte) 0xD3, (byte) 0x80, (byte) 0xE2, (byte) 0x85,
    (byte) 0xAE, (byte) 0xCE, (byte) 0x9D, (byte) 0x20, (byte) 0xD0,
    (byte) 0x9D, (byte) 0xCE, (byte) 0xBF, (byte) 0xE2, (byte) 0x85,
    (byte) 0xBF, (byte) 0xD0, (byte) 0xBE, (byte) 0xC9, (byte) 0xA1,
    (byte) 0xD0, (byte) 0xB3, (byte) 0xD0, (byte) 0xB0, (byte) 0xCF,
    (byte) 0x81, (byte) 0xE2, (byte) 0x84, (byte) 0x8E, (byte) 0x20,
    (byte) 0xCE, (byte) 0x91, (byte) 0x74, (byte) 0x74, (byte) 0xCE,
    (byte) 0xB1, (byte) 0xE2, (byte) 0x85, (byte) 0xBD, (byte) 0xCE,
    (byte) 0xBA, (byte) 0xEF, (byte) 0xBF, (byte) 0xBD, (byte) 0xE2,
    (byte) 0x80, (byte) 0xBC, (byte) 0x02, (byte) 0x00, (byte) 0x0A,
    (byte) 0x00, (byte) 0x0B, (byte) 0x00, (byte) 0x0B, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x06, (byte) 0x62, (byte) 0x61,
    (byte) 0x73, (byte) 0x65, (byte) 0x36, (byte) 0x34, (byte) 0x0F,
    (byte) 0x00, (byte) 0x0C, (byte) 0x03, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x03, (byte) 0x01, (byte) 0x02, (byte) 0x03,
    (byte) 0x0F, (byte) 0x00, (byte) 0x0D, (byte) 0x06, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x01,
    (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x03, (byte) 0x0F,
    (byte) 0x00, (byte) 0x0E, (byte) 0x0A, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x01,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x03, (byte) 0x00, (byte) 0x00 };
  public static final byte[] persistentBytesHolyMoley = new byte[] { (byte) 0x0F, (byte) 0x00,
    (byte) 0x01, (byte) 0x0C, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x02, (byte) 0x02, (byte) 0x00, (byte) 0x01, (byte) 0x01,
    (byte) 0x02, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x03,
    (byte) 0x00, (byte) 0x03, (byte) 0x23, (byte) 0x06, (byte) 0x00,
    (byte) 0x04, (byte) 0x69, (byte) 0x78, (byte) 0x08, (byte) 0x00,
    (byte) 0x05, (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x0A, (byte) 0x00, (byte) 0x06, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x01, (byte) 0x65, (byte) 0xA0, (byte) 0xBC,
    (byte) 0x00, (byte) 0x04, (byte) 0x00, (byte) 0x07, (byte) 0x40,
    (byte) 0x09, (byte) 0x21, (byte) 0xFB, (byte) 0x54, (byte) 0x44,
    (byte) 0x2D, (byte) 0x18, (byte) 0x0B, (byte) 0x00, (byte) 0x08,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x0D, (byte) 0x4A,
    (byte) 0x53, (byte) 0x4F, (byte) 0x4E, (byte) 0x20, (byte) 0x54,
    (byte) 0x48, (byte) 0x49, (byte) 0x53, (byte) 0x21, (byte) 0x20,
    (byte) 0x22, (byte) 0x01, (byte) 0x0B, (byte) 0x00, (byte) 0x09,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x2E, (byte) 0xD3,
    (byte) 0x80, (byte) 0xE2, (byte) 0x85, (byte) 0xAE, (byte) 0xCE,
    (byte) 0x9D, (byte) 0x20, (byte) 0xD0, (byte) 0x9D, (byte) 0xCE,
    (byte) 0xBF, (byte) 0xE2, (byte) 0x85, (byte) 0xBF, (byte) 0xD0,
    (byte) 0xBE, (byte) 0xC9, (byte) 0xA1, (byte) 0xD0, (byte) 0xB3,
    (byte) 0xD0, (byte) 0xB0, (byte) 0xCF, (byte) 0x81, (byte) 0xE2,
    (byte) 0x84, (byte) 0x8E, (byte) 0x20, (byte) 0xCE, (byte) 0x91,
    (byte) 0x74, (byte) 0x74, (byte) 0xCE, (byte) 0xB1, (byte) 0xE2,
    (byte) 0x85, (byte) 0xBD, (byte) 0xCE, (byte) 0xBA, (byte) 0xEF,
    (byte) 0xBF, (byte) 0xBD, (byte) 0xE2, (byte) 0x80, (byte) 0xBC,
    (byte) 0x02, (byte) 0x00, (byte) 0x0A, (byte) 0x00, (byte) 0x0B,
    (byte) 0x00, (byte) 0x0B, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x06, (byte) 0x62, (byte) 0x61, (byte) 0x73, (byte) 0x65,
    (byte) 0x36, (byte) 0x34, (byte) 0x0F, (byte) 0x00, (byte) 0x0C,
    (byte) 0x03, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x03,
    (byte) 0x01, (byte) 0x02, (byte) 0x03, (byte) 0x0F, (byte) 0x00,
    (byte) 0x0D, (byte) 0x06, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x03, (byte) 0x00, (byte) 0x01, (byte) 0x00, (byte) 0x02,
    (byte) 0x00, (byte) 0x03, (byte) 0x0F, (byte) 0x00, (byte) 0x0E,
    (byte) 0x0A, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x03,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x01, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x03, (byte) 0x00,
    (byte) 0x02, (byte) 0x00, (byte) 0x01, (byte) 0x01, (byte) 0x02,
    (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x03, (byte) 0x00,
    (byte) 0x03, (byte) 0xD6, (byte) 0x06, (byte) 0x00, (byte) 0x04,
    (byte) 0x69, (byte) 0x78, (byte) 0x08, (byte) 0x00, (byte) 0x05,
    (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x0A,
    (byte) 0x00, (byte) 0x06, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x01, (byte) 0x65, (byte) 0xA0, (byte) 0xBC, (byte) 0x00,
    (byte) 0x04, (byte) 0x00, (byte) 0x07, (byte) 0x40, (byte) 0x09,
    (byte) 0x21, (byte) 0xFB, (byte) 0x54, (byte) 0x44, (byte) 0x2D,
    (byte) 0x18, (byte) 0x0B, (byte) 0x00, (byte) 0x08, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x0D, (byte) 0x4A, (byte) 0x53,
    (byte) 0x4F, (byte) 0x4E, (byte) 0x20, (byte) 0x54, (byte) 0x48,
    (byte) 0x49, (byte) 0x53, (byte) 0x21, (byte) 0x20, (byte) 0x22,
    (byte) 0x01, (byte) 0x0B, (byte) 0x00, (byte) 0x09, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x2E, (byte) 0xD3, (byte) 0x80,
    (byte) 0xE2, (byte) 0x85, (byte) 0xAE, (byte) 0xCE, (byte) 0x9D,
    (byte) 0x20, (byte) 0xD0, (byte) 0x9D, (byte) 0xCE, (byte) 0xBF,
    (byte) 0xE2, (byte) 0x85, (byte) 0xBF, (byte) 0xD0, (byte) 0xBE,
    (byte) 0xC9, (byte) 0xA1, (byte) 0xD0, (byte) 0xB3, (byte) 0xD0,
    (byte) 0xB0, (byte) 0xCF, (byte) 0x81, (byte) 0xE2, (byte) 0x84,
    (byte) 0x8E, (byte) 0x20, (byte) 0xCE, (byte) 0x91, (byte) 0x74,
    (byte) 0x74, (byte) 0xCE, (byte) 0xB1, (byte) 0xE2, (byte) 0x85,
    (byte) 0xBD, (byte) 0xCE, (byte) 0xBA, (byte) 0xEF, (byte) 0xBF,
    (byte) 0xBD, (byte) 0xE2, (byte) 0x80, (byte) 0xBC, (byte) 0x02,
    (byte) 0x00, (byte) 0x0A, (byte) 0x00, (byte) 0x0B, (byte) 0x00,
    (byte) 0x0B, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x06,
    (byte) 0x62, (byte) 0x61, (byte) 0x73, (byte) 0x65, (byte) 0x36,
    (byte) 0x34, (byte) 0x0F, (byte) 0x00, (byte) 0x0C, (byte) 0x03,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x03, (byte) 0x01,
    (byte) 0x02, (byte) 0x03, (byte) 0x0F, (byte) 0x00, (byte) 0x0D,
    (byte) 0x06, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x03,
    (byte) 0x00, (byte) 0x01, (byte) 0x00, (byte) 0x02, (byte) 0x00,
    (byte) 0x03, (byte) 0x0F, (byte) 0x00, (byte) 0x0E, (byte) 0x0A,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x03, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x02,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x0E,
    (byte) 0x00, (byte) 0x02, (byte) 0x0F, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x03, (byte) 0x0B, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x0B, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x03, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x0F, (byte) 0x74, (byte) 0x68, (byte) 0x65, (byte) 0x6E,
    (byte) 0x20, (byte) 0x61, (byte) 0x20, (byte) 0x6F, (byte) 0x6E,
    (byte) 0x65, (byte) 0x2C, (byte) 0x20, (byte) 0x74, (byte) 0x77,
    (byte) 0x6F, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x06,
    (byte) 0x74, (byte) 0x68, (byte) 0x72, (byte) 0x65, (byte) 0x65,
    (byte) 0x21, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x06,
    (byte) 0x46, (byte) 0x4F, (byte) 0x55, (byte) 0x52, (byte) 0x21,
    (byte) 0x21, (byte) 0x0B, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x09,
    (byte) 0x61, (byte) 0x6E, (byte) 0x64, (byte) 0x20, (byte) 0x61,
    (byte) 0x20, (byte) 0x6F, (byte) 0x6E, (byte) 0x65, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x09, (byte) 0x61, (byte) 0x6E,
    (byte) 0x64, (byte) 0x20, (byte) 0x61, (byte) 0x20, (byte) 0x74,
    (byte) 0x77, (byte) 0x6F, (byte) 0x0D, (byte) 0x00, (byte) 0x03,
    (byte) 0x0B, (byte) 0x0F, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x03, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x03,
    (byte) 0x74, (byte) 0x77, (byte) 0x6F, (byte) 0x0C, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x02, (byte) 0x08, (byte) 0x00,
    (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x01,
    (byte) 0x0B, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x05, (byte) 0x57, (byte) 0x61, (byte) 0x69,
    (byte) 0x74, (byte) 0x2E, (byte) 0x00, (byte) 0x08, (byte) 0x00,
    (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x02,
    (byte) 0x0B, (byte) 0x00, (byte) 0x02, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x05, (byte) 0x57, (byte) 0x68, (byte) 0x61,
    (byte) 0x74, (byte) 0x3F, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x05, (byte) 0x74, (byte) 0x68, (byte) 0x72,
    (byte) 0x65, (byte) 0x65, (byte) 0x0C, (byte) 0x00, (byte) 0x00,
    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x04, (byte) 0x7A, (byte) 0x65, (byte) 0x72, (byte) 0x6F,
    (byte) 0x0C, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    (byte) 0x00 };

  private static final byte[] kUnicodeBytes = { (byte) 0xd3, (byte) 0x80,
      (byte) 0xe2, (byte) 0x85, (byte) 0xae, (byte) 0xce, (byte) 0x9d,
      (byte) 0x20, (byte) 0xd0, (byte) 0x9d, (byte) 0xce, (byte) 0xbf,
      (byte) 0xe2, (byte) 0x85, (byte) 0xbf, (byte) 0xd0, (byte) 0xbe,
      (byte) 0xc9, (byte) 0xa1, (byte) 0xd0, (byte) 0xb3, (byte) 0xd0,
      (byte) 0xb0, (byte) 0xcf, (byte) 0x81, (byte) 0xe2, (byte) 0x84,
      (byte) 0x8e, (byte) 0x20, (byte) 0xce, (byte) 0x91, (byte) 0x74,
      (byte) 0x74, (byte) 0xce, (byte) 0xb1, (byte) 0xe2, (byte) 0x85,
      (byte) 0xbd, (byte) 0xce, (byte) 0xba, (byte) 0x83, (byte) 0xe2,
      (byte) 0x80, (byte) 0xbc };

  static {
    try {
      oneOfEach = new OneOfEach();
      oneOfEach.setIm_true(true);
      oneOfEach.setIm_false(false);
      oneOfEach.setA_bite((byte) 0xd6);
      oneOfEach.setInteger16((short) 27000);
      oneOfEach.setInteger32(1 << 24);
      oneOfEach.setInteger64((long) 6000 * 1000 * 1000);
      oneOfEach.setDouble_precision(Math.PI);
      oneOfEach.setSome_characters("JSON THIS! \"\1");
      oneOfEach.setZomg_unicode(new String(kUnicodeBytes, "UTF-8"));
      oneOfEach.setBase64(ByteBuffer.wrap("base64".getBytes()));
      // byte, i16, and i64 lists are populated by default constructor

      Bonk bonk = new Bonk();
      bonk.setType(31337);
      bonk.setMessage("I am a bonk... xor!");
      nesting = new Nesting(bonk, oneOfEach);

      holyMoley = new HolyMoley();
      ArrayList big = new ArrayList<OneOfEach>();
      big.add(new OneOfEach(oneOfEach));
      big.add(nesting.my_ooe);
      holyMoley.setBig(big);
      holyMoley.getBig().get(0).setA_bite((byte) 0x22);
      holyMoley.getBig().get(0).setA_bite((byte) 0x23);

      holyMoley.setContain(new HashSet<List<String>>());
      ArrayList<String> stage1 = new ArrayList<String>(2);
      stage1.add("and a one");
      stage1.add("and a two");
      holyMoley.getContain().add(stage1);
      stage1 = new ArrayList<String>(3);
      stage1.add("then a one, two");
      stage1.add("three!");
      stage1.add("FOUR!!");
      holyMoley.getContain().add(stage1);
      stage1 = new ArrayList<String>(0);
      holyMoley.getContain().add(stage1);

      ArrayList<Bonk> stage2 = new ArrayList<Bonk>();
      holyMoley.setBonks(new HashMap<String, List<Bonk>>());
      // one empty
      holyMoley.getBonks().put("zero", stage2);

      // one with two
      stage2 = new ArrayList<Bonk>();
      Bonk b = new Bonk();
      b.setType(1);
      b.setMessage("Wait.");
      stage2.add(b);
      b = new Bonk();
      b.setType(2);
      b.setMessage("What?");
      stage2.add(b);
      holyMoley.getBonks().put("two", stage2);

      // one with three
      stage2 = new ArrayList<Bonk>();
      b = new Bonk();
      b.setType(3);
      b.setMessage("quoth");
      b = new Bonk();
      b.setType(4);
      b.setMessage("the raven");
      b = new Bonk();
      b.setType(5);
      b.setMessage("nevermore");
      holyMoley.getBonks().put("three", stage2);

      // superhuge compact proto test struct
      compactProtoTestStruct = new CompactProtoTestStruct(
          thrift.test.DebugProtoTestConstants.COMPACT_TEST);
      compactProtoTestStruct.setA_binary(ByteBuffer.wrap(new byte[] { 0, 1, 2,
          3, 4, 5, 6, 7, 8 }));
    } catch (Exception e) {
      throw new RuntimeException(e);
    }
  }
}
