# TMP_lib
Template metaprogramming library originally spun off from CppOrderBook project.
#### components overview:
- non_repeating_combinations: provides functionality that generates all non-repeating combinations of a specified size for all integers from 1 through n. All combinations are encapsulated consecutively in a single std::integer_sequence which can than be split and consumed.
- param_pack: provides class templates that can wrap type and non-type parameter packs which can then be accessed and manipulated much like an array-like data structure. Provide interfaces for monoidal, functorial, applicative-functorial and monadic access
- function_signature: can deduce the return type and the argument type from an instance of a function-like type
- monoidal_class_template: provides static checks to enforce monoidal behavior in a class template.
- type_pack_check: provides functionality to enforce certain properties across multiple types at compile time using either a single monoidal class template or two different templates.
- helpers: short collection that provides functionality to other components

---

### helpers
- found in namespace `helpers`in file `helpers.hpp`
##### constexpr_bool_value:
`template <typename T>`<br>
`concept constexpr_bool_value`<br>
- C++20 concept to determine whether a type has as static constexpr member of type bool named value

##### specializes_Class_Template_v:
`template<template <typename...> class Class_Template, typename type>`<br>
`constexpr inline bool specializes_Class_Template_v`<br>
- variable template to determine wheter a specific class template accepting only type parameters is specialized by a specific type

##### specializes_Class_Template_nt_v:
`template<template <auto...> class Class_Template, typename type>`<br>
`constexpr inline bool specializes_Class_Template_nt_v`<br>
- variable template to determine wheter a specific class template accepting only non-type parameters is specialized by a specific type

##### specializes_Class_Template_tnt_v:
`template<template <typename T, T...> class Class_Template, typename type>`<br>
`constexpr inline bool specializes_Class_Template_tnt_v`<br>
- variable template to determine wheter a specific class template accepting a single type parameter `T` and a pack of non-type parameters of type `T` is specialized by a specific type
  
---

### non_rep_combinations
- found in namespace `non_rep_combinations` in file `non_rep_combinations.hpp`

##### non_rep_combinations_t:
`template <size_t n, size_t r>`<br>
`using non_rep_combinations_t`
- computes all non-repeating combinations of length `r` integers 1 through `n`
- generates a specialization of `std::integer_sequence` with type `size_t` containing all combinations in one conseqcutive sequence which can then be broken down into the individual combinations

##### non_rep_index_combinations_t:
`template <size_t n, size_t r>`<br>
`using non_rep_combinations_t`
- computes all non-repeating combinations of length `r` integers 0 through `n` - 1
- generates a specialization of `std::integer_sequence` with type `size_t` containing all combinations in one conseqcutive sequence which can then be broken down into the individual combinations
- can be used to compute combinations of indices for data structures with `n` entries, e.g. during compile time checks
  
---

### param_pack
- found in namespace `param_pack`in file `param_pack.hpp`

##### pack_index_t:
`template <size_t i, typename... Ts>`<br>
`using pack_index_t`<br>
- type alias template to extract the i-th type in type parameter pack `Ts`

##### pack_index_v:
`template <size_t i, auto... vs>`<br>
`constexpr inline auto pack_index_v`<br>
- variable template to extract the i-th in non-type parameter pack `vs`

##### single_type_nt_pack_v:
`template<auto... is>`<br>
`constexpr inline bool single_type_nt_pack_v`<br>
- variable template to determine whether all values in non-type parameter pack `is` are of the same type

##### pack_type_t:
`template <auto... is>`<br>
`using pack_type_t`<br>
- type alias template to extract the type of of a non-type paramter pack `is`
- requires that all values of `is` are of the same type (i.e. `single_type_nt_pack_v<is...> == true`)

##### non_type_pack_t:
`template<auto... vals>`<br>
`using non_type_pack_t`<br>
- alias template that generates a `non_type_pack` from values `vals`

##### generate_non_type_pack_t:
`template<typename T>`<br>
`using generate_non_type_pack_t`<br>
- generates a `non_type_pack` from a different type `T`
- `T` needs to implement a template with either just a non-type argument pack or a single type as first argument denoting the the type of the non-type argument pack that follows

##### non_type_pack_convertible_v:
`template<typename T>`<br>
`constexpr inline bool non_type_pack_convertible_v`<br>
- variable template to determine whether type `T` can be used to generate a `non_type_pack` type via the `generate_non_type_pack_t` template

##### generate_type_pack_t:
`template<typename T>`<br>
`using generate_type_pack_t`<br>
- generates a `type_pack_t` from a different type `T`
- `T` needs to implement a template with either just type arguments

##### type_pack_convertible_v:
`template<typename T>`<br>
`constexpr inline bool type_pack_convertible_v`<br>
- variable template to determine whether type `T` can be used to generate a `type_pack_t` type via the `generate_type_pack_t` template

#### non_type_pack:
`template<typename T, T... vals>`<br>
`struct non_type_pack`<br>
- class template that encapsulates non-type paramter pack consisting of values of type `T`

#### public members of non_type_pack:
##### type:
- type alias for `T` (i.e. type of `vals`)

##### size:
`static constexpr size_t size` <br>
- number of values in `vals` (i.e. `sizeof...(vals)`)

##### append_t:
`template<typename append_pack>`<br>
`using append_t`<br>
- type alias that appends different `non_type_pack` of values of type `T` to this one
- associative operation with 'empty' non-type-pack of same type `non_type_pack<T>` as neutral element
- `non_type_pack` is thus a monoid on a type level with `append_t` as its monoid operation

##### truncate_front_t:
`template<size_t n>`<br>
`using truncate_front_t`<br>
- type alias that removes the first `n` values from this `non_type_pack`

##### truncate_back_t:
`template<size_t n>`<br>
`using truncate_front_t`<br>
- type alias that removes the last `n` values from this `non_type_pack`

##### head_t:
`template<size_t n>`<br>
`using head_t`<br>
- type alias that yields a new `non_type_pack` containing the first `n` values of this one

##### tail_t:
`template<size_t n>`<br>
`using tail_t`<br>
- type alias that yields a new `non_type_pack` containing the last `n` values of this one

##### index_v:
`template<size_t i>`<br>
`static constexpr T index_v`<br>
- variable template that extracts the i-th value from this `non_type_pack`

##### specialize_template_t:
`template<template <auto...> class Class_Template>`<br>
`using specialize_template_t`<br>
- class template that uses `vals` as non-type template arguments to `Class_Template` to generate a type

##### specialize_type_param_template_t:
`template<template <typename U, U...> class Class_Template>`<br>
`using specialize_type_param_template_t`
- class template that uses `type` as type argument and `vals` as non-type template arguments to `Class_Template` to generate a type

##### reverse_t:
- member type, `non_type_pack` containing `vals` in reverse order

##### functor_map_t:
`template<auto callable>`<br>
`using functor_map_t`<br>
- alias template taking a non-type argument
- argument needs to be constexpr callable, taking an argument of type `type` as argument
- `functor_map_t` applies `callable` to all values contained in `vals` and yields a new `non_type_pack` containing the results
- `non_type_pack` is thus a functor on a type level 

##### applicative_pure_t:
`template<template<type...> class Class_Template, typename... Other_NTP_Packs>`<br>
`using applicative_pure_t`<br>
- alias template taking a class template (which in turn takes non-type paramenter of type `type`) and an arbitrary number of other `non_type_pack`-types of same type and size as the one being used
- `Class_Template` is then specialized with the n-th values in this `non_type_pack` and all those contained `Other_NTP_Packs` for n = 0 to `size - 1`
- all the specializations then need to contain a `constexpr` member variable `value` of the same type
- the resulting type is a new `non_type_pack` of type `decltype(value)` and size `size`, containing all the `value`s
- `non_type_pack` is thus an applicative functor on a type level

##### monadic_bind_t:
`template<template<T...> class Class_Template>`<br>
`using monadic_bind_t`<br>
- takes a class template as an argument which takes a values of type `type` and generates a `non_type_pack` type
- the generated types are then all concatenated to one `non_type_pack` type
- `non_type_pack` is thus a monad on a type level 
  
##### fold_v:
`template<auto fold_by, T init>`<br>
`static constexpr T fold_v`<br>
- takes a value `fold_by` as non-type arguments which in turn is callable with two values of type `type`
- takes an inital value `init` and recursively "folds" all values contained in `vals` into a single value

#### type_pack_t:
`template<typename... Ts>`<br>
`struct type_pack_t`<br>
- class template that encapsulates type paramter pack

#### public members of type_pack_t:

##### size:
`static constexpr size_t size` <br>
- number of values in `Ts` (i.e. `sizeof...(Ts)`)

##### index_v:
`template<size_t i>`<br>
`using index_t`<br>
- alias template that extracts the i-th type from this `type_pack_t`

##### append_t:
`template<typename append_pack>`<br>
`using append_t`<br>
- type alias that appends different `type_pack_t` to this one
- associative operation with 'empty' type-pack `type_pack_t<>` as neutral element
- `type_pack_t` is thus a monoid on a type level with `append_t` as its monoid operation

##### truncate_front_t:
`template<size_t n>`<br>
`using truncate_front_t`<br>
- type alias that removes the first `n` values from this `type_pack_t`

##### head_t:
`template<size_t n>`<br>
`using head_t`<br>
- type alias that yields a new `type_pack_t` containing the first `n` values of this one

##### tail_t:
`template<size_t n>`<br>
`using tail_t`<br>
- type alias that yields a new `type_pack_t` containing the last `n` values of this one

##### specialize_template_t:
`template<template <auto...> class Class_Template>`<br>
`using specialize_template_t`<br>
- class template that uses `Ts` as type template arguments to `Class_Template` to generate a type

##### reverse_t:
- member type, `type_pack_t` containing `Ts` in reverse order

##### functor_map_t:
`template<template <typename...> class Class_Template><br>
`using functor_map_t`<br>
- alias template taking a class template with a type parameter (or possibly more)
- `functor_map_t` applies `Class_Templ` to all types contained in `Ts` and yields a new `type_pack_t` containing the results
- `type_pack_t` is thus a functor on a type level 

##### applicative_pure_t:
`template<template<typename...> class Class_Template, typename... Other_Type_Packs>`<br>
`using applicative_pure_t`<br>
- alias template taking a class template (type paramenters) and an arbitrary number of other `type_pack_t`-types of same size as the one being used
- `Class_Template` is then specialized with the n-th type in this `type_pack_t` and all those contained in `Other_Type_Packs` for n = 0 to `size - 1`
- the resulting type is a new `type_pack_t` of size `size`, containing all the specializations
- `type_pack_t` is thus an applicative functor on a type level

##### monadic_bind_t:
`template<template <typename...> class Class_Template>`<br>
`using monadic_bind_t`<br>
- takes a class template as an argument which has a type parameter generates a `type_pack_t` type
- the generated types are then all concatenated to one `type_pack_t` type
- `type_pack_t` is thus a monad on a type level 

##### split_t:
`template<typename NT_Pack, size_t len>`<br>
`using split_t`<br>
- alias template with `NT_Pack` as a type paremeter, which is a `non_type_pack` with values of type size_t, and `size_t` `len` as a non-type parameter
- generates nested `type_pack_t` whose elements are `type_pack_t`s of size `len` containing types from `Ts` at indices specified in `nt_pack` (and split into groups of size `len`)   

##### fold_t:
`template<template<typename...> class F, typename Init>`<br>
`using fold_t`<br>
- takes a class template `F` with two type parameters
- takes an inital value `Init` and recursively "folds" all types contained in `Ts` into a single type

##### apply_templ_to_nr_combs_t:
`template<template <typename...> class Templ, size_t order>`<br>
`using apply_templ_to_nr_combs_t`
- computes all non-repeating combinations of size `order` from `Ts`, uses each combiation to specialize `Templ` and generates a `type_pack_t`-type containing all specializations
---

### function_signature:
- found in namespace `function_signature` in file function_signature.hpp
- deduces the return type and argument types of a function-like from an instance of that type
- piggybacks on `std::function`, which can be specilized/constructed from any number of callable types
- some credit goes to this [article](https://ventspace.wordpress.com/2022/04/11/quick-snippet-c-type-trait-templates-for-lambda-details/)

#### relevant intefaces:

##### ret_type_t:
`template<auto f>`<br>
`requires helpers::specializes_class_template_v<std::function, decltype(std::function(f))>`<br>
`using ret_type_t`<br>
- deduces the return type of `f`, provided `std::function` can be constructed from `f`

##### arg_types_t:
`template<auto f>`<br>
`requires helpers::specializes_class_template_v<std::function, decltype(std::function(f))>`<br>
`using arg_types_t`<br>
- generates a specialization of `param_pack::type_pack_t` containing all the argument types of `f`, provided `std::function` can be constructed from `f`

---

### monoidal_class_template:
- found in namespace `monoidal_class_template` in file monoidal_class_template.hpp
#### abstract outline:
- offers a mechanism to enforce a class template behave as monoid w.r.t. to its specializations by a set of different types, using specialization as the monoid operation
#### in more detail:
- consider three types `T1`, `T2` and `T3` as well as a class template `monoid_template`of the form <br>
`template <typename...>`<br>
`struct monoid_template{/*...*/}`<br>
- now let's consider the specializations of `monoid_template` by `T1`, `T2` and `T3` (i.e. `monoid_template<T1>`, `monoid_template<T2>` and `monoid_template<T3>`) and name them `MT1`, `MT2` and `MT3` for brevity
- `monoid_template` will be considered monoidal w.r.t. `T1`, `T2` and `T3` if the following three conditions are fullfilled:
    - closure: specializing `monoid_template` with any combination of two out of `MT1`, `MT2` and `MT3` (e.g. `monoid_template<MT3, MT1>` or `monoid_template<MT2, MT3>`) generates a valid type
    - identity: choosing omission of a second argument as identity element, specializing `monoid_template` with just one out of `MT1`, `MT2` or `MT3` has to yield a type that's equivalent to the one used as type argument
    - associativity: the specializations `monoid_template<MT1, monoid_template<MT2, MT3>>` and `monoid_template<monoid_template<MT1, MT2>, MT3>` yield equivalent types
- a note on equivalence: as we cannot rely on the compiler to recognize types generated by different orders of specialzing a template as equal, we additionally need to define under what conditions we consider two types to be equivalent (e.g. by equal value of a static member) when checking for identity and and associativity properties
- pratical applications: the idea of defining a monoidal class template arose from the desire to come up perform certain checks on a set of types at compile time. To this end, one could specialize a monoidal template with each of the subtypes respectively (or any subset of those types).  The resulting specializations can then be aggregated in a fold-like fashion, again using the same monoidal template

#### relevant interfaces:

##### comparator class-template:
- two of the following property checks require a user provided template template paramter `Comparator` that can be specialized by two type arguments to determine the those two types are to be considered equivalent within the context of this check
- to this end, specializations of the comparator template are required to a `static constexpr bool` member named `value`, which is checked via `requires` clause in the relevant metafunctions

##### check_closure_property_v:
`template <template <typename... Ts> class Class_Templ, size_t order, typename... Ts>`<br>
`constexpr inline bool check_closure_property_v`<br>
- variable template to determine whether `Class_Templ` fullfills the closure property w.r.t. `Ts`, i.e. whether `Class_Templ`can be spcialized with any two distinct types out of all the specializations of `Class_Templ` from all the possible non-repeating combinations of size `order` out of types `Ts`
- `Ts`can be passed as a raw type pack or as a single type specializing `param_pack::type_pack_t`

##### check_closure_property_v:
`template <template <typename... > class Class_Templ,
          template <typename, typename> class Comparator, size_t order, typename... Ts>`<br>
`constexpr inline bool check_associativity_property_v`<br>
- variable template to determine whether `Class_Templ` fulfills the associativity property w.r.t. `Ts`
- as verifying associativity includes checking for equality, a class template `Comparator` needs to be provived to determine whether specializations of `Class_Templ` are to be considered equivalent within the context of the of the check   
- `Ts`can be passed as a raw type pack or as a single type specializing `param_pack::type_pack_t`

##### check_identity_element_property_v:
`template <template <typename... Ts> class Class_Templ,
          template <typename M1, typename M2> class Comparator, size_t order, typename... Ts>`<br>
`constexpr inline bool check_identity_element_property_v`<br>
- variable template to determine whether `Class_Templ` fulfills the identity-element property w.r.t. `Ts`
- as verifying identity includes checking for equality, a class template `Comparator` needs to be provived to determine whether specializations of `Class_Templ` are to be considered equivalent within the context of the of the check   
- `Ts`can be passed as a raw type pack or as a single type specializing `param_pack::type_pack_t`

##### bool monoidal_class_template_v:
`template <template <typename... > class Class_Templ,
          template <typename MA, typename MB> class Comparator, size_t order, typename... Ts>`<br>
constexpr inline bool monoidal_class_template_v`<br>
- combines checks for closure, associativity and identity for `Class_Templ` w.r.t. `Ts`
- comparator needs to be provided for associativity and identity checks
- `Ts`can be passed as a raw type pack or as a single type specializing `param_pack::type_pack_t`

---

### type_pack_check:
- found in namespace `type_pack_t` in file `type_pack_check.hpp`
- provides functionality to perform compile time checks over set of types

##### checker_aggregator_check_t:
`template<template<typename...> class Type_Handler, template<typename, typename> class Aggregator, size_t order, typename... Ts>`<br>
`using checker_aggregator_check_t`
- uses every non-repeating compination of length `order` from `Ts` to specialize template `Type_Handler` and aggregates all specializations in a fold-like fashion using class template `Aggregator`
- `Ts` can be passed as a regualar type-parameter-pack or as a specialization of `param_pack::type_pack_t`


##### monoid_check_t:
`template<template<typename...> class Monoid_Templ, size_t order, typename... Ts>`<br>
`using monoid_check_t = monoid_check<Monoid_Templ,order,Ts...>`
- uses every non-repeating compination of length `order` from `Ts` to specialize template `Monoid_Templ` and then uses `Monoid_Templ` to aggregate all specializations in a fold-like fashion
- for `Monoid_Templ` to be able to be used in that way, `param_pack::check_closure_property_v<Monoid_Templ, order, Ts...> == true` is required
- `Ts` can be passed as a regualar type-parameter-pack or as a specialization of `param_pack::type_pack_t`
