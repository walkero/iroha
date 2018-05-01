#ifndef IROHA_TRANSACTION_BUILDER_HPP
#define IROHA_TRANSACTION_BUILDER_HPP

template <typename BackendBuilder, typename BuildPolicy, int S = 0>
class TransactionBuilder {
 public:
  auto build() {
    return build_func_(backend_builder_.build());
  }

  TransactionBuilder() = default;

  //  template <typename BF>
  //  TransactionBuilder(BF &&build_func): build_func_(build_func) {}

  TransactionBuilder(const TransactionBuilder &rhs) = default;

  auto creatorAccountId(
      const shared_model::interface::types::AccountIdType &id) {
    return transform<CreatorAccountId>([this, &id](auto &b) {
      b = backend_builder_.creatorAccountId(id);
    });
  }

  auto createdTime(shared_model::interface::types::TimestampType created_time) {
    return transform<CreatorAccountId>([this, &created_time](auto &b) {
      b = backend_builder_.createdTime(created_time);
    });
  }

 private:
  template <int Sp>
  TransactionBuilder(
      const TransactionBuilder<BackendBuilder, BuildPolicy, Sp> &o)
      : backend_builder_(o.backend_builder_),
        build_func_(o.build_func_) {}

  template <typename, typename, int>
  friend class TransactionBuilder;

  enum RequiredFields { Command, CreatorAccountId, TOTAL };

  template <int s>
  using NextBuilder =
      TransactionBuilder<BackendBuilder, BuildPolicy, S | (1 << s)>;

  template <int Fields, typename Transformation>
  auto transform(Transformation t) const {
    NextBuilder<Fields> copy = *this;
    t(copy.backend_builder_);
    return copy;
  }

  BackendBuilder backend_builder_;
  BuildPolicy build_func_;
};
#endif  // IROHA_TRANSACTION_BUILDER_HPP
