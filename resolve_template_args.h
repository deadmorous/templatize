#pragma once

#include "compile_time_seq.h"
#include "tuple_util.h"

namespace resolve_tpl_args {
namespace detail {

template<class P>
class SingleTemplateArgResolver
{
public:
    template<class Caller, class ArgsAsTuple>
    static auto run(typename CompileTimeSeq<P>::template seq<>,
                      const std::tuple<P>&, const Caller&, const ArgsAsTuple&)
    {
        throw std::range_error("Invalid template parameter");
    }
    template<class Caller, class ArgsAsTuple, P p1, P ... rest>
    static auto run (typename CompileTimeSeq<P>::template seq<p1, rest...>,
                      const std::tuple<P>& a2t, const Caller& caller, const ArgsAsTuple& args)
    {
        if (p1 == std::get<0>(a2t))
            return callWithArgsExpanded<p1>(ints_up_to<std::tuple_size_v<ArgsAsTuple>>(), caller, args);
        else
            return run(typename CompileTimeSeq<P>::template seq<rest...>(), a2t, caller, args);
    }
private:
    template<P p, class Caller, class ArgsAsTuple, int ... S>
    static auto callWithArgsExpanded(int_seq<S...>, const Caller& caller, const ArgsAsTuple& args) {
        return caller.template operator()<static_cast<int>(p)>(std::get<S>(args)...);
    }
};


template<class R, class OriginalCaller, int UnresolvedCount, int ... resolved>
struct Caller
{
    explicit Caller(const R& resolver, const OriginalCaller& originalCaller) :
        m_resolver(resolver),
        m_originalCaller(originalCaller) {}
    const R& m_resolver;
    const OriginalCaller& m_originalCaller;
    template<int p, class ... Args>
    auto operator()(Args ... args) const {
        auto nextResolver = m_resolver.next();
        auto nextCaller = Caller<decltype(nextResolver), OriginalCaller, UnresolvedCount-1, resolved..., p>(nextResolver, m_originalCaller);
        return nextResolver(nextCaller, args...);
    }
};

template<class R, class OriginalCaller, int ... resolved>
struct Caller<R, OriginalCaller, 0, resolved ...>
{
    explicit Caller(const R& resolver, const OriginalCaller& originalCaller) :
        m_resolver(resolver),
        m_originalCaller(originalCaller) {}
    const R& m_resolver;
    const OriginalCaller& m_originalCaller;
    template<class ... Args>
    auto operator()(Args ... args) const {
        return m_originalCaller.template operator()<resolved...>(args...);
    }
};

template<class ... TemplateArgs> struct Resolver;

template<>
struct Resolver<>
{
    explicit Resolver(const std::tuple<>&) {}
    template<class C, class ... Args>
    auto operator()(const C& caller, const Args ... args) {
        return caller(args...);
    }
};

template<class TemplateArg1, class ... TemplateArgsRest>
struct Resolver<TemplateArg1, TemplateArgsRest ...>
{
    using P1 = typename TemplateArg1::element_type;
    using A2T = std::tuple<P1, typename TemplateArgsRest::element_type...>;
    explicit Resolver(const A2T& a2t) :
        m_a2t(a2t) {}
    A2T m_a2t;
    template<class C, class ... Args>
    auto operator()(const C& caller, const Args ... args) {
        return SingleTemplateArgResolver<P1>::run(TemplateArg1(), std::get<0>(m_a2t), caller, std::make_tuple(args ...));
    }
    auto next() const {
        return Resolver<TemplateArgsRest...>{ trunc_tuple(m_a2t) };
    }
};

} // detail
} // resolve_tpl_args

template<class ... TemplateArgs> struct resolve_template_args
{
    template<class OriginalCaller, class ... Args>
    resolve_template_args(
            const std::tuple<typename TemplateArgs::element_type...>& a2t,
            const OriginalCaller& originalCaller,
            Args ... args)
    {
        using namespace resolve_tpl_args::detail;
        using R = Resolver<TemplateArgs...>;
        R resolver(a2t);
        Caller<R, OriginalCaller, sizeof... (TemplateArgs)> caller(resolver, originalCaller);
        resolver(caller, args...);
    }
};

template<class P> using template_arg = CompileTimeSeq<P>;
