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

template < class ... type_v >
inline auto gen_tupel(type_v ... arg_v) -> details::tupel < type_v ... > {
	return details::tupel < type_v ... >(arg_v...);
}


void tupel_test() {

	auto select = param(&test_struct::select);

	test_struct v = { 123, false };

	auto tup = details::tupel <
		decltype (param(&test_struct::id)),
		decltype (param(&test_struct::select))
	> (123, false);
	
}


int main (int arg_c, char **arg_v) {

	tupel_test();	

	//int source_a [] = { 8, 5, 7 ,8 ,2, 5, 6, 3, 3, 7, 2, 4, 1};
	//
	//// parameters can be literals, lincxx::item ( the current item while iterating )
	//// as shown above, but also public members of classes.
	//// The supported member types are public fields and methods with no parameters
	//test_struct source_b [] = {{2, true, {"A"}}, {7, false, {"A"}}, {12, false, {"A"}}, {5, true, {"B"}}, {1, true, {"A"}}, {2, false, {"A"}}, {72, false, {"A"}}, {1, true, {"B"}}, {5, true, {"A"}}, {8, true, {"A"}}, {1, true, {"A"}}};
	//
	//auto id_s = param (&test_struct::id);
	//auto select_test = from (source_b).select (id_s);
	//
	//// simple filter example:

	//// select all items lesser than 5
	//// the resulting type will be an iterable query handle
	//// no data copy is done.
	//auto query_a = from (source_a).where (lincxx::item < 5);

	//// the filtering iterator then evaluates the conditional expression
	//// and returns only compliant values
	//for (auto i : query_a) {
	//	std::cout << i << std::endl;
	//}

	//// could also have been writen literally
	//for (auto i : from (source_a).where (lincxx::item < 5)) {
	//	std::cout << i << std::endl;
	//}

	//auto query_b = from (source_b).where (
	//	param (&test_struct::id)
	//	<
	//	param (&test_struct::calculate_stuffs)
	//);

	//// these params can be declared as variables to be easier to define multiple uses.
	//auto id = param (&test_struct::id);

	//auto query_c = from (source_b).where (id < 10 || id > 70);

	//// since the query is iterable
	//// it can be used as source for another query (nesting)
	//auto query_d =
	//	from (
	//		from (source_b).where (id < 10)
	//	).where (id > 5);

	//// you can also send a visitor anonymous function to browse through the 
	//// condition's compliant items
	//from (source_b).where (id < 10).visit ([](test_struct i) { std::cout << i.id << std::endl; });

	//// if a copy of results is desired
	//// just use the to_list function
	//// this pushes all compliant items into a list < T > 
	//// container
	//auto list_res = from (source_b).where (id < 10).to_list ();

	//// if the condition expression needs to be more complex than supported
	//// for example requiring the access to fields of fields you can use an anonymous function
	//// that takes in item as parameter and returns a boolean 
	//auto query_e = from (source_b).where ([](test_struct & i) {
	//	return i.id < 10 && i.ref.x != "A";
	//});

	//// first or default
	//query_e.first_or_default ({0});
	//query_e.first_or_default ({0}, id > 3);

	//query_e.count ();

	//auto dist = from (source_a).where (lincxx::item < 5).distinct ();

	//auto tupel_query = query_e.select (id);
}