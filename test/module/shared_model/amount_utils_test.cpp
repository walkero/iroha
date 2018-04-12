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

#include <gtest/gtest.h>

#include "utils/amount_utils.hpp"

class AmountTest : public testing::Test {};

TEST_F(AmountTest, PlusTest) {
  iroha::expected::Result<std::shared_ptr<shared_model::interface::Amount>,
                          std::shared_ptr<std::string>>
      a = shared_model::builder::DefaultAmountBuilder::fromString("1234.567");

  iroha::expected::Result<std::shared_ptr<shared_model::interface::Amount>,
                          std::shared_ptr<std::string>>
      b = shared_model::builder::DefaultAmountBuilder::fromString("100");

  a.match(
      [&b](const iroha::expected::Value<
           std::shared_ptr<shared_model::interface::Amount>> &a_value) {
        b.match(
            [&a_value](const iroha::expected::Value<std::shared_ptr<
                           shared_model::interface::Amount>> &b_value) {
              auto c = *a_value.value + *b_value.value;
              c.match(
                  [](const auto &c_value) {
                    ASSERT_EQ(c_value.value->intValue(), 1334567);
                    ASSERT_EQ(c_value.value->precision(), 3);
                  },
                  [](const iroha::expected::Error<std::shared_ptr<std::string>>
                         &e) { FAIL() << *e.error; });
            },
            [](const iroha::expected::Error<std::shared_ptr<std::string>> &e) {
              FAIL() << *e.error;
            });
      },
      [](const iroha::expected::Error<std::shared_ptr<std::string>> &e) {
        FAIL() << *e.error;
      });
}

TEST_F(AmountTest, MinusTest) {
  iroha::expected::Result<std::shared_ptr<shared_model::interface::Amount>,
                          std::shared_ptr<std::string>>
      a = shared_model::builder::DefaultAmountBuilder::fromString("1234.567");

  iroha::expected::Result<std::shared_ptr<shared_model::interface::Amount>,
                          std::shared_ptr<std::string>>
      b = shared_model::builder::DefaultAmountBuilder::fromString("100");

  a.match(
      [&b](const iroha::expected::Value<
           std::shared_ptr<shared_model::interface::Amount>> &a_value) {
        b.match(
            [&a_value](const iroha::expected::Value<std::shared_ptr<
                           shared_model::interface::Amount>> &b_value) {
              auto c = *a_value.value - *b_value.value;
              c.match(
                  [](const auto &c_value) {
                    ASSERT_EQ(c_value.value->intValue(), 1134567);
                    ASSERT_EQ(c_value.value->precision(), 3);
                  },
                  [](const iroha::expected::Error<std::shared_ptr<std::string>>
                         &e) { FAIL() << *e.error; });
            },
            [](const iroha::expected::Error<std::shared_ptr<std::string>> &e) {
              FAIL() << *e.error;
            });
      },
      [](const iroha::expected::Error<std::shared_ptr<std::string>> &e) {
        FAIL() << *e.error;
      });
}

TEST_F(AmountTest, PrecisionTest) {
  iroha::expected::Result<std::shared_ptr<shared_model::interface::Amount>,
                          std::shared_ptr<std::string>>
      a = shared_model::builder::DefaultAmountBuilder::fromString("1234.567");

  a.match(
      [](const iroha::expected::Value<
          std::shared_ptr<shared_model::interface::Amount>> &a_value) {
        auto c = makePrecision(*a_value.value, 4);
        c.match(
            [](const iroha::expected::Value<
                std::shared_ptr<shared_model::interface::Amount>> &c_value) {
              ASSERT_EQ(c_value.value->intValue(), 12345670);
              ASSERT_EQ(c_value.value->precision(), 4);
            },
            [](const iroha::expected::Error<std::shared_ptr<std::string>> &e) {
              FAIL() << *e.error;
            });
      },
      [](const iroha::expected::Error<std::shared_ptr<std::string>> &e) {
        FAIL() << *e.error;
      });
}
