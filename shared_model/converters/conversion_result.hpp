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

#ifndef IROHA_CONVERSION_RESULT_HPP
#define IROHA_CONVERSION_RESULT_HPP

#include <memory>
#include <string>

#include "common/result.hpp"

/**
 * Conversion result represents return type of all converters such as from json
 */
template <typename T>
using ConversionResult =
    iroha::expected::PolymorphicValueResult<T, std::string, std::unique_ptr<T>>;

#endif  // IROHA_CONVERSION_RESULT_HPP
