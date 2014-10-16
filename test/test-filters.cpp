#include <gtest/gtest.h>
#include <lincxx.hpp>
#include <vector>
#include "test_tools.h"

class test_filters : public ::testing::Test {
protected:

	struct test_struct_deep {
		std::string x;
	};

	struct test_struct {

		int id;
		bool select;
		test_struct_deep ref;

		inline int calculate_stuffs() { return id * 2; }

		inline bool operator == (const test_struct & v) const {
			return
				id == v.id &&
				select == v.select &&
				ref.x == v.ref.x;
		}

	};

	// data ---
	static int source_array_int [];
	static test_struct source_array_struct [];
	static std::vector < int > source_vector_int;

	static const int source_const_array_int[];
	static const test_struct source_const_array_struct[];
	static const std::vector < int > source_const_vector_int;
	// --------

	virtual void SetUp () {}

	virtual void TearDown () {}

};

int test_filters::source_array_int [] = { 8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1 };
test_filters::test_struct test_filters::source_array_struct [] = { { 2, true, { "A" } }, { 7, false, { "A" } }, { 12, false, { "A" } }, { 5, true, { "B" } }, { 1, true, { "A" } }, { 2, false, { "A" } }, { 72, false, { "A" } }, { 1, true, { "B" } }, { 5, true, { "A" } }, { 8, true, { "A" } }, { 1, true, { "A" } } };
std::vector < int > test_filters::source_vector_int = { 8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1 };

const int test_filters::source_const_array_int[] = { 8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1 };
const test_filters::test_struct test_filters::source_const_array_struct[] = { { 2, true, { "A" } }, { 7, false, { "A" } }, { 12, false, { "A" } }, { 5, true, { "B" } }, { 1, true, { "A" } }, { 2, false, { "A" } }, { 72, false, { "A" } }, { 1, true, { "B" } }, { 5, true, { "A" } }, { 8, true, { "A" } }, { 1, true, { "A" } } };
const std::vector < int > test_filters::source_const_vector_int = { 8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1 };

template < class source_type, class expression >
inline void test_expression(source_type & source, expression & condition) {
	using namespace lincxx;
	using namespace lincxx_test;
	// prepare expected result
	auto result = prepare_expected_result(source, condition);

	auto query = from(source).where(condition);

	size_t res_index = 0;
	size_t res_size = source_size(result);

	for (auto v : query) {
		if (res_index >= res_size)
			GTEST_FAIL() << "Unexpected return value count!";

		ASSERT_EQ(result[res_index], v);
		++res_index;
	}
}

TEST_F (test_filters, int_array_less_5) {
	test_expression(source_array_int, [](const int & i) { return i < 5; });
}

TEST_F(test_filters, struct_array_less_5) {
	test_expression(source_array_struct, [](const test_struct & i) { return i.id < 5; });
}

TEST_F(test_filters, int_vector_less_5) {
	test_expression(source_vector_int, [](const int & i) { return i < 5; });
}

TEST_F(test_filters, const_int_array_less_5) {
	test_expression(source_const_array_int, [](const int & i) { return i < 5; });
}

TEST_F(test_filters, const_struct_array_less_5) {
	test_expression(source_const_array_struct, [](const test_struct & i) { return i.id < 5; });
}

TEST_F(test_filters, const_int_vector_less_5) {
	test_expression(source_const_vector_int, [](const int & i) { return i < 5; });
}