//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================

#ifndef smtk_graph_Extensions_h
#define smtk_graph_Extensions_h

#include <functional>
#include <type_traits>

namespace smtk
{
namespace graph
{

template <class...>
struct conjunction : std::true_type
{
};
template <class B1>
struct conjunction<B1> : B1
{
};
template <class B1, class... Bn>
struct conjunction<B1, Bn...> : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type
{
};

template <typename T, typename... Ts>
using CompatibleTypes =
  typename std::enable_if<conjunction<std::is_convertible<Ts, T>...>::value>::type;

template <typename Iterable>
class is_iterable
{
  template <typename X>
  static std::true_type testIterable(
    decltype(std::distance(std::declval<X>(), std::declval<X>()))*);
  template <typename X>
  static std::false_type testIterable(...);

public:
  using type = decltype(testIterable<Iterable>(nullptr));
};

template <typename Container>
class is_container
{
  template <typename X>
  static std::true_type testContainer(
    decltype(std::distance(std::declval<X>().begin(), std::declval<X>().end()))*);
  template <typename X>
  static std::false_type testContainer(...);

public:
  using type = decltype(testContainer<Container>(nullptr));
};
}
}

#endif