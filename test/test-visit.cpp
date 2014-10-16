#include <gtest/gtest.h>
#include <lincxx.hpp>
#include <vector>
#include "test_tools.h"

class test_visit : public ::testing::Test {
protected:

	struct test_struct_deep {
		std::string x;
	};

	struct test_struct {

		int id;
		bool select;
		test_struct_deep ref;

		inline int calculate_stuffs () { return id * 2; }

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

	static const int source_const_array_int [];
	static const test_struct source_const_array_struct [];
	static const std::vector < int > source_const_vector_int;
	// --------

	virtual void SetUp () {}

	virtual void TearDown () {}

};

int 
test_visit::source_array_int [] = {8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1};

test_visit::test_struct 
test_visit::source_array_struct [] = {{2, true, {"A"}}, {7, false, {"A"}}, {12, false, {"A"}}, {5, true, {"B"}}, {1, true, {"A"}}, {2, false, {"A"}}, {72, false, {"A"}}, {1, true, {"B"}}, {5, true, {"A"}}, {8, true, {"A"}}, {1, true, {"A"}}};

std::vector < int > 
test_visit::source_vector_int = {8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1};

const int 
test_visit::source_const_array_int [] = {8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1};

const test_visit::test_struct 
test_visit::source_const_array_struct [] = {{2, true, {"A"}}, {7, false, {"A"}}, {12, false, {"A"}}, {5, true, {"B"}}, {1, true, {"A"}}, {2, false, {"A"}}, {72, false, {"A"}}, {1, true, {"B"}}, {5, true, {"A"}}, {8, true, {"A"}}, {1, true, {"A"}}};

const std::vector < int > 
test_visit::source_const_vector_int = {8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1};

template < class value_type, class source_type >
inline void test_visitor (source_type & source) {
	using namespace lincxx;
	using namespace lincxx_test;

	std::vector < value_type > result;

	from (source).visit ([&](value_type & i) { result.push_back (i); });

	size_t index = 0;
	size_t expected_size = source_size (source);
	size_t result_size = source_size (result);

	ASSERT_EQ (expected_size, result_size);
	
	for (size_t i = 0; i < result_size; ++i) {
		ASSERT_EQ (source [i], result [i]);
	}
}

template < class value_type, class source_type, class expression >
inline void test_visitor_filtered (source_type & source, expression & condition) {
	using namespace lincxx;
	using namespace lincxx_test;

	// prepare expected result
	auto expected = prepare_expected_result (source, condition);

	std::vector < value_type > result;

	from (source).where (condition).visit ([&](value_type & i) { result.push_back (i); });

	size_t index = 0;
	size_t expected_size = source_size (expected);
	size_t result_size = source_size (result);

	ASSERT_EQ (expected_size, result_size);

	for (size_t i = 0; i < result_size; ++i) {
		ASSERT_EQ (expected [i], result [i]);
	}
}

TEST_F (test_visit, int_array_visit) {
	test_visitor < int > (source_array_int);
}

TEST_F (test_visit, struct_array_visit) {
	test_visitor < test_struct > (source_array_struct);
}

TEST_F (test_visit, int_vector_visit) {
	test_visitor < int > (source_vector_int);
}

TEST_F (test_visit, const_int_array_visit) {
	test_visitor < const int > (source_const_array_int);
}

TEST_F (test_visit, const_struct_array_visit) {
	test_visitor < const test_struct > (source_const_array_struct);
}

TEST_F (test_visit, const_int_vector_visit) {
	test_visitor < int > (source_const_vector_int);
}

TEST_F (test_visit, int_array_visit_filtered) {
	test_visitor_filtered < int > (source_array_int, [](const int & i) { return i < 5; });
}

TEST_F (test_visit, struct_array_visit_filtered) {
	test_visitor_filtered < test_struct > (source_array_struct, [](const test_struct & i) { return i.id < 5; });
}

TEST_F (test_visit, int_vector_visit_filtered) {
	test_visitor_filtered < int > (source_vector_int, [](const int & i) { return i < 5; });
}

TEST_F (test_visit, const_int_array_visit_filtered) {
	test_visitor_filtered < const int > (source_const_array_int, [](const int & i) { return i < 5; });
}

TEST_F (test_visit, const_struct_array_visit_filtered) {
	test_visitor_filtered < const test_struct > (source_const_array_struct, [](const test_struct & i) { return i.id < 5; });
}

TEST_F (test_visit, const_int_vector_visit_filtered) {
	test_visitor_filtered < int > (source_const_vector_int, [](const int & i) { return i < 5; });
}

TEST_F (test_visit, change_source ) {
	using namespace lincxx;
	using namespace lincxx_test;

	const int change_value = 10;
	
	std::vector < int > source = {1, 2, 3, 4, 5, 6, 7};

	from (source).visit ([&](int & i) { i = change_value; });

	for (int & i : source) {
		ASSERT_EQ (i, change_value);
	}
}