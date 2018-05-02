#include <gtest/gtest.h>

#include "builders/builder2/proto_transaction_builder.hpp"
#include "builders/builder2/transaction_builder.hpp"

TEST(ProtoBuilderTest, TestProtoBuilderExample) {
  ProtoTransactionBuilder builder;
  builder = builder.createdTime(1337);

  auto transaction = builder.creatorAccountId("Peter").build();

  EXPECT_EQ(transaction.creatorAccountId(), "Peter");
  EXPECT_EQ(transaction.createdTime(), 1337);

  auto transaction2 = builder.creatorAccountId("Meg").build();

  EXPECT_EQ(transaction2.creatorAccountId(), "Meg");
  EXPECT_EQ(transaction2.createdTime(), 1337);
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
  std::unique_ptr<shared_model::interface::Transaction> operator()(
      T &&transaction) const {
    throw 1;
    return std::make_unique<T>(transaction.getTransport());
  }
};

TEST(GenericBuilderTest, TestTransactionBuilder) {
  TransactionBuilder<ProtoTransactionBuilder,
                     DefaultPolicy<shared_model::proto::Transaction>>
      builder;

  auto transaction =
      builder.creatorAccountId("Peter").createdTime(1337).build();

  EXPECT_EQ(transaction->creatorAccountId(), "Peter");
  EXPECT_EQ(transaction->createdTime(), 1337);
}

TEST(GenericBuilderTest, TestTransactionExceptionPolicy) {
  TransactionBuilder<ProtoTransactionBuilder,
                     ExceptionPolicy<shared_model::proto::Transaction>>
      builder;

  auto builder2 = builder.creatorAccountId("Peter").createdTime(1337);

  ASSERT_ANY_THROW(builder2.build());
}

TEST(GenericBuilderTest, TestCopyOfState) {
  TransactionBuilder<ProtoTransactionBuilder,
                     DefaultPolicy<shared_model::proto::Transaction>>
      builder;

  auto builder1 = builder.createdTime(1337);
  auto builder2(builder1);

  auto t1 = builder1.creatorAccountId("John").build();
  auto t2 = builder2.creatorAccountId("Stan").build();

  EXPECT_EQ(t1->createdTime(), t2->createdTime());
  EXPECT_EQ(t1->creatorAccountId(), "John");
  EXPECT_EQ(t2->creatorAccountId(), "Stan");
}
