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

#ifndef IROHA_TRANSPORT_BUILDER_HPP
#define IROHA_TRANSPORT_BUILDER_HPP

#include "backend/protobuf/block.hpp"
#include "backend/protobuf/empty_block.hpp"
#include "common/result.hpp"
#include "interfaces/common_objects/types.hpp"
#include "utils/polymorphic_wrapper.hpp"
#include "validators/default_validator.hpp"

namespace shared_model {
  namespace proto {

    /**
     * Class for building any shared model objects from corresponding transport
     * representation (e.g. protobuf object)
     * @tparam T Build type
     * @tparam SV Stateless validator type
     */
    template <typename T, typename SV>
    class TransportBuilder {
     public:
      TransportBuilder(const SV &validator = SV())
          : stateless_validator_(validator) {}

      /**
       * Builds result from transport object
       * @return value if transport object is valid and error message otherwise
       */
      iroha::expected::Result<T, std::string> build(
          typename T::TransportType transport) {
        auto result = T(transport);
        auto answer = stateless_validator_.validate(result);
        if (answer.hasErrors()) {
          return iroha::expected::makeError(answer.reason());
        }
        return iroha::expected::makeValue(T(std::move(transport)));
      }

     private:
      SV stateless_validator_;
    };

    template <typename SV>
    class TransportBuilder<interface::BlockVariantType, SV> {
     public:
      TransportBuilder<interface::BlockVariantType, SV>(
          SV stateless_validator = SV())
          : stateless_validator_(stateless_validator) {}

      iroha::expected::Result<interface::BlockVariantType, std::string> build(
          iroha::protocol::Block transport) {
        if (transport.payload().transactions().size() == 0) {
          auto result = shared_model::proto::EmptyBlock(transport);
          auto answer = stateless_validator_.validate(result);
          if (answer.hasErrors()) {
            return iroha::expected::makeError(answer.reason());
          }
          return iroha::expected::makeValue(
              std::make_shared<proto::EmptyBlock>(std::move(transport)));
        } else {
          auto result = shared_model::proto::Block(transport);
          auto answer = stateless_validator_.validate(result);
          if (answer.hasErrors()) {
            return iroha::expected::makeError(answer.reason());
          }
          return iroha::expected::makeValue(
              std::make_shared<proto::Block>(std::move(transport)));
        }
      }

     private:
      SV stateless_validator_;
    };

  }  // namespace proto
}  // namespace shared_model
#endif  // IROHA_TRANSPORT_BUILDER_HPP
