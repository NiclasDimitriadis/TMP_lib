#include "Test_Includes.hpp"

constexpr auto test_lambda= [](int a, double b) { return false; };

static_assert(std::is_same_v<function_signature::ret_type_t<test_lambda>, bool>);
static_assert(std::is_same_v<function_signature::arg_types_t<test_lambda>, param_pack::type_pack_t<int, double>>);

bool test_function(int a, double b) { return false; };

static_assert(std::is_same_v<function_signature::ret_type_t<test_function>, bool>);
static_assert(std::is_same_v<function_signature::arg_types_t<test_function>, param_pack::type_pack_t<int, double>>);

struct test_callable_class {
  int i;
  bool operator()(int a, double b) { return false; };
};

static_assert(std::is_same_v<function_signature::ret_type_t<test_callable_class{}>, bool>);
static_assert(std::is_same_v<function_signature::arg_types_t<test_callable_class{}>, param_pack::type_pack_t<int, double>>);

int main() {
  std::cout << "All test cases for function_signature.hpp compiled successfully" << "\n";
}
