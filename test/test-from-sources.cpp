#include <gtest/gtest.h>
#include <lincxx.hpp>
#include <vector>

class test_from_sources : public ::testing::Test {
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

	static const int source_const_array_int [];
	static const test_struct source_const_array_struct[];
	static const std::vector < int > source_const_vector_int;
	// --------

	virtual void SetUp () {}

	virtual void TearDown () {}

};

int test_from_sources::source_array_int [] = { 8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1 };
test_from_sources::test_struct test_from_sources::source_array_struct [] = { { 2, true, { "A" } }, { 7, false, { "A" } }, { 12, false, { "A" } }, { 5, true, { "B" } }, { 1, true, { "A" } }, { 2, false, { "A" } }, { 72, false, { "A" } }, { 1, true, { "B" } }, { 5, true, { "A" } }, { 8, true, { "A" } }, { 1, true, { "A" } } };
std::vector < int > test_from_sources::source_vector_int = { 8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1 };

const int test_from_sources::source_const_array_int[] = { 8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1 };
const test_from_sources::test_struct test_from_sources::source_const_array_struct[] = { { 2, true, { "A" } }, { 7, false, { "A" } }, { 12, false, { "A" } }, { 5, true, { "B" } }, { 1, true, { "A" } }, { 2, false, { "A" } }, { 72, false, { "A" } }, { 1, true, { "B" } }, { 5, true, { "A" } }, { 8, true, { "A" } }, { 1, true, { "A" } } };
const std::vector < int > test_from_sources::source_const_vector_int = { 8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1 };

template < class t, size_t length >
inline size_t sizeof_array(t(&array_inst)[length]) {
	return length;
}

TEST_F (test_from_sources, from_array_fundamental_type) {
	using namespace lincxx;

	auto query = from(source_array_int);
	size_t query_count = 0;

	for (auto v : query) {
		ASSERT_EQ(v, source_array_int[query_count]);
		++query_count;
	}

	ASSERT_EQ(sizeof_array(source_array_int), query_count);
}

TEST_F(test_from_sources, from_array_structs) {
	using namespace lincxx;

	auto query = from(source_array_struct);
	size_t query_count = 0;

	for (auto v : query) {
		ASSERT_EQ(v, source_array_struct[query_count]);
		++query_count;
	}

	ASSERT_EQ(sizeof_array(source_array_struct), query_count);
}

TEST_F(test_from_sources, from_vector_fundamental_type) {
	using namespace lincxx;

	auto query = from(source_vector_int);
	size_t query_count = 0;

	for (auto v : query) {
		ASSERT_EQ(v, source_vector_int [query_count]);
		++query_count;
	}

	ASSERT_EQ(source_vector_int.size (), query_count);
}

TEST_F(test_from_sources, from_const_array_fundamental_type) {
	using namespace lincxx;

	auto query = from(source_const_array_int);
	size_t query_count = 0;

	for (auto v : query) {
		ASSERT_EQ (v, source_const_array_int [query_count]);
		++query_count;
	}

	ASSERT_EQ (sizeof_array (source_const_array_int), query_count);
}

TEST_F(test_from_sources, from_const_array_structs) {
	using namespace lincxx;

	auto query = from(source_const_array_struct);
	size_t query_count = 0;

	for (auto v : query) {
		ASSERT_EQ (v, source_const_array_struct [query_count]);
		++query_count;
	}

	ASSERT_EQ (sizeof_array (source_const_array_struct), query_count);
}

TEST_F(test_from_sources, from_const_vector_fundamental_type) {
	using namespace lincxx;

	auto query = from(source_const_vector_int);
	size_t query_count = 0;

	for (auto v : query) {
		ASSERT_EQ (v, source_const_vector_int [query_count]);
		++query_count;
	}

	ASSERT_EQ (source_const_vector_int.size (), query_count);
}

TEST_F(test_from_sources, from_query) {
	using namespace lincxx;

	auto query_a = from(source_vector_int);
	auto query_b = from(query_a);

	size_t query_count = 0;

	for (auto v : query_b) {
		ASSERT_EQ (v, source_vector_int [query_count]);
		++query_count;
	}

	ASSERT_EQ (source_vector_int.size (), query_count);
}