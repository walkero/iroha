#include <gtest/gtest.h>

#include "builders/builder2/proto_transaction_builder.hpp"
#include "builders/builder2/transaction_builder.hpp"

TEST(ProtoBuilderTest, TestProtoBuilderExample) {
  ProtoTransactionBuilder<SetterPolicy::Move> builder;
  builder = builder.createdTime(1337);

  auto transaction = builder.creatorAccountId("Peter").build();

  EXPECT_EQ(transaction.creatorAccountId(), "Peter");
  EXPECT_EQ(transaction.createdTime(), 1337);

  auto transaction2 = builder.creatorAccountId("Meg").build();

  EXPECT_EQ(transaction2.creatorAccountId(), "Meg");
  EXPECT_EQ(transaction2.createdTime(), 1337);
}

TEST(ProtoBuilderTest, FromProtocolObject) {
  ProtoTransactionBuilder<SetterPolicy::Move> builder;

  iroha::protocol::Transaction proto_transaction;
  proto_transaction.mutable_payload()->set_creator_account_id("Johny");
  proto_transaction.mutable_payload()->set_created_time(1338);
  auto transaction = builder.fromImplementation(proto_transaction).build();

  EXPECT_EQ(transaction.creatorAccountId(), "Johny");
  EXPECT_EQ(transaction.createdTime(), 1338);
}

template <typename T>
class DefaultPolicy {
 public:
  std::unique_ptr<shared_model::interface::Transaction> operator()(
      T &&transaction) const {
    return std::make_unique<T>(transaction.getTransport());
  }
};

template <typename T>
class ExceptionPolicy {
 public:
  std::shared_ptr<shared_model::interface::Transaction> operator()(
      T &&transaction) const {
    throw 1;
    return std::make_shared<T>(transaction.getTransport());
  }
};

TEST(GenericBuilderTest, TestTransactionBuilder) {
  TransactionBuilder<ProtoTransactionBuilder<>,
                     DefaultPolicy<shared_model::proto::Transaction>>
      builder;

  auto transaction =
      builder.creatorAccountId("Peter").createdTime(1337).build();

  EXPECT_EQ(transaction->creatorAccountId(), "Peter");
  EXPECT_EQ(transaction->createdTime(), 1337);
}

TEST(GenericBuilderTest, TestTransactionExceptionPolicy) {
  TransactionBuilder<ProtoTransactionBuilder<>,
                     ExceptionPolicy<shared_model::proto::Transaction>>
      builder;

  auto builder2 = builder.creatorAccountId("Peter").createdTime(1337);

  ASSERT_ANY_THROW(builder2.build());
}

TEST(GenericBuilderTest, TestCopyOfState) {
  TransactionBuilder<ProtoTransactionBuilder<SetterPolicy::Move>,
                     DefaultPolicy<shared_model::proto::Transaction>,
                     SetterPolicy::Move>
      builder;

  auto builder1 = builder.createdTime(1337);
  auto builder2(builder1);

  auto builder3 = builder1.creatorAccountId("John");
  auto builder4 = builder2.creatorAccountId("Stan");

  auto t1 = builder3.build();
  auto t2 = builder4.build();

  EXPECT_EQ(t1->createdTime(), t2->createdTime());
  EXPECT_EQ(t1->createdTime(), 1337);
  EXPECT_EQ(t1->creatorAccountId(), "John");
  EXPECT_EQ(t2->creatorAccountId(), "Stan");
}

TEST(GenericBuilderTest, FromImplementation) {
  TransactionBuilder<ProtoTransactionBuilder<>,
                     DefaultPolicy<shared_model::proto::Transaction>>
      builder;

  iroha::protocol::Transaction proto_transaction;
  proto_transaction.mutable_payload()->set_creator_account_id("Johny");
  proto_transaction.mutable_payload()->set_created_time(1338);
  auto transaction = builder.fromImplementation(proto_transaction).build();

  EXPECT_EQ(transaction->creatorAccountId(), "Johny");
  EXPECT_EQ(transaction->createdTime(), 1338);
}
