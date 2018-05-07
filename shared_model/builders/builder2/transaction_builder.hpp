#ifndef IROHA_TRANSACTION_BUILDER_HPP
#define IROHA_TRANSACTION_BUILDER_HPP

#include "builders/builder2/basic_builder.hpp"

/**
 * Builder which constructs shared_model::interface::Transaction objects.
 * @tparam BackendBuilder - Builder used to construct implementation
 * @tparam BuildPolicy - function used during build call
 * @tparam SetterPolicy - Copy or Move builder state on each setter call
 * @tparam S - number used for field verification
 */
template <typename BackendBuilder,
          typename BuildPolicy,
          SetterPolicy SetterPolicy = SetterPolicy::Copy,
          int S = 0>
class TransactionBuilder
    : public BasicBuilder<
          TransactionBuilder<BackendBuilder, BuildPolicy, SetterPolicy, S>,
          BackendBuilder,
          BuildPolicy,
          SetterPolicy,
          S> {
 public:
  using Base = BasicBuilder<
      TransactionBuilder<BackendBuilder, BuildPolicy, SetterPolicy, S>,
      BackendBuilder,
      BuildPolicy,
      SetterPolicy,
      S>;

  using Base::Base;

  enum RequiredFields { CreatedTime, CreatorAccountId, TOTAL };

  auto creatorAccountId(
      const shared_model::interface::types::AccountIdType &id) {
    return transform<CreatorAccountId>(*this, [this, &id](auto &b) {
      b = this->backend_builder_.creatorAccountId(id);
    });
  }

  auto createdTime(shared_model::interface::types::TimestampType created_time) {
    return transform<CreatedTime>(*this, [this, &created_time](auto &b) {
      b = this->backend_builder_.createdTime(created_time);
    });
  }

  /**
   * Constructs builder with the same parameters, but S will be set to s
   */
  template <int s>
  using NextBuilder = TransactionBuilder<BackendBuilder,
                                         BuildPolicy,
                                         SetterPolicy,
                                         s>;
};
#endif  // IROHA_TRANSACTION_BUILDER_HPP
