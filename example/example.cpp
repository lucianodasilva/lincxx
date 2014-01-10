#include <lincxx.h>
#include <vector>	   
#include <iostream>

using namespace lincxx;

struct test_struct {

	int id;

	inline int calculate_stuffs () { return id * 2; }

};

int main (int arg_c, char **arg_v) {

	int source_a [] = { 3, 5, 7 ,8 ,2, 5, 6, 3, 3, 7, 2, 4, 1};

	// simple filter example
	// select all items lesser than 5
	// the resulting type will be a std::vector < int >
	// but may be subject to change
	auto res_a = from (source_a).where (lincxx::item < 5).select ();

	// parameters can be literals, lincxx::item ( the current item while iterating )
	// as shown above, but also public members of classes.
	// These members are fields and methods with no parameters
	test_struct source_b [] = {{2}, {7}, {12}, {5}, {1}, {2}, {72}, {1}, {5}, {8}, {1}};

	auto res_b = from (source_b).where (
		param (&test_struct::id) 
		< 
		param (&test_struct::calculate_stuffs)
	).select ();

	// these params can be defined to be easier to define multiple uses.
	auto id = param (&test_struct::id);

	auto res_c = from (source_b).where (id < 10 || id > 70).select ();

	// since the result is itself a range for compatible container
	// it can be used as source for another query
	auto res_d =
		from (
			from (source_b).where (id < 10).select ()
		).where (id > 5).select ();

	// finally you can send a visitor anonymous function to the 
	// selected items
	from (source_b).where (id < 10) [
		[](test_struct i) { std::cout << i.id << std::endl; }
	].select ();
}