#ifndef IROHA_BASIC_BUILDER_HPP
#define IROHA_BASIC_BUILDER_HPP

enum class SetterPolicy { Copy, Move };

template <typename BuilderImpl,
          typename BackendBuilder,
          typename BuildPolicy,
          SetterPolicy SetterPolicy,
          int S = 0>
class BasicBuilder {
 public:
  auto build() {
    static_assert(S == (1 << BuilderImpl::TOTAL) - 1,
                  "Required fields are not set");
    return build_func_(backend_builder_.build());
  }

  BasicBuilder() = default;

  template <typename Builder>
  BasicBuilder(Builder &&o)
      : backend_builder_(std::move(o.backend_builder_)),
        build_func_(o.build_func_) {}

  BackendBuilder backend_builder_;
  BuildPolicy build_func_;

  static_assert(BackendBuilder::kSetterPolicy == SetterPolicy,
                "backend builder has different build policy");

  static constexpr enum SetterPolicy kSetterPolicy = SetterPolicy;
};

template <typename Builder, int Fields>
using NextBuilderType =
    typename std::decay_t<Builder>::template NextBuilder<Fields>;

template <typename Builder>
constexpr SetterPolicy getSetterPolicy() {
  return std::decay_t<Builder>::kSetterPolicy;
}

template <typename Builder>
auto makeNextBuilder(Builder &&b)
    -> std::enable_if_t<getSetterPolicy<Builder>() == SetterPolicy::Copy,
                        Builder> {
  return Builder(b);
}

template <typename Builder>
auto makeNextBuilder(Builder &&b)
    -> std::enable_if_t<getSetterPolicy<Builder>() == SetterPolicy::Move,
                        Builder> {
  return Builder(std::move(b));
}

template <typename Builder, int Fields>
auto makeNextBuilder(Builder &&b)
    -> std::enable_if_t<getSetterPolicy<Builder>() == SetterPolicy::Copy,
                        NextBuilderType<Builder, Fields>> {
  return NextBuilderType<Builder, Fields>(b);
}

template <typename Builder, int Fields>
auto makeNextBuilder(Builder &&b)
    -> std::enable_if_t<getSetterPolicy<Builder>() == SetterPolicy::Move,
                        NextBuilderType<Builder, Fields>> {
  return NextBuilderType<Builder, Fields>(std::move(b));
}


template <int Fields, typename Builder, typename Transformation>
auto transform(Builder &&b, Transformation &&t) {
  NextBuilderType<Builder, Fields> copy =
      makeNextBuilder<Builder, Fields>(std::forward<Builder>(b));
  t(copy.backend_builder_);
  return copy;
}

#endif  // IROHA_BASIC_BUILDER_HPP
