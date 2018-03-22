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

#ifndef IROHA_JSON_PROTO_CONVERSION_HPP
#define IROHA_JSON_PROTO_CONVERSION_HPP

#include "google/protobuf/util/json_util.h"

#include "common/result.hpp"

namespace shared_model {
  namespace conversion {
    namespace proto {
      /**
       * Converts json string into arbitrary protobuf object
       * @tparam T type of model which json converts to
       * @param json is the json string
       * @return optional of protobuf object which contains value if json
       * conversion was successful and none otherwise
       */
      template <typename T>
      iroha::expected::Result<T, std::string> jsonToProto(
          const std::string &json) {
        T result;
        auto status =
            google::protobuf::util::JsonStringToMessage(json, &result);
        if (status.ok()) {
          return iroha::expected::makeValue(result);
        }
        return iroha::expected::makeError(status.ToString());
      }

      /**
       * Converts json into arbitrary transaction shared model object
       * @tparam T type of shared model object converted from json
       * @param json is the json string containing protobuf object
       * @return result containing proto object or error message
       */
      template <typename T>
      iroha::expected::Result<T, std::string> jsonToModel(
          const std::string &json) {
        return jsonToProto<typename T::TransportType>(json) |
            [](auto &&tp) -> iroha::expected::Result<T, std::string> { return iroha::expected::makeValue(std::move(T(std::move(tp)))); };
      }
    }  // namespace proto
  }    // namespace conversion
}  // namespace shared_model

#endif  // IROHA_JSON_PROTO_CONVERSION_HPP
