#ifndef IROHA_BASIC_BUILDER_HPP
#define IROHA_BASIC_BUILDER_HPP

enum class SetterPolicy { Copy, Move };

template <typename Builder, int Fields>
using NextBuilderType =
    typename std::decay_t<Builder>::template NextBuilder<Fields>;

template <typename BuilderImpl,
          typename BackendBuilder,
          typename BuildPolicy,
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
};

// copy transform
template <int Fields, typename Builder, typename Transformation>
auto transform(Builder &&b, Transformation &&t) {
  //    -> std::enable_if_t<Builder::kSPolicy == SetterPolicy::Copy,
  //                        NextBuilderType<Builder, Fields>> {
  //  typename Builder::template NextBuilder<Fields> copy = b;
  NextBuilderType<Builder, Fields> copy(std::move(b));
  static_assert(not std::is_same<Builder, decltype(copy)>::value,
                "types are the same!");
  t(copy.backend_builder_);
  return copy;
}

//// move transform
// template <int Fields, typename Builder, typename Transformation>
// auto transform(const Builder &b, Transformation &&t)
//    -> std::enable_if_t<Builder::kSPolicy == SetterPolicy::Move,
//                        NextBuilderType<Builder, Fields>> {
//  NextBuilderType<Builder, Fields> copy = std::move(b);
//  t(copy.backend_builder_);
//  return copy;
//}

#endif  // IROHA_BASIC_BUILDER_HPP
