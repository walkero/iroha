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

#ifndef IROHA_JSON_CONVERSION_HPP
#define IROHA_JSON_CONVERSION_HPP

#include <utility>

#include "converters/conversion_result.hpp"

namespace shared_model {
  namespace conversion {
    /**
     * Converts json into arbitrary shared model object
     * @tparam T type of shared model object converted from json
     * @param json is the json string containing protobuf object
     * @return result containing proto object or error message
     */
    template <typename T, typename Converter>
    ConversionResult<T> jsonToModel(const std::string &json, Converter &&c) {
      return c(json) | [](auto &&object) -> ConversionResult<T> {
        return iroha::expected::makeValue(std::unique_ptr<T>(object.copy()));
      };
    }
  }  // namespace conversion
}  // namespace shared_model

#endif  // IROHA_JSON_CONVERSION_HPP
