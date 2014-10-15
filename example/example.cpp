#include <lincxx.hpp>
#include <vector>	   
#include <iostream>
#include <string>
#include <tuple>

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

const int const_source_a[] = { 8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1 };

const std::vector < int > issue_source = {8, 5, 7, 8, 2, 5, 6, 3, 3, 7, 2, 4, 1};

int main (int arg_c, char **arg_v) {

	auto q = from (issue_source);

	auto v = issue_source.begin ();
	auto x = *v;

	for (int i : q) {
		std::cout << i << std::endl;
	}

	return 0;

	// simple filter example:

	// select all items lesser than 5
	// the resulting type will be an iterable query handle
	// no data copy is done.
	auto query_a = from(source_a).where([](const int & i){ return i < 5; });

	// the filtering iterator then evaluates the conditional expression
	// and returns only compliant values
	for (int & i : query_a) {
		std::cout << i << std::endl;
	}

	// could also have been writen literally into the for cicle
	for (int & i : from(source_a).where([](const int & i){ return i < 5; })) {
		std::cout << i << std::endl;
	}

	// since the query is iterable
	// it can be used as source for another query (nesting)
	auto query_b =
		from(
			from(source_b).where(
				[](const test_struct & i) { return i.id < 10; }
			)
		).where(
			[](const test_struct & i) { return i.id > 5; }
		);
	
	query_b.visit([](const test_struct & i) { std::cout << i.id << " "; });

	// you can also send a visitor anonymous function to browse through the 
	// condition's compliant items
	from(source_b).where(
		[](const test_struct & i) { return i.id < 10; }
	).visit(
		[](const test_struct & i) { std::cout << i.id << std::endl; }
	);

	// if a copy of results is desired
	// just use the to_list function
	// this pushes all compliant items into a vector < T > 
	// container
	auto list_res = from (source_b).where (
		[](const test_struct & i) { return i.id < 10; }
	).to_list ();

	// first or default
	auto first = test_struct();
	
	// return the first value in the list
	// in case the list is empty returns the supplied default value
	first = query_b.first_or_default({0});

	// first value compliant with the query that is also
	// compliant with the condition parameter
	first = query_b.first_or_default(
		{0}, 
		[](const test_struct & i) { return i.id > 5; }
	);

	//  Size returns the number of items that exist
	// in the source list that are compliant with
	// the established filter.
	//  Take into consideration that this forces
	// the query to sweep through items to count
	// them.
	size_t size_value = query_b.size ();

	std::cout << "Size: " << size_value << std::endl;

	// distinct returns a list of unique values within
	// the filtered set
	auto dist = from(source_a).where(
		[](const int & i) { return i < 10; }
	).distinct();

	// You can transform the type of the 
	// result of the running query.
	// just select what members of the 
	// source class you want to have 
	// copied into the results
	auto transforming_query =
		from(source_b).where(
			[](const test_struct & i) { return i.id < 5; }
		).select(
			&test_struct::id,
			&test_struct::select
		);

	for (auto v : transforming_query) {
		auto id = std::get < 0 >(v);
		auto sel = std::get < 1 >(v);

		std::cout << "ID: " << id << " SEL: " << sel << std::endl;
	}

	// if you choose to select a single member
	// the returning type will be a copy 
	// of the same type as the selected member
	auto transforming_query_b =
		from(source_b).where(
			[](const test_struct & i) { return i.id < 5; }
		).select(
			&test_struct::id
		);

	// this way no tuple access is needed
	for (auto v : transforming_query_b) {
		std::cout << v << std::endl;
	}

	// if its possible to apply a sum operation (+) to the 
	// selected type this is possible to execute a sum operation
	int id_sum = from(source_b).select(&test_struct::id).sum ();
	std::cout << "id_sum: " << id_sum << std::endl;

}