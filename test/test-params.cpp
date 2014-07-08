#include <gtest/gtest.h>
#include <lincxx.hpp>
#include <vector>

class test_params : public ::testing::Test {
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

	virtual void SetUp () {}

	virtual void TearDown () {}

};

template < class t, size_t length >
inline size_t sizeof_array(t(&array_inst)[length]) {
	return length;
}

TEST_F (test_params, param_literal) {
	using namespace lincxx;
	
	auto literal_parameter = param(123);
	auto literal_parameter_value = literal_parameter.get_value("dummy");

	EXPECT_EQ(literal_parameter_value, 123);
}
