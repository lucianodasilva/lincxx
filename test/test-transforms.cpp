#include <gtest/gtest.h>
#include <lincxx.hpp>
#include <vector>

class test_transforms : public ::testing::Test {
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
	static test_struct source_array_struct [];
	// --------

	virtual void SetUp () {}

	virtual void TearDown () {}

};

test_transforms::test_struct test_transforms::source_array_struct [] = { { 2, true, { "A" } }, { 7, false, { "A" } }, { 12, false, { "A" } }, { 5, true, { "B" } }, { 1, true, { "A" } }, { 2, false, { "A" } }, { 72, false, { "A" } }, { 1, true, { "B" } }, { 5, true, { "A" } }, { 8, true, { "A" } }, { 1, true, { "A" } } };

// These tests are more of a "does it build" kind of test than execution
TEST_F (test_transforms, transform_tupel_2) {
	using namespace lincxx;

	auto transform_query =
		from(source_array_struct)
		.where([](const test_struct & i) { return i.select; })
		.select(
			&test_struct::id,
			&test_struct::select
		);

	for (auto i : transform_query) {
		int id_value = std::get < 0 >(i);
		bool select_value = std::get < 1 >(i);

		if (select_value != true) {
			GTEST_FAIL() << "Unexpected test_struct::select member value";
		}
	}
}

TEST_F(test_transforms, transform_single_value_sum) {
	using namespace lincxx;

	auto transform_query =
		from(source_array_struct)
		.where([](const test_struct & i) { return i.select; })
		.select(
			&test_struct::id
		);

	int id_sum = 0;
	for (auto i : transform_query) {
		id_sum += i;
	}

	int query_id_sum = transform_query.sum();

	EXPECT_EQ(id_sum, query_id_sum);
}