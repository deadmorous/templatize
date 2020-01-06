#pragma once

#include <type_traits>

template<class P, std::enable_if_t<std::is_integral_v<P> || std::is_enum_v<P>, int> = 0>
struct CompileTimeSeq {
    using element_type = P;
    template< P... >
    struct seq {
        using element_type = P;
    };
    template<P ... p1>
    static constexpr seq<p1 ...> cat(seq<p1...>) {
        return {};
    }
    template<P ... p1, P ... p2>
    static constexpr seq<p1 ..., p2 ... > cat(seq<p1...>, seq<p2...>) {
        return {};
    }
    template<class seq1, class seq2, class seq3, class ... rest>
    static constexpr auto cat(seq1 s1, seq2 s2, seq3 s3, rest ... r) {
        return cat(s1, cat(s2, s3, r...));
    }
    template<class ... seq>
    using catseq = decltype (cat(seq() ... ));
};
