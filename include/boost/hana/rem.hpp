/*!
@file
Defines `boost::hana::rem`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_REM_HPP
#define BOOST_HANA_REM_HPP

#include <boost/hana/fwd/rem.hpp>

#include <boost/hana/concept/constant.hpp>
#include <boost/hana/concept/integral_domain.hpp>
#include <boost/hana/core/convert.hpp>
#include <boost/hana/core/dispatch.hpp>
#include <boost/hana/detail/canonical_constant.hpp>
#include <boost/hana/detail/has_common_embedding.hpp>
#include <boost/hana/value.hpp>

#include <type_traits>


namespace boost { namespace hana {
    //! @cond
    template <typename X, typename Y>
    constexpr decltype(auto) rem_t::operator()(X&& x, Y&& y) const {
        using T = typename hana::tag_of<X>::type;
        using U = typename hana::tag_of<Y>::type;
        using Rem = BOOST_HANA_DISPATCH_IF(decltype(rem_impl<T, U>{}),
            _models<IntegralDomain, T>::value &&
            _models<IntegralDomain, U>::value &&
            !is_default<rem_impl<T, U>>::value
        );

    #ifndef BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS
        static_assert(_models<IntegralDomain, T>::value,
        "hana::rem(x, y) requires 'x' to be an IntegralDomain");

        static_assert(_models<IntegralDomain, U>::value,
        "hana::rem(x, y) requires 'y' to be an IntegralDomain");

        static_assert(!is_default<rem_impl<T, U>>::value,
        "hana::rem(x, y) requires 'x' and 'y' to be embeddable "
        "in a common IntegralDomain");
    #endif

        return Rem::apply(static_cast<X&&>(x), static_cast<Y&&>(y));
    }
    //! @endcond

    template <typename T, typename U, bool condition>
    struct rem_impl<T, U, when<condition>> : default_ {
        template <typename ...Args>
        static constexpr auto apply(Args&& ...) = delete;
    };

    // Cross-type overload
    template <typename T, typename U>
    struct rem_impl<T, U, when<
        detail::has_nontrivial_common_embedding<IntegralDomain, T, U>::value
    >> {
        using C = typename common<T, U>::type;
        template <typename X, typename Y>
        static constexpr decltype(auto) apply(X&& x, Y&& y) {
            return hana::rem(hana::to<C>(static_cast<X&&>(x)),
                             hana::to<C>(static_cast<Y&&>(y)));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Model for integral data types
    //////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct rem_impl<T, T, when<std::is_integral<T>::value &&
                               !std::is_same<T, bool>::value>> {
        template <typename X, typename Y>
        static constexpr decltype(auto) apply(X&& x, Y&& y)
        { return static_cast<X&&>(x) % static_cast<Y&&>(y); }
    };

    //////////////////////////////////////////////////////////////////////////
    // Model for Constants over an IntegralDomain
    //////////////////////////////////////////////////////////////////////////
    template <typename C>
    struct rem_impl<C, C, when<
        _models<Constant, C>::value &&
        _models<IntegralDomain, typename C::value_type>::value
    >> {
        using T = typename C::value_type;
        //! @cond
        template <typename X, typename Y>
        struct constant_t {
            static constexpr decltype(auto) get() {
                return hana::rem(hana::value<X>(), hana::value<Y>());
            }

            using hana_tag = detail::CanonicalConstant<T>;
        };
        //! @endcond
        template <typename X, typename Y>
        static constexpr decltype(auto) apply(X const&, Y const&)
        { return hana::to<C>(constant_t<X, Y>{}); }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_REM_HPP