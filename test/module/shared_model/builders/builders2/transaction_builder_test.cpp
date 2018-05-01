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

TEST(ProtoBuilderTest, TestTransactionBuilder) {
  TransactionBuilder<ProtoTransactionBuilder,
                     DefaultPolicy<shared_model::proto::Transaction>>
      builder;

  auto transaction =
      builder.creatorAccountId("Peter").createdTime(1337).build();

  EXPECT_EQ(transaction->creatorAccountId(), "Peter");
  EXPECT_EQ(transaction->createdTime(), 1337);
}
