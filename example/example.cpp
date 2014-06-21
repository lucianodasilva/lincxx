#include <lincxx.hpp>
#include <vector>	   
#include <iostream>

using namespace lincxx;

struct test_struct {

	int id;

	inline int calculate_stuffs () { return id * 2; }

};

int main (int arg_c, char **arg_v) {

	int source_a [] = { 8, 5, 7 ,8 ,2, 5, 6, 3, 3, 7, 2, 4, 1};

	// simple filter example:

	// select all items lesser than 5
	// the resulting type will be an iterable query handle
	// no data copy is done.
	auto query_a = from (source_a).where (lincxx::item < 5);

	// the filtering iterator then evaluates the conditional expression
	// and returns only compliant values
	for (auto i : query_a) {
		std::cout << i << std::endl;
	}

	// could also have been writen literally
	for (auto i : from (source_a).where (lincxx::item < 5)) {
		std::cout << i << std::endl;
	}

	// parameters can be literals, lincxx::item ( the current item while iterating )
	// as shown above, but also public members of classes.
	// The supported member types are public fields and methods with no parameters
	test_struct source_b [] = {{2}, {7}, {12}, {5}, {1}, {2}, {72}, {1}, {5}, {8}, {1}};

	auto query_b = from (source_b).where (
		param (&test_struct::id)
		<
		param (&test_struct::calculate_stuffs)
	);

	// these params can be declared as variables to be easier to define multiple uses.
	auto id = param (&test_struct::id);

	auto query_c = from (source_b).where (id < 10 || id > 70).to_list ();

	// since the query is iterable
	// it can be used as source for another query (nesting)
	auto query_d =
		from (
			from (source_b).where (id < 10)
		).where (id > 5);

	// you can also send a visitor anonymous function to browse through the 
	// condition's compliant items
	from (source_b).where (id < 10).visit ([](test_struct i) { std::cout << i.id << std::endl; });

	// if a copy of results is desired
	// just use the to_list function
	// this pushes all compliant items into a list < T > 
	// container
	auto list_res = from (source_b).where (id < 10).to_list ();
}