#include "Test_Includes.hpp"

template<int I>
struct INT_ {
   static constexpr int value = I;
};

using ONE = INT_<1>;
using TWO = INT_<2>;
using THREE = INT_<3>;
using FOUR = INT_<4>;
using FIVE = INT_<5>;

template<typename T>
struct EXTRACT_VAL {
   static constexpr auto value = T::value;
};

template<typename T1, typename T2>
struct MAX_AGGR {
   static constexpr auto value = std::max(T1::value, T2::value);
};

template<typename T1, typename T2>
struct EXTRACT_NE {
   static constexpr bool value = T1::value != T2::value;
};

template<typename T1, typename T2>
struct AND {
   static constexpr bool value = T1::value && T2::value;
};

using max_val_functor_fold = type_pack_check::checker_aggregator_check_t<EXTRACT_VAL, MAX_AGGR, 1, ONE, TWO, THREE, FOUR, FIVE>;
static_assert(max_val_functor_fold::value == 5);

using no_equal_vals = type_pack_check::checker_aggregator_check_t<EXTRACT_NE, AND, 2, ONE, TWO, THREE, FOUR, FIVE>;
static_assert(no_equal_vals::value == true);

int main() {
   std::cout << "All test cases for type_pack_check.hpp compiled successfully" << "\n";
};
