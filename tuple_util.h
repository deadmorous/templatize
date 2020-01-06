#pragma once
#include <tuple>

namespace tuple_util {
namespace detail {

// https://stackoverflow.com/questions/36612596/tuple-to-parameter-pack
template<int ...> struct seq {};
template<int N, int ...S> struct gens : gens<N - 1, N - 1, S...> { };
template<int ...S> struct gens<0, S...>{ using type = seq<S...>; };

template <int ... S, class T>
inline auto trunc_tuple(seq<S...>, const T& t) {
    return std::make_tuple(std::get<1+S>(t)...);
}

} // detail
} // tuple_util

template<int ... S> using int_seq = tuple_util::detail::seq<S...>;
template<int N> using ints_up_to = typename tuple_util::detail::gens<N>::type;

template <class T1, class ... Trest>
inline std::tuple<Trest ...> trunc_tuple(const std::tuple<T1, Trest ...>& t) {
    namespace d = tuple_util::detail;
    return d::trunc_tuple(ints_up_to<sizeof ... (Trest)>(), t);
}
