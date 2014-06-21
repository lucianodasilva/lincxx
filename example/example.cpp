#include <lincxx.h>
#include <vector>	   
#include <iostream>

using namespace lincxx;

struct test_struct {

	int id;

	inline int calculate_stuffs () { return id * 2; }

};

int main (int arg_c, char **arg_v) {

	int source_a [] = { 8, 5, 7 ,8 ,2, 5, 6, 3, 3, 7, 2, 4, 1};

	auto res_z = from (source_a).where (lincxx::item < 5);
	for (auto i : res_z) {
		std::cout << i << std::endl;
	}

	// simple filter example
	// select all items lesser than 5
	// the resulting type will be a std::vector < int >
	// but may be subject to change
	auto res_a = from (source_a).where (lincxx::item < 5).to_list ();

	// parameters can be literals, lincxx::item ( the current item while iterating )
	// as shown above, but also public members of classes.
	// These members are fields and methods with no parameters
	test_struct source_b [] = {{2}, {7}, {12}, {5}, {1}, {2}, {72}, {1}, {5}, {8}, {1}};

	auto res_b = from (source_b).where (
		param (&test_struct::id)
		<
		param (&test_struct::calculate_stuffs)
		).to_list ();

	// these params can be defined to be easier to define multiple uses.
	auto id = param (&test_struct::id);

	auto res_c = from (source_b).where (id < 10 || id > 70).to_list ();

	// since the result is itself a range for compatible container
	// it can be used as source for another query
	auto res_d =
		from (
			from (source_b).where (id < 10)
		).where (id > 5).to_list ();

	// finally you can send a visitor anonymous function to the 
	// selected items
	//from (source_b).where (id < 10) [
	//	[](test_struct i) { std::cout << i.id << std::endl; }
	//].to_list ();

	// if you wish to only have a "filtered" visitor
	// without returning the selected items call visit instead
	//from (source_b).where (id < 10) [
	//	[](test_struct i) {std::cout << i.id << std::endl; }
	//].visit ();

	from (source_b).where (id < 10).visit ([](test_struct i) {std::cout << i.id << std::endl; });
}