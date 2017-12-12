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

#include <gtest/gtest.h>
#include "merkletree/merkle_tree.h"
#include "cryptography/ed25519_sha3_impl/internal/sha3_hash.hpp"
#include "merkletree/generic_hasher.hpp"

using iroha::GenericHasher;
using iroha::sha3_256;

TEST(MerkleTree, root_of_same_sequence) {
  std::vector<std::string> sequence{
      "The", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog"};
  std::unique_ptr<merkletree::SerialHasher> hasher{new GenericHasher(sha3_256)};
  merkletree::MerkleTree tree{std::move(hasher)};
  for (auto &e : sequence) {
    tree.AddLeaf(e);
  }
  auto root = tree.CurrentRoot();

  // Same sequence
  std::vector<std::string> sequence2{
      "The", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog"};
  // Create another tree
  std::unique_ptr<merkletree::SerialHasher> hasher2{new GenericHasher(sha3_256)};
  merkletree::MerkleTree tree2{std::move(hasher2)};
  for (auto &e : sequence2) {
    tree2.AddLeaf(e);
  }
  auto root2 = tree2.CurrentRoot();
  EXPECT_EQ(root, root2);
}

TEST(MerkleTree, root_of_different_sequence) {
  std::vector<std::string> sequence{
      "The", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog"};
  std::unique_ptr<merkletree::SerialHasher> hasher{new GenericHasher(sha3_256)};
  merkletree::MerkleTree tree{std::move(hasher)};
  for (auto &e : sequence) {
    tree.AddLeaf(e);
  }
  auto root = tree.CurrentRoot();

  // Missing first element
  std::vector<std::string> sequence2{
      "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog"};
  // Create another tree
  std::unique_ptr<merkletree::SerialHasher> hasher2{new GenericHasher(sha3_256)};
  merkletree::MerkleTree tree2{std::move(hasher2)};
  for (auto &e : sequence2) {
    tree2.AddLeaf(e);
  }
  auto root2 = tree2.CurrentRoot();
  EXPECT_NE(root, root2);
}

TEST(MerkleTree, root_of_shuffled_sequence) {
  std::vector<std::string> sequence{
      "The", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog"};
  std::unique_ptr<merkletree::SerialHasher> hasher{new GenericHasher(sha3_256)};
  merkletree::MerkleTree tree{std::move(hasher)};
  for (auto &e : sequence) {
    tree.AddLeaf(e);
  }
  auto root = tree.CurrentRoot();

  // Missing first element
  std::vector<std::string> sequence2{
      "quick", "The", "lazy", "dog", "over", "the", "brown", "fox", "jumps"};
  // Create another tree
  std::unique_ptr<merkletree::SerialHasher> hasher2{new GenericHasher(sha3_256)};
  merkletree::MerkleTree tree2{std::move(hasher2)};
  for (auto &e : sequence2) {
    tree2.AddLeaf(e);
  }
  auto root2 = tree2.CurrentRoot();
  EXPECT_NE(root, root2);
}
