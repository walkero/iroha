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

#ifndef IROHA_GETACCOUNTDETAILVALUE_HPP
#define IROHA_GETACCOUNTDETAILVALUE_HPP

#include "interfaces/base/primitive.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  namespace interface {
    /**
     * Query for get all account's assets and balance
     */
    class GetAccountDetailValue : public ModelPrimitive<GetAccountDetailValue> {
     public:
      /**
       * @return account identifier
       */
      virtual const types::AccountIdType &accountId() const = 0;
      virtual const std::string &key() const = 0;

      std::string toString() const override {
        return detail::PrettyStringBuilder()
            .init("GetAccountDetailValue")
            .append("account_id", accountId())
            .append("key", key())
            .finalize();
      }

      bool operator==(const ModelType &rhs) const override {
        return accountId() == rhs.accountId() && key == rhs.key();
      }
    };
  }  // namespace interface
}  // namespace shared_model
#endif  // IROHA_GETACCOUNTDETAILVALUE_HPP
