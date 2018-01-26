/**
 * Copyright Soramitsu Co., Ltd. 2017, 2018 All Rights Reserved.
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

#include "builders/protobuf/queries.hpp"
#include "builders/protobuf/transaction.hpp"
#include "cryptography/keypair.hpp"
#include "datetime/time.hpp"
#include "integration/pipeline/tx_pipeline_integration_test_fixture.hpp"
#include "model/generators/query_generator.hpp"
#include "responses.pb.h"

using namespace iroha::model::generators;
using namespace iroha::model::converters;
using namespace shared_model;

// TODO: refactor services to allow dynamic port binding IR-741
class TxPipelineIntegrationTest : public TxPipelineIntegrationTestFixture {
 public:
  void SetUp() override {
    iroha::ametsuchi::AmetsuchiTest::SetUp();

    // create 1st genesis tx
    auto genesisTx =
        TransactionGenerator().generateGenesisTransaction(0, {"0.0.0.0:10001"});

    // admin key file is generated after generateGenesisTransaction()
    auto adminKey = iroha::KeysManagerImpl(adminId).loadKeys();
    ASSERT_TRUE(adminKey.has_value());
    adminKeypair = std::make_shared<crypto::Keypair>(
        crypto::PublicKey(adminKey->pubkey.to_string()),
        crypto::PrivateKey(adminKey->privkey.to_string()));

    // create 2nd genesis tx
    std::unique_ptr<iroha::model::Transaction> usersGenesisTx(
        generateCreateUsersTransaction(*adminKeypair).makeOldModel());

    // create genesis block
    genesis_block =
        iroha::model::generators::BlockGenerator().generateGenesisBlock(
            0, {genesisTx, *usersGenesisTx});

    manager = std::make_shared<iroha::KeysManagerImpl>("node0");
    auto keypair = manager->loadKeys().value();

    irohad = std::make_shared<TestIrohad>(block_store_path,
                                          redishost_,
                                          redisport_,
                                          pgopt_,
                                          0,
                                          10001,
                                          10,
                                          5000ms,
                                          5000ms,
                                          5000ms,
                                          keypair);

    ASSERT_TRUE(irohad->storage);

    // insert genesis block
    iroha::main::BlockInserter inserter(irohad->storage);

    inserter.applyToLedger({genesis_block});

    // initialize irohad
    irohad->init();

    // start irohad
    irohad->run();
  }

  void TearDown() override {
    iroha::ametsuchi::AmetsuchiTest::TearDown();
    std::remove("node0.pub");
    std::remove("node0.priv");
    std::remove("admin@test.pub");
    std::remove("admin@test.priv");
    std::remove("test@test.pub");
    std::remove("test@test.priv");
    std::remove("sample@test.pub");
    std::remove("sample@test.priv");
    std::remove("example@test.pub");
    std::remove("example@test.priv");
  }

  proto::Transaction generateCreateUsersTransaction(
      const crypto::Keypair &keypair) {
    using CryptoProvider = crypto::CryptoProviderEd25519Sha3;
    for (size_t i = 0; i < 2; ++i) {
      usersKeypair.emplace_back(CryptoProvider::generateKeypair(
          CryptoProvider::generateSeed(usersId[i])));
    }
    return shared_model::proto::TransactionBuilder()
        .txCounter(1)
        .createdTime(iroha::time::now())
        .creatorAccountId(adminId)
        .createAccount(usersName[0], domainName, usersKeypair[0].publicKey())
        .createAccount(usersName[1], domainName, usersKeypair[1].publicKey())
        .build()
        .signAndAddSignature(keypair);
  }

  std::shared_ptr<shared_model::crypto::Keypair> adminKeypair;
  std::vector<shared_model::crypto::Keypair> usersKeypair;

  const std::string adminName = "admin", domainName = "test",
                    assetName = "coin";
  const std::string adminId = adminName + "@" + domainName,
                    assetId = assetName + "#" + domainName;
  const std::vector<std::string> usersName = {"sample", "example"};
  const std::vector<std::string> usersId = {"sample@test", "example@test"};
};

TEST_F(TxPipelineIntegrationTest, TxPipelineTest) {
  // TODO 19/12/17 motxx - Rework integration test using shared model (IR-715
  // comment)
  // generate test command
  auto cmd =
      iroha::model::generators::CommandGenerator().generateAddAssetQuantity(
          "admin@test",
          "coin#test",
          iroha::Amount().createFromString("20.00").value());

  // generate test transaction
  auto tx =
      iroha::model::generators::TransactionGenerator().generateTransaction(
          "admin@test", 1, {cmd});
  iroha::KeysManagerImpl manager("admin@test");
  auto keypair = manager.loadKeys().value();
  iroha::model::ModelCryptoProviderImpl provider(keypair);
  provider.sign(tx);

  sendTxsInOrderAndValidate({tx});
}

/**
 * @given Admin sends some transaction and keep its hash
 * @when GetTransactions query with the hash is sent
 * @then Validate the transaction
 */
TEST_F(TxPipelineIntegrationTest, GetTransactionsTest) {
  // TODO 19/12/17 motxx - Rework integration test using shared model (IR-715
  // comment)
  const auto CREATOR_ACCOUNT_ID = "admin@test";
  // send some transaction
  const auto cmd =
      iroha::model::generators::CommandGenerator().generateAddAssetQuantity(
          CREATOR_ACCOUNT_ID,
          "coin#test",
          iroha::Amount().createFromString("20.00").value());
  auto given_tx =
      iroha::model::generators::TransactionGenerator().generateTransaction(
          CREATOR_ACCOUNT_ID, 1, {cmd});
  iroha::KeysManagerImpl manager(CREATOR_ACCOUNT_ID);
  const auto keypair = manager.loadKeys().value();
  iroha::model::ModelCryptoProviderImpl provider(keypair);
  provider.sign(given_tx);

  sendTxsInOrderAndValidate({given_tx});

  // keep sent tx's hash
  const auto given_tx_hash = iroha::hash(given_tx);

  auto query =
      iroha::model::generators::QueryGenerator().generateGetTransactions(
          iroha::time::now(), CREATOR_ACCOUNT_ID, 1, {given_tx_hash});
  provider.sign(*query);

  const auto pb_query = PbQueryFactory{}.serialize(query);
  ASSERT_TRUE(pb_query.has_value());

  iroha::protocol::QueryResponse response;
  irohad->getQueryService()->FindAsync(pb_query.value(), response);
  ASSERT_EQ(1, response.transactions_response().transactions().size());
  const auto got_pb_tx = response.transactions_response().transactions()[0];
  ASSERT_EQ(given_tx, *PbTransactionFactory{}.deserialize(got_pb_tx));
}

struct GetAccountAndAccountAssetTransactionsTest
    : public TxPipelineIntegrationTest {
  void SetUp() override {
    TxPipelineIntegrationTest::SetUp();
    sendTransactions();
  }

  // c++ class cannot have virtual data member
  auto adminTransaction() {
    static auto tx =
        shared_model::proto::TransactionBuilder()
            .txCounter(txCounter)
            .creatorAccountId(adminId)
            .addAssetQuantity(adminId, assetId, "1000.00")
            .transferAsset(adminId, srcAccountId, assetId, "lent", "100.00")
            .createdTime(createdTime)
            .build()
            .signAndAddSignature(*adminKeypair);
    return tx;
  }

  auto userTransaction() {
    static auto tx =
        shared_model::proto::TransactionBuilder()
            .txCounter(txCounter)
            .creatorAccountId(creatorAccountId)
            .transferAsset(
                srcAccountId, destAccountId, assetId, "message", "12.34")
            .createdTime(createdTime)
            .build()
            .signAndAddSignature(usersKeypair[0]);
    return tx;
  }

  void sendTransactions() {
    const auto oldModelAdminTx = std::shared_ptr<iroha::model::Transaction>(
        adminTransaction().makeOldModel());
    const auto oldModelUserTx = std::shared_ptr<iroha::model::Transaction>(
        userTransaction().makeOldModel());
    sendTxsInOrderAndValidate({*oldModelAdminTx, *oldModelUserTx});
  }

  const std::string creatorAccountId = usersId[0];
  const std::string srcAccountId = creatorAccountId;
  const std::string destAccountId = usersId[1];
  const iroha::ts64_t createdTime = iroha::time::now();
  const uint64_t txCounter = 1;
  const uint64_t queryCounter = 1;
};

/**
 * @given TransferAsset transaction from test@test to test2@test
 * @when GetAccountTransaction query sent
 * @then Validate the transaction response
 */
TEST_F(GetAccountAndAccountAssetTransactionsTest, GetAccountTransactionsTest) {
  const auto query = shared_model::proto::QueryBuilder()
                         .createdTime(createdTime)
                         .creatorAccountId(creatorAccountId)
                         .getAccountTransactions(creatorAccountId)
                         .queryCounter(queryCounter)
                         .build()
                         .signAndAddSignature(usersKeypair[0]);

  iroha::protocol::QueryResponse response;
  irohad->getQueryService()->FindAsync(query.getTransport(), response);
  ASSERT_TRUE(response.has_transactions_response());
  const auto txResponse = response.transactions_response().transactions();
  ASSERT_EQ(1, txResponse.size());
  ASSERT_EQ(userTransaction().getTransport().SerializeAsString(),
            txResponse.Get(0).SerializeAsString());
}

/**
 * @given TransferAsset transaction from test@test to test2@test
 * @when GetAccountTransaction query sent with pager specified
 * @then Validate the transaction response
 * @note Pager is optional in TransactionBuilder()
 * Verifying pager conversion is in field_validator_test
 */
TEST_F(GetAccountAndAccountAssetTransactionsTest,
       GetAccountTransactionsWithPagerTest) {
  const auto query =
      shared_model::proto::QueryBuilder()
          .createdTime(createdTime)
          .creatorAccountId(creatorAccountId)
          .getAccountTransactions(creatorAccountId, userTransaction().hash())
          .queryCounter(queryCounter)
          .build()
          .signAndAddSignature(usersKeypair[0]);

  iroha::protocol::QueryResponse response;
  irohad->getQueryService()->FindAsync(query.getTransport(), response);
  ASSERT_TRUE(response.has_transactions_response());
  const auto txResponse = response.transactions_response().transactions();
  ASSERT_EQ(0, txResponse.size());
}

/**
 * @given TransferAsset transaction from test@test to test2@test
 * @when GetAccountAssetTransaction query sent
 * @then Validate the transaction response
 */
TEST_F(GetAccountAndAccountAssetTransactionsTest,
       GetAccountAssetTransactionsTest) {
  const auto query =
      shared_model::proto::QueryBuilder()
          .createdTime(createdTime)
          .creatorAccountId(creatorAccountId)
          .getAccountAssetTransactions(creatorAccountId, {assetId})
          .queryCounter(queryCounter)
          .build()
          .signAndAddSignature(usersKeypair[0]);

  iroha::protocol::QueryResponse response;
  irohad->getQueryService()->FindAsync(query.getTransport(), response);
  ASSERT_TRUE(response.has_transactions_response());
  const auto txResponse = response.transactions_response().transactions();
  ASSERT_EQ(2, txResponse.size());
  // Note: Get from last transaction
  ASSERT_EQ(adminTransaction().getTransport().SerializeAsString(),
            txResponse.Get(1).SerializeAsString());
  ASSERT_EQ(userTransaction().getTransport().SerializeAsString(),
            txResponse.Get(0).SerializeAsString());
}

/**
 * @given TransferAsset transaction from test@test to test2@test
 * @when GetAccountAssetTransaction query sent with pager specified
 * @then Validate the transaction response
 * @note Pager is optional in TransactionBuilder()
 * Verifying pager conversion is in field_validator_test
 */
TEST_F(GetAccountAndAccountAssetTransactionsTest,
       GetAccountAssetTransactionsWithPagerTest) {
  const auto query =
      shared_model::proto::QueryBuilder()
          .createdTime(createdTime)
          .creatorAccountId(creatorAccountId)
          .getAccountAssetTransactions(
              creatorAccountId, {assetId}, userTransaction().hash())
          .queryCounter(queryCounter)
          .build()
          .signAndAddSignature(usersKeypair[0]);

  iroha::protocol::QueryResponse response;
  irohad->getQueryService()->FindAsync(query.getTransport(), response);
  ASSERT_TRUE(response.has_transactions_response());
  const auto txResponse = response.transactions_response().transactions();
  ASSERT_EQ(1, txResponse.size());
  ASSERT_EQ(adminTransaction().getTransport().SerializeAsString(),
            txResponse.Get(0).SerializeAsString());
}
