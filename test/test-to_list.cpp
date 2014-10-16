#include <gtest/gtest.h>
#include <lincxx.hpp>
#include <vector>

class test_to_list : public ::testing::Test {
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
test_to_list::source_array_int [] = {8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1};

test_to_list::test_struct 
test_to_list::source_array_struct [] = {{2, true, {"A"}}, {7, false, {"A"}}, {12, false, {"A"}}, {5, true, {"B"}}, {1, true, {"A"}}, {2, false, {"A"}}, {72, false, {"A"}}, {1, true, {"B"}}, {5, true, {"A"}}, {8, true, {"A"}}, {1, true, {"A"}}};

std::vector < int > 
test_to_list::source_vector_int = {8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1};

const int 
test_to_list::source_const_array_int [] = {8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1};

const test_to_list::test_struct 
test_to_list::source_const_array_struct [] = {{2, true, {"A"}}, {7, false, {"A"}}, {12, false, {"A"}}, {5, true, {"B"}}, {1, true, {"A"}}, {2, false, {"A"}}, {72, false, {"A"}}, {1, true, {"B"}}, {5, true, {"A"}}, {8, true, {"A"}}, {1, true, {"A"}}};

const std::vector < int > 
test_to_list::source_const_vector_int = {8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1};

template < class source_type >
inline size_t source_size (source_type & source) {
	return source.size ();
}

template < class item_type, size_t size >
inline size_t source_size (item_type (&source) [size]) {
	return size;
}

template < class source_type, class expression >
inline std::vector < typename source_type::value_type > prepare_expected_result (source_type & source, expression & condition) {
	std::vector < typename source_type::value_type > list;

	for (auto v : source) {
		if (condition (v))
			list.push_back (v);
	}

	return list;
}

template < class item_type, size_t size, class expression >
inline std::vector < item_type > prepare_expected_result (item_type (&source) [size], expression & condition) {
	std::vector < item_type > list;

	for (auto v : source) {
		if (condition (v))
			list.push_back (v);
	}

	return list;
}

template < class source_type, class expression >
inline void test_expression (source_type & source, expression & condition) {
	using namespace lincxx;
	// prepare expected result
	auto expected = prepare_expected_result (source, condition);

	auto result = from (source).where (condition).to_list ();

	size_t index = 0;
	size_t expected_size = source_size (expected);
	size_t result_size = source_size (result);

	ASSERT_EQ (expected_size, result_size);

	for (size_t i = 0; i < result_size; ++i) {
		ASSERT_EQ (expected [i], result [i]);
	}
}

TEST_F (test_to_list, int_array_less_5) {
	test_expression (source_array_int, [](const int & i) { return i < 5; });
}

TEST_F (test_to_list, struct_array_less_5) {
	test_expression (source_array_struct, [](const test_struct & i) { return i.id < 5; });
}

TEST_F (test_to_list, int_vector_less_5) {
	test_expression (source_vector_int, [](const int & i) { return i < 5; });
}

TEST_F (test_to_list, const_int_array_less_5) {
	test_expression (source_const_array_int, [](const int & i) { return i < 5; });
}

TEST_F (test_to_list, const_struct_array_less_5) {
	test_expression (source_const_array_struct, [](const test_struct & i) { return i.id < 5; });
}

TEST_F (test_to_list, const_int_vector_less_5) {
	test_expression (source_const_vector_int, [](const int & i) { return i < 5; });
}

TEST_F (test_to_list, empty_list) {
	std::vector < int > empty_list;
	test_expression (empty_list, [](const int & i) { return i < 5; });
}