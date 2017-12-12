/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
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

#ifndef IROHA_MERKLE_TREE_GENERIC_HASHER_HPP
#define IROHA_MERKLE_TREE_GENERIC_HASHER_HPP

#include "cryptography/ed25519_sha3_impl/internal/sha3_hash.hpp"
#include "merkletree/serial_hasher.h"



namespace iroha {

  using HashFunc = hash256_t (*)(const std::string &msg);
  /**
   * Generic Hasher is a class which creates implementation
   * of merkletree::SerialHasher, with hash function provided
   * during initialization
   */
  class GenericHasher : public merkletree::SerialHasher {
   public:
    GenericHasher(HashFunc hash_function) : hash_func{hash_function} {}

    virtual void Reset() { hash_ = hash_func("").to_string(); };

    virtual void Update(const std::string &data) {
      hash_ = hash_func(hash_ + data).to_string();
    }

    virtual std::string Final() {
      auto res = hash_;
      Reset();
      return res;
    }

    virtual size_t DigestSize() const { return hash256_t::size(); }

    virtual std::unique_ptr<SerialHasher> Create() const {
      return std::unique_ptr<SerialHasher>{new GenericHasher(hash_func)};
    }

   private:
    std::string hash_;
    HashFunc hash_func;
  };

}  // namespace iroha

#endif  // IROHA_MERKLE_TREE_GENERIC_HASHER_HPP
