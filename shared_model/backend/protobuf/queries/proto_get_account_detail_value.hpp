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

#ifndef IROHA_PROTO_GET_ACCOUNT_DETAIL_VALUE_HPP
#define IROHA_PROTO_GET_ACCOUNT_DETAIL_VALUE_HPP

#include "interfaces/queries/get_account_detail_value.hpp"

#include "queries.pb.h"
#include "utils/lazy_initializer.hpp"
#include "utils/reference_holder.hpp"

namespace shared_model {
  namespace proto {
    class GetAccountDetailValue final
        : public CopyableProto<interface::GetAccountDetailValue,
                               iroha::protocol::Query,
                               GetAccountDetailValue> {
     public:
      template <typename QueryType>
      explicit GetAccountDetailValue(QueryType &&query)
          : CopyableProto(std::forward<QueryType>(query)) {}

      GetAccountDetailValue(const GetAccountDetailKeys &o)
          : GetAccountDetailValue(o.proto_) {}

      GetAccountDetailValue(GetAccountDetailValue &&o) noexcept
          : GetAccountDetailValue(std::move(o.proto_)) {}

      const interface::types::AccountIdType &accountId() const override {
        return account_detail_.account_id();
      }

      const std::string &key() const override {
        return account_detail_.key();
      }

     private:
      // ------------------------------| fields |-------------------------------

      const iroha::protocol::GetAccountDetailKeys &account_detail_value_{
          proto_->payload().get_account_detail_value()};
    };

  }  // namespace proto
}  // namespace shared_model

#endif  // IROHA_PROTO_GET_ACCOUNT_DETAIL_VALUE_HPP
