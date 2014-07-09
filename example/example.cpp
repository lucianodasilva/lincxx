#include <lincxx.hpp>
#include <vector>	   
#include <iostream>
#include <string>

using namespace lincxx;

struct test_struct_deep {
	std::string x;
};

struct test_struct {

	int id;
	bool select;
	test_struct_deep ref;

	inline int calculate_stuffs () { return id * 2; }
};

int source_a[] = { 8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1 };
test_struct source_b[] = { { 2, true, { "A" } }, { 7, false, { "A" } }, { 12, false, { "A" } }, { 5, true, { "B" } }, { 1, true, { "A" } }, { 2, false, { "A" } }, { 72, false, { "A" } }, { 1, true, { "B" } }, { 5, true, { "A" } }, { 8, true, { "A" } }, { 1, true, { "A" } } };

int main (int arg_c, char **arg_v) {

	// simple filter example:

	// select all items lesser than 5
	// the resulting type will be an iterable query handle
	// no data copy is done.
	auto query_a = from(source_a).where([](const int & i){ return i < 5; });

	// the filtering iterator then evaluates the conditional expression
	// and returns only compliant values
	for (auto i : query_a) {
		std::cout << i << std::endl;
	}

	// could also have been writen literally into the for cicle
	for (auto i : from(source_a).where([](const int & i){ return i < 5; })) {
		std::cout << i << std::endl;
	}

	// since the query is iterable
	// it can be used as source for another query (nesting)
	auto query_b =
		from (
			from(source_b).where(
				[](const test_struct & i) { return i.id < 10; }
			)
		).where(
			[](const test_struct & i) { return i.id < 5; }
		);

	// you can also send a visitor anonymous function to browse through the 
	// condition's compliant items
	from(source_b).where(
		[](const test_struct & i) { return i.id < 10; }
	).visit(
		[](const test_struct & i) { std::cout << i.id << std::endl; }
	);

	// if a copy of results is desired
	// just use the to_list function
	// this pushes all compliant items into a list < T > 
	// container
	auto list_res = from (source_b).where (
		[](const test_struct & i) { return i.id < 10; }
	).to_list ();

	// first or default
	auto first = test_struct();
	
	// return the first value in the list
	// in case the list is empty returns the supplied default value
	first = query_b.first_or_default({ 0 });

	// first value compliant with the query that is also
	// compliant with the condition parameter
	first = query_b.first_or_default(
		{ 0 }, 
		[](const test_struct & i) { return i.id > 5; }
	);

	int size_value = query_b.size ();

	// distinct returns a list of unique values within
	// the filtered set
	auto dist = from(source_a).where(
		[](const int & i) { return i < 10; }
	).distinct();
}