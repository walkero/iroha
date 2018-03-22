/**
 * Copyright Soramitsu Co., Ltd. 2018 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "backend/protobuf/block.hpp"
#include "converters/json_conversion.hpp"
#include "converters/protobuf/json_proto_conversion.hpp"
#include "framework/result_fixture.hpp"
#include "interfaces/iroha_internal/block.hpp"

auto json = R"({
  "signatures": [],
  "payload": {
    "created_time": 0,
    "prev_block_hash": "0000000000000000000000000000000000000000000000000000000000000000",
    "height": 1,
    "tx_number": 1,
    "transactions": [
      {
        "signature": [],
        "payload": {
          "created_time": 0,
          "creator_account_id": "",
          "tx_counter": 0,
          "commands": [

          ]
        }
      }
    ]
  }
})";

using shared_model::interface::Block;
using namespace shared_model::conversion;

TEST(JsonConversionTest, JsonToBlock) {
  auto result =
      jsonToModel<Block>(json, proto::jsonToModel<shared_model::proto::Block>);
  result.match(
      [](const iroha::expected::Value<std::unique_ptr<Block>> &v) {
        ASSERT_EQ(v.value->height(), 1);
      },
      [](const iroha::expected::Error<std::string> &e) { FAIL(); });
}

TEST(JsonConversionTest, InvalidJsonToBlock) {
  auto result =
      jsonToModel<Block>("definitely not json", proto::jsonToModel<shared_model::proto::Block>);
  result.match(
      [](const iroha::expected::Value<std::unique_ptr<Block>> &v) {
        FAIL();
      },
      [](const iroha::expected::Error<std::string> &e) { SUCCEED(); });
}
