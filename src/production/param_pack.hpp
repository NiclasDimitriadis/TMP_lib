#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

#include "helpers.hpp"
#include "non_rep_combinations.hpp"

namespace param_pack {

// extracts the i-th type from a type-parameter pack
template<size_t i, typename... Ts>
requires (i < sizeof...(Ts))
struct t_pack_index {
private:
   template<size_t at_index, size_t target_index, typename T, typename... Ts_>
   struct pack_index_logic {
      using type = pack_index_logic<at_index + 1, target_index, Ts_...>::type;
   };

   template<size_t target_index, typename T, typename... Ts_>
   struct pack_index_logic<target_index, target_index, T, Ts_...> {
      using type = T;
   };

public:
   using type = pack_index_logic<0, i, Ts...>::type;
};

template<size_t i, typename... Ts>
using pack_index_t = t_pack_index<i, Ts...>::type;

// extracts i-th element from non-type parameter pack
template<size_t i, auto... vs>
requires (i < sizeof...(vs))
struct nt_pack_index {

private:
   template<size_t at_index, size_t target_index, auto v1, auto... vs_>
   struct nt_pack_index_logic {
      static constexpr auto value = nt_pack_index_logic<at_index + 1, target_index, vs_...>::value;
   };

   template<size_t target_index, auto v1, auto... vs_>
   struct nt_pack_index_logic<target_index, target_index, v1, vs_...> {
      static constexpr auto value = v1;
   };

public:
   static constexpr auto value = nt_pack_index_logic<0, i, vs...>::value;
};

template<size_t i, auto... vs>
constexpr inline auto pack_index_v = nt_pack_index<i, vs...>::value;

// determines whether all elements is a non-type parameter pack are of the same
// type
template<auto...>
struct single_type_nt_pack: std::false_type {};

template<typename T, T... is>
struct single_type_nt_pack<is...>: std::true_type {};

template<auto... is>
constexpr inline bool single_type_nt_pack_v = single_type_nt_pack<is...>::value;

// clang compliant version of single_type_nt_pack
template<auto...>
struct single_type_nt_pack_clang: std::false_type {};

template<auto fst, auto scnd, auto... rest>
requires (sizeof...(rest) > 0)
struct single_type_nt_pack_clang<fst, scnd, rest...>{
      static constexpr bool value = std::is_same_v<decltype(fst), decltype(scnd)> && single_type_nt_pack_clang<scnd, rest...>::value;
};

template<auto fst, auto last>
struct single_type_nt_pack_clang<fst, last>{
      static constexpr bool value = std::is_same_v<decltype(fst), decltype(last)>;
};

template<auto only>
struct single_type_nt_pack_clang<only>{
      static constexpr bool value = true;
};

template<auto... is>
constexpr bool single_type_nt_pack_clang_v =  single_type_nt_pack_clang<is...>::value;

// deduces the type of a non-type parameter pack
template<auto... is>
requires (sizeof...(is) > 0) && single_type_nt_pack_v<is...>
struct pack_type {
   using type = decltype(pack_index_v<0, is...>);
};

template<auto... is>
using pack_type_t = typename pack_type<is...>::type;

template<typename T, T... vals>
struct non_type_pack {
private:
   // abridged version of type_pack_t internal to non_type_pack, avoids circular
   // dependencies
   template<typename... Ts>
   struct type_pack_pr {
   private:
      template<typename>
      struct append_logic {
         static_assert(false);
      };

      template<typename... Us>
      struct append_logic<type_pack_pr<Us...>> {
         using type = type_pack_pr<Ts..., Us...>;
      };

      template<template<typename...> class, typename...>
      struct functor_map_logic {
         using type = type_pack_pr<>;
      };

      template<template<typename...> class Class_Template, typename Fst,
         typename... Tail>
      struct functor_map_logic<Class_Template, Fst, Tail...> {
         using type = type_pack_pr<Class_Template<Fst>>::template append_t<
            typename functor_map_logic<Class_Template, Tail...>::type>;
      };

      template<template<typename...> class Class_Template, typename Last>
      struct functor_map_logic<Class_Template, Last> {
         using type = type_pack_pr<Class_Template<Last>>;
      };

      template<template<typename...> class Class_Template>
      struct functor_map {
         using type = functor_map_logic<Class_Template, Ts...>::type;
      };

      template<template<typename...> class F, typename Init, typename...>
      struct fold_logic {
         using type = F<Init>;
      };

      template<template<typename...> class F, typename First, typename Second,
         typename... Tail>
      requires helpers::specializes_class_template_v<F, F<First, Second>>
      struct fold_logic<F, First, Second, Tail...> {
         using type = fold_logic<F, F<First, Second>, Tail...>::type;
      };

      template<template<typename...> class F, typename Second_To_Last,
         typename Last>
      requires helpers::specializes_class_template_v<F, F<Second_To_Last, Last>>
      struct fold_logic<F, Second_To_Last, Last> {
         using type = F<Second_To_Last, Last>;
      };

   public:
      static constexpr size_t size = sizeof...(Ts);

      template<size_t i>
      using index_t = pack_index_t<i, Ts...>;

      template<typename Append_Pack>
      using append_t = append_logic<Append_Pack>::type;

      template<template<typename...> class Class_Template>
      using specialize_template_t = Class_Template<Ts...>;

      template<template<typename...> class Class_Template>
      using functor_map_t = functor_map<Class_Template>::type;

      template<template<typename...> class F, typename Init>
      using fold_t = fold_logic<F, Init, Ts...>::type;
   };

   template<typename, auto...>
   struct append_logic {
      static_assert(false);
   };

   template<T... app_vals>
   struct append_logic<non_type_pack<T, app_vals...>> {
      using type = non_type_pack<T, vals..., app_vals...>;
   };

   template<size_t n, auto...>
   requires (n <= sizeof...(vals))
   struct truncate_front_logic {
      static_assert(false);
   };

   template<size_t n, T fst, T... tail>
   requires (sizeof...(tail) != sizeof...(vals) - n - 1)
   struct truncate_front_logic<n, fst, tail...> {
      using type = truncate_front_logic<n, tail...>::type;
   };

   template<T... trunc_vals>
   struct truncate_front_logic<sizeof...(vals), trunc_vals...> {
      using type = non_type_pack<T>;
   };

   template<T... tail>
   struct truncate_front_logic<sizeof...(vals) - sizeof...(tail), tail...> {
      using type = non_type_pack<T, tail...>;
   };

   template<size_t n, auto...>
   requires (n <= sizeof...(vals))
   struct truncate_back_logic {
      static_assert(false);
   };

   template<size_t n, T fst, T... tail>
   struct truncate_back_logic<n, fst, tail...> {
      using type = non_type_pack<T, fst>::template append_t<
         typename truncate_back_logic<n, tail...>::type>;
   };

   template<T fst, T... tail>
   struct truncate_back_logic<sizeof...(tail), fst, tail...> {
      using type = non_type_pack<T, fst>;
   };

   template<size_t index, auto... vals_>
   struct reverse_logic {
      using type = non_type_pack<T, pack_index_v<index, vals_...>>::template append_t<
         typename reverse_logic<index - 1, vals_...>::type>;
   };

   template<size_t index>
   struct reverse_logic<index> {
      using type = non_type_pack<T>;
   };

   template<auto... vals_>
   struct reverse_logic<0, vals_...> {
      using type = non_type_pack<T, pack_index_v<0, vals_...>>;
   };

   template<auto...>
   struct functor_map_logic {
      static_assert(false);
   };

   template<auto callable, T fst, T... tail>
   struct functor_map_logic<callable, fst, tail...> {
   private:
      using F_Type = decltype(callable(std::declval<T>()));

   public:
      using type = non_type_pack<F_Type, callable(fst)>::template append_t<
         typename functor_map_logic<callable, tail...>::type>;
   };

   template<auto callable>
   struct functor_map_logic<callable> {
      using type = non_type_pack<decltype(callable(std::declval<T>()))>;
   };

   template<auto callable>
   requires std::is_invocable_v<decltype(callable), T> && (!std::is_same_v<decltype(callable(std::declval<T>())), void>)
   struct functor_map {
      using type = functor_map_logic<callable, vals...>::type;
   };

   // checks whether a pack of other specializations of non_type_pack are all of the same size as this specialization
   template<typename Other_NTP, typename... Other_NTPs>
   requires helpers::specializes_class_template_tnt_v<non_type_pack, Other_NTP>
   struct same_size_NTPs {
      static constexpr bool value = (sizeof...(vals) == Other_NTP::size) && same_size_NTPs<Other_NTPs...>::value;
   };

   template<typename Other_NTP>
   requires helpers::specializes_class_template_tnt_v<non_type_pack, Other_NTP>
   struct same_size_NTPs<Other_NTP> {
      static constexpr bool value = (sizeof...(vals) == Other_NTP::size);
   };

   // specializes a class template from the n-th elements of a pack of specializations non_type_pack
   template<template<auto...> class Class_Template, typename NTPs, size_t n, size_t iter, auto... vals_>
   struct apply_logic {
      static constexpr auto val = NTPs::template index_t<iter>::template index_v<n>;
      using type = apply_logic<Class_Template, NTPs, n, iter + 1, vals_..., val>::type;
   };

   template<template<auto...> class Class_Template, typename NTPs, size_t n, auto... vals_>
   struct apply_logic<Class_Template, NTPs, n, NTPs::size, vals_...> {
      using type = Class_Template<vals_...>;
   };

   // generates a specialization of type_pack_pr from the specializations of a class template from the n-th elements of this specialization of non_type_pack as well as a pack of others
   template<template<auto...> class Class_Template, typename NTPs, size_t n, typename... Applied>
   struct apply_to_tp {
      using iter_type = apply_logic<Class_Template, NTPs, n, 0>::type;
      using type = apply_to_tp<Class_Template, NTPs, n + 1, Applied..., iter_type>::type;
   };

   template<template<auto...> class Class_Template, typename NTPs, typename... Applied>
   struct apply_to_tp<Class_Template, NTPs, NTPs::size, Applied...> {
      using type = type_pack_pr<Applied...>;
   };

   // generate a specialization of non_type_pack from the value members of all types contained in a specilazation of type_pack_pr
   template<typename Type_Pack, size_t index, auto... vals_>
   struct extract_values {
      static constexpr auto iter_value = Type_Pack::template index_t<index>::value;
      using type = extract_values<Type_Pack, index + 1, vals_..., iter_value>::type;
   };

   template<typename Type_Pack, auto... vals_>
   struct extract_values<Type_Pack, Type_Pack::size, vals_...> {
      using type = non_type_pack<pack_type_t<vals_...>, vals_...>;
   };

   template<template<auto...> class Class_Templ, size_t n_vals, typename... NTPs>
   requires same_size_NTPs<NTPs...>::value
   struct apply {
      using applied = apply_to_tp<Class_Templ, type_pack_pr<NTPs...>, 0>::type;
      using type = extract_values<applied, 0>::type;
   };

   template<template<auto...> class Class_Templ, typename... NTPs>
   requires same_size_NTPs<NTPs...>::value
   struct apply<Class_Templ, 0, NTPs...> {
      using content_type = decltype(Class_Templ<>::value);
      using type = non_type_pack<content_type, Class_Templ<>::value>;
   };

   template<template<auto...> class, auto...>
   struct monadic_bind_logic {
      using type = non_type_pack<T>;
   };

   template<template<auto...> class Class_Template, T fst, T... tail>
   requires helpers::specializes_class_template_tnt_v<non_type_pack,
      Class_Template<fst>>
   struct monadic_bind_logic<Class_Template, fst, tail...> {
      using type = Class_Template<fst>::template append_t<
         typename monadic_bind_logic<Class_Template, tail...>::type>;
   };

   template<template<auto...> class Class_Template, T last>
   requires helpers::specializes_class_template_tnt_v<non_type_pack,
      Class_Template<last>>
   struct monadic_bind_logic<Class_Template, last> {
      using type = Class_Template<last>;
   };

   template<template<auto...> class Class_Template>
   struct monadic_bind {
      using type = monadic_bind_logic<Class_Template, vals...>::type;
   };

   template<auto fold_by, T first, T second, T... tail>
   requires std::is_invocable_r_v<T, decltype(fold_by), T, T> && (sizeof...(vals) > 0)
   struct fold_logic {
      static constexpr T value = fold_logic<fold_by, fold_by(first, second), tail...>::value;
   };

   template<auto fold_by, T second_to_last, T last>
   struct fold_logic<fold_by, second_to_last, last> {
      static constexpr T value = fold_by(second_to_last, last);
   };

public:
   using type = T;
   static constexpr size_t size = sizeof...(vals);

   // appends another non_type_pack
   template<typename Append_Pack>
   using append_t = append_logic<Append_Pack>::type;

   // removes first n entries
   template<size_t n>
   using truncate_front_t = truncate_front_logic<n, vals...>::type;

   // removes last n entries
   template<size_t n>
   using truncate_back_t = truncate_back_logic<n, vals...>::type;

   // yields first n elements
   template<size_t n>
   using head_t = truncate_back_t<sizeof...(vals) - n>;

   // yields last n elements
   template<size_t n>
   using tail_t = truncate_front_t<sizeof...(vals) - n>;

   // extract i-th entry in parameter pack
   template<size_t i>
   static constexpr T index_v = pack_index_v<i, vals...>;

   // uses vals as non-type arguments to another template
   template<template<auto...> class Class_Template>
   using specialize_template_t = Class_Template<vals...>;

   // uses T as type argument and vals as non-type arguments to another template
   template<template<typename U, U...> class Class_Template>
   using specialize_type_param_template_t = Class_Template<T, vals...>;

   // yields non_type_pack with entries in reverse order
   using reverse_t = reverse_logic<sizeof...(vals) - 1, vals...>::type;

   // applies a callable object to every singe entry in vals in a functorial way
   template<auto callable>
   using functor_map_t = functor_map<callable>::type;

   // apply a class template to multiple non_type_pack-types as an applicative
   // functor
   template<template<auto...> class Class_Template,
      typename... Other_NTPs>
   using apply_t = apply<Class_Template, sizeof...(vals), non_type_pack<type, vals...>, Other_NTPs...>::type;

   // applies a class template to all entries in vals in a monadic way
   template<template<T...> class Class_Template>
   using monadic_bind_t = monadic_bind<Class_Template>::type;

   // folds vals... into a single value
   template<auto fold_by, T init>
   static constexpr T fold_v = fold_logic<fold_by, init, vals...>::value;

   using drop_const_t = non_type_pack<std::remove_const_t<type>, vals...>;
};

template<auto... vals>
using non_type_pack_t = non_type_pack<pack_type_t<vals...>, vals...>;

template<typename>
struct generate_non_type_pack {};

template<template<auto...> class from_template, auto... vals_>
struct generate_non_type_pack<from_template<vals_...>> {
   using type = param_pack::non_type_pack_t<vals_...>;
};

template<template<typename, auto...> class from_template, typename T,
   T... vals_>
struct generate_non_type_pack<from_template<T, vals_...>> {
   using type = param_pack::non_type_pack_t<vals_...>;
};

template<typename T>
using generate_non_type_pack_t = generate_non_type_pack<T>::type;

// metafunction to determine whether a type can be used to generate a
// non_type_pack_t via generate_non_type_pack_t
template<typename, typename = void>
struct non_type_pack_convertible: std::false_type {};

template<typename T>
struct non_type_pack_convertible<T, std::void_t<generate_non_type_pack_t<T>>>
    : std::true_type {};

template<typename T>
constexpr inline bool non_type_pack_convertible_v = non_type_pack_convertible<T>::value;

template<size_t n, auto i, auto... is>
struct repeat_val_n_times {
   using type = repeat_val_n_times<n - 1, i, is..., i>::type;
};

template<auto i, auto... is>
struct repeat_val_n_times<0, i, is...> {
   using type = non_type_pack_t<is...>;
};

template<size_t n, auto i>
using repeat_val_n_times_t = repeat_val_n_times<n, i>::type;

template<size_t n, typename NT_pack>
struct repeat_NT_pack_n_times{
  using type = repeat_NT_pack_n_times<n - 1, typename NT_pack::template append_t<NT_pack>>::type;
};

template<typename NT_pack>
struct repeat_NT_pack_n_times<1, NT_pack>{
   using type = NT_pack;
};

template<typename NT_pack>
struct repeat_NT_pack_n_times<0, NT_pack>{
   using type = non_type_pack<typename NT_pack::type>;
};

template<size_t n, typename NT_pack>
using repeat_NT_pack_n_times_t = repeat_NT_pack_n_times<n, NT_pack>::type;

template<typename... Ts>
struct type_pack_t {
private:
   template<typename>
   struct append_logic {
      static_assert(false);
   };

   template<typename... Us>
   struct append_logic<type_pack_t<Us...>> {
      using type = type_pack_t<Ts..., Us...>;
   };

   template<size_t n, typename... Us>
   requires (n <= sizeof...(Ts))
   struct truncate_front_logic {
      static_assert(false);
   };

   // regular recursion step
   template<size_t n, typename U, typename... Us>
   requires (sizeof...(Us) != sizeof...(Ts) - n - 1)
   struct truncate_front_logic<n, U, Us...> {
      using type = truncate_front_logic<n, Us...>::type;
   };

   // stop recursion when n first entries have been removed
   template<typename... Us>
   struct truncate_front_logic<sizeof...(Ts) - sizeof...(Us), Us...> {
      using type = type_pack_t<Us...>;
   };

   template<size_t n, typename...>
   requires (n <= sizeof...(Ts))
   struct truncate_back_logic {
      static_assert(false);
   };

   template<size_t n, typename U, typename... Us>
   struct truncate_back_logic<n, U, Us...> {
      using type = type_pack_t<U>::template append_t<
         typename truncate_back_logic<n, Us...>::type>;
   };

   template<typename U, typename... Us>
   struct truncate_back_logic<sizeof...(Us), U, Us...> {
      using type = type_pack_t<U>;
   };

   template<size_t index, typename... Us>
   struct reverse_logic {
      using type = type_pack_t<pack_index_t<index, Us...>>::template append_t<
         typename reverse_logic<index - 1, Us...>::type>;
   };

   template<typename... Us>
   struct reverse_logic<0, Us...> {
      using type = type_pack_t<pack_index_t<0, Us...>>;
   };

   template<size_t index>
   struct reverse_logic<index> {
      using type = type_pack_t<>;
   };

   template<template<typename...> class, typename...>
   struct functor_map_logic {
      using type = type_pack_t<>;
   };

   template<template<typename...> class Class_Template, typename Fst,
      typename... Tail>
   struct functor_map_logic<Class_Template, Fst, Tail...> {
      using type = type_pack_t<Class_Template<Fst>>::template append_t<
         typename functor_map_logic<Class_Template, Tail...>::type>;
   };

   template<template<typename...> class Class_Template, typename Last>
   struct functor_map_logic<Class_Template, Last> {
      using type = type_pack_t<Class_Template<Last>>;
   };

   template<template<typename...> class Class_Template>
   struct functor_map {
      using type = functor_map_logic<Class_Template, Ts...>::type;
   };

   template<typename Other_Pack>
   requires helpers::specializes_class_template_v<type_pack_t, Other_Pack>
   struct same_size {
      static constexpr bool value = sizeof...(Ts) == Other_Pack::size;
   };

   template<size_t index>
   struct extract_nth_type {
      template<typename Type_Pack>
      using type = typename Type_Pack::template index_t<index>;
   };

   template<template<typename...> class Class_Template, const size_t index,
      typename Nested_Pack>
   struct pure_step {
      // generate type_pack_t of all types at index 'index' of all the packs (get
      // 'row')
      using intermediate_pack = Nested_Pack::template functor_map_t<
         extract_nth_type<index>::template type>;
      // apply template to intermediate_pack
      using type = intermediate_pack::template specialize_template_t<Class_Template>;
   };

   template<template<typename...> class Class_Template, const size_t index,
      typename Nested_Pack, typename Res_Pack>
   struct pure_logic {
      using iter_res_pack = type_pack_t<
         typename pure_step<Class_Template, index, Nested_Pack>::type>;
      using type = pure_logic<Class_Template, index + 1, Nested_Pack,
         typename Res_Pack::template append_t<iter_res_pack>>::type;
   };

   template<template<typename...> class Class_Template, typename Nested_Pack,
      typename Res_Pack>
   struct pure_logic<Class_Template, sizeof...(Ts), Nested_Pack, Res_Pack> {
      using type = Res_Pack;
   };

   template<template<typename...> class Class_Templ, typename Other_Packs,
      typename...>
   requires Other_Packs::template
   functor_map_t<same_size>::template fold_t<
      helpers::And, std::true_type>::value struct apply {
      using type = type_pack_t<Class_Templ<>>;
   };

   template<template<typename...> class Class_Templ, typename Other_Packs,
      typename T_, typename... Ts_>
   requires Other_Packs::template
   functor_map_t<same_size>::template fold_t<helpers::And, std::true_type>::
      value struct apply<Class_Templ, Other_Packs, T_, Ts_...> {
      using type = pure_logic<Class_Templ, 0,
         typename type_pack_t<
            type_pack_t<T_, Ts_...>>::template append_t<Other_Packs>,
         type_pack_t<>>::type;
   };

   template<template<typename...> class, typename...>
   struct monadic_bind_logic {
      using type = type_pack_t<>;
   };

   template<template<typename...> class Class_Template, typename Fst,
      typename... Tail>
   requires helpers::specializes_class_template_v<type_pack_t,
      Class_Template<Fst>>
   struct monadic_bind_logic<Class_Template, Fst, Tail...> {
      using type = Class_Template<Fst>::template append_t<
         typename monadic_bind_logic<Class_Template, Tail...>::type>;
   };

   template<template<typename...> class Class_Template, typename Last>
   requires helpers::specializes_class_template_v<type_pack_t,
      Class_Template<Last>>
   struct monadic_bind_logic<Class_Template, Last> {
      using type = Class_Template<Last>;
   };

   template<template<typename...> class Class_Template>
   struct monadic_bind {
      using type = monadic_bind_logic<Class_Template, Ts...>::type;
   };

   template<size_t...>
   struct subset {
      using type = type_pack_t<>;
   };

   template<size_t fst, size_t... tail>
   struct subset<fst, tail...> {
      using type = type_pack_t<pack_index_t<fst, Ts...>>::template append_t<
         typename subset<tail...>::type>;
   };

   template<size_t last>
   struct subset<last> {
      using type = type_pack_t<pack_index_t<last, Ts...>>;
   };

   template<size_t... is>
   using subset_t = subset<is...>::type;

   template<typename...>
   struct subset_from_pack {
      static_assert(false);
   };

   template<typename Uint, size_t... is>
   requires std::unsigned_integral<Uint>
   struct subset_from_pack<non_type_pack<Uint, is...>> {
      using type = subset_t<is...>;
   };

   template<typename NT_Pack>
   using subset_from_pack_t = subset_from_pack<NT_Pack>::type;

   template<typename T_, size_t>
   struct split_logic {
      static_assert(false);
   };

   template<size_t len, typename Uint, size_t... is>
   requires (sizeof...(is) > len) && (sizeof...(is) % len == 0) && std::unsigned_integral<Uint>
   struct split_logic<non_type_pack<Uint, is...>, len> {
   private:
      using arg_pack = non_type_pack_t<is...>;
      using subset_pack = arg_pack::template head_t<len>;
      using pass_down_pack = arg_pack::template truncate_front_t<len>;

   public:
      using type = type_pack_t<subset_from_pack_t<subset_pack>>::template append_t<
         typename split_logic<pass_down_pack, len>::type>;
   };

   template<size_t len, typename Uint, size_t... is>
   requires (sizeof...(is) == len) && std::unsigned_integral<Uint>
   struct split_logic<non_type_pack<Uint, is...>, len> {
   private:
      using subset_pack = non_type_pack<Uint, is...>;

   public:
      using type = type_pack_t<subset_from_pack_t<subset_pack>>;
   };

   template<template<typename...> class F, typename Init, typename...>
   struct fold_logic {
      using type = F<Init>;
   };

   template<template<typename...> class F, typename First, typename Second,
      typename... Tail>
   requires helpers::specializes_class_template_v<F, F<First, Second>>
   struct fold_logic<F, First, Second, Tail...> {
      using type = fold_logic<F, F<First, Second>, Tail...>::type;
   };

   template<template<typename...> class F, typename Second_To_Last,
      typename Last>
   requires helpers::specializes_class_template_v<F, F<Second_To_Last, Last>>
   struct fold_logic<F, Second_To_Last, Last> {
      using type = F<Second_To_Last, Last>;
   };

   template<template<typename...> class, size_t>
   struct apply_templ_to_nr_combs {
      using type = type_pack_t<>;
   };

   template<template<typename...> class templ, size_t order>
   requires (order > 0) && (sizeof...(Ts) > 0)
   struct apply_templ_to_nr_combs<templ, order> {
   private:
      template<typename T>
      using feed_template = T::template specialize_template_t<templ>;

      using combinations = generate_non_type_pack_t<
         non_rep_combinations::non_rep_index_combinations_t<sizeof...(Ts),
            order>>;
      using split_up = type_pack_t<Ts...>::template split_logic<combinations, order>::type;

   public:
      using type = split_up::template functor_map_t<feed_template>;
   };

public:
   static constexpr size_t size = sizeof...(Ts);

   // extract i-th entry in parameter pack
   template<size_t i>
   using index_t = pack_index_t<i, Ts...>;

   // append another tpye_pack_t
   template<typename Append_Pack>
   using append_t = append_logic<Append_Pack>::type;

   // removes first n entries
   template<size_t n>
   using truncate_front_t = truncate_front_logic<n, Ts...>::type;

   // removes last n entries
   template<size_t n>
   using truncate_back_t = truncate_back_logic<n, Ts...>::type;

   // yields first n elements
   template<size_t n>
   using head_t = truncate_back_t<sizeof...(Ts) - n>;

   // yields last n elements
   template<size_t n>
   using tail_t = truncate_front_t<sizeof...(Ts) - n>;

   // use Ts as type-arguments to specialize other type template
   template<template<typename...> class Class_Template>
   using specialize_template_t = Class_Template<Ts...>;

   // reverses order of Ts...
   using reverse_t = reverse_logic<sizeof...(Ts) - 1, Ts...>::type;

   // apply a class template to Ts in a functorial way
   template<template<typename...> class Class_Template>
   using functor_map_t = functor_map<Class_Template>::type;

   // apply a class template to multiple type_pack_t-types as an applicative
   // functor
   template<template<typename...> class Class_Template,
      typename... Other_Type_Packs>
   using apply_t = apply<Class_Template, type_pack_t<Other_Type_Packs...>, Ts...>::type;

   // apply a class template to Ts in a monadic way
   template<template<typename...> class Class_Template>
   using monadic_bind_t = monadic_bind<Class_Template>::type;

   // generates a nested type pack of subsets of Ts... of length len from indices
   // specified in a non type pack of type size_t
   template<typename NT_Pack, size_t len>
   using split_t = split_logic<NT_Pack, len>::type;

   // generates a type by folding Ts... via a class template
   template<template<typename...> class F, typename Init>
   using fold_t = fold_logic<F, Init, Ts...>::type;

   // splits types into non-repeating groups of a certain size and uses each to
   // specialize a template
   template<template<typename...> class Templ, size_t order>
   using apply_templ_to_nr_combs_t = apply_templ_to_nr_combs<Templ, order>::type;
};

template<typename>
struct generate_type_pack {};

template<template<typename...> class Class_Templ, typename... Ts>
struct generate_type_pack<Class_Templ<Ts...>> {
   using type = type_pack_t<Ts...>;
};

template<typename T>
using generate_type_pack_t = generate_type_pack<T>::type;

// metafunction to determine whether a type can be used to generate a
// type_pack_t via generate_type_pack_t
template<typename, typename = void>
struct type_pack_convertible: std::false_type {};

template<typename T>
struct type_pack_convertible<T, std::void_t<generate_type_pack_t<T>>>
    : std::true_type {};

template<typename T>
constexpr inline bool type_pack_convertible_v = type_pack_convertible<T>::value;

}; // namespace param_pack
