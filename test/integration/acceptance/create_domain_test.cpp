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
#include "backend/protobuf/transaction.hpp"
#include "cryptography/crypto_provider/crypto_defaults.hpp"
#include "datetime/time.hpp"
#include "framework/base_tx.hpp"
#include "framework/integration_framework/integration_test_framework.hpp"
#include "module/shared_model/builders/protobuf/test_transaction_builder.hpp"
#include "validators/permissions.hpp"

using namespace std::string_literals;
using namespace integration_framework;
using namespace shared_model;

class CreateDomain : public ::testing::Test {
 public:
  /**
   * Creates the transaction with the user creation commands
   * @param perms are the permissions of the user
   * @return built tx
   */
  auto makeUserWithPerms(const std::vector<std::string> &perms = {
                             shared_model::permissions::can_create_domain}) {
    return framework::createUserWithPerms(
               kUser, kUserKeypair.publicKey(), kRole, perms)
        .build()
        .signAndAddSignature(kAdminKeypair);
  }

  /**
   * Create valid base pre-built transaction
   * @return pre-built tx
   */
  auto baseTx() {
    return TestUnsignedTransactionBuilder()
        .creatorAccountId(kUserId)
        .createdTime(iroha::time::now());
  }

  /**
   * Completes pre-built transaction
   * @param builder is a pre-built tx
   * @return built tx
   */
  template <typename TestTransactionBuilder>
  auto completeTx(TestTransactionBuilder builder) {
    return builder.build().signAndAddSignature(kUserKeypair);
  }

  const std::function<void(const shared_model::proto::TransactionResponse &)>
      checkStatelessInvalid = [](auto &status) {
        ASSERT_NO_THROW(
            boost::get<shared_model::detail::PolymorphicWrapper<
                shared_model::interface::StatelessFailedTxResponse>>(
                status.get()));
      };

  const std::string kRole = "role"s;
  const std::string kUser = "user"s;
  const std::string kNewDomain = "newdomain";
  const std::string kUserId = kUser + "@test";
  const crypto::Keypair kAdminKeypair =
      crypto::DefaultCryptoAlgorithmType::generateKeypair();
  const crypto::Keypair kUserKeypair =
      crypto::DefaultCryptoAlgorithmType::generateKeypair();
};

/**
 * @given some user with can_create_domain permission
 * @when execute tx with CreateDomain command
 * @then there is the tx in proposal
 */
TEST_F(CreateDomain, Basic) {
  IntegrationTestFramework(1)
      .setInitialState(kAdminKeypair)
      .sendTx(makeUserWithPerms())
      .skipProposal()
      .skipBlock()
      .sendTx(completeTx(baseTx().createDomain(kNewDomain, kRole)))
      .skipProposal()
      .checkBlock(
          [](auto &block) { ASSERT_EQ(block->transactions().size(), 1); })
      .done();
}

/**
 * @given some user without can_create_domain permission
 * @when execute tx with CreateDomain command
 * @then there is no tx in proposal
 */
TEST_F(CreateDomain, NoPermissions) {
  IntegrationTestFramework(1)
      .setInitialState(kAdminKeypair)
      .sendTx(makeUserWithPerms({shared_model::permissions::can_get_my_txs}))
      .skipProposal()
      .skipBlock()
      .sendTx(completeTx(baseTx().createDomain(kNewDomain, kRole)))
      .skipProposal()
      .checkBlock(
          [](auto &block) { ASSERT_EQ(block->transactions().size(), 0); })
      .done();
}

/**
 * @given some user with can_create_domain permission
 * @when execute tx with CreateDomain command with nonexistent role
 * @then there is no tx in proposal
 */
TEST_F(CreateDomain, NoRole) {
  const std::string nonexistent_role = "asdf"s;
  IntegrationTestFramework(1)
      .setInitialState(kAdminKeypair)
      .sendTx(makeUserWithPerms())
      .skipProposal()
      .skipBlock()
      .sendTx(completeTx(baseTx().createDomain(kNewDomain, nonexistent_role)))
      .skipProposal()
      .checkBlock(
          [](auto &block) { ASSERT_EQ(block->transactions().size(), 0); })
      .done();
}

/**
 * @given some user with can_create_domain permission
 * @when execute tx with CreateDomain command with already existing domain
 * @then there is no tx in proposal
 */
TEST_F(CreateDomain, ExistingName) {
  const std::string &existing_domain = IntegrationTestFramework::kDefaultDomain;
  IntegrationTestFramework(1)
      .setInitialState(kAdminKeypair)
      .sendTx(makeUserWithPerms())
      .skipProposal()
      .skipBlock()
      .sendTx(completeTx(baseTx().createDomain(existing_domain, kRole)))
      .skipProposal()
      .checkBlock(
          [](auto &block) { ASSERT_EQ(block->transactions().size(), 0); })
      .done();
}

/**
 * @given some user with can_create_domain permission
 * @when execute tx with CreateDomain command with maximum available length
 * @then there is the tx in proposal
 */
TEST_F(CreateDomain, MaxLenName) {
  std::string maxLongDomain =
      // 255 characters string
      "maxLabelLengthIs63paddingPaddingPaddingPaddingPaddingPaddingPad."
      "maxLabelLengthIs63paddingPaddingPaddingPaddingPaddingPaddingPad."
      "maxLabelLengthIs63paddingPaddingPaddingPaddingPaddingPaddingPad."
      "maxLabelLengthIs63paddingPaddingPaddingPaddingPaddingPaddingPad";
  IntegrationTestFramework(1)
      .setInitialState(kAdminKeypair)
      .sendTx(makeUserWithPerms())
      .skipProposal()
      .skipBlock()
      .sendTx(completeTx(baseTx().createDomain(maxLongDomain, kRole)))
      .skipProposal()
      .checkBlock(
          [](auto &block) { ASSERT_EQ(block->transactions().size(), 1); })
      .done();
}

/**
 * @given some user with can_create_domain permission
 * @when execute tx with CreateDomain command with too long length
 * @then the tx hasn't passed stateless validation
 *       (aka skipProposal throws)
 */
TEST_F(CreateDomain, TooLongName) {
  IntegrationTestFramework(1)
      .setInitialState(kAdminKeypair)
      .sendTx(makeUserWithPerms())
      .skipProposal()
      .skipBlock()
      .sendTx(completeTx(baseTx().createDomain(std::string(257, 'a'), kRole)),
              checkStatelessInvalid);
}

/**
 * @given some user with can_create_domain permission
 * @when execute tx with CreateDomain command with empty domain name
 * @then the tx hasn't passed stateless validation
 *       (aka skipProposal throws)
 */
TEST_F(CreateDomain, EmptyName) {
  const std::string &empty_name = "";
  IntegrationTestFramework(1)
      .setInitialState(kAdminKeypair)
      .sendTx(makeUserWithPerms())
      .skipProposal()
      .skipBlock()
      .sendTx(completeTx(baseTx().createDomain(empty_name, kRole)),
              checkStatelessInvalid);
}

/**
 * @given some user with can_create_domain permission
 * @when execute tx with CreateDomain command with empty role name
 * @then the tx hasn't passed stateless validation
 *       (aka skipProposal throws)
 */
TEST_F(CreateDomain, DISABLED_EmptyRoleName) {
  const std::string &empty_name = "";
  IntegrationTestFramework(1)
      .setInitialState(kAdminKeypair)
      .sendTx(makeUserWithPerms())
      .skipProposal()
      .skipBlock()
      .sendTx(completeTx(baseTx().createDomain(kNewDomain, empty_name)),
              checkStatelessInvalid);
}
