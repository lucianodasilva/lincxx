#ifndef _lincxx_h_
#define _lincxx_h_

#include <list>
#include <map>
#include <vector>
#include <functional>

namespace lincxx {

	namespace details {

		namespace tools {
		
			template < class t >
			struct add : public std::binary_function < t, t, t > {
		
				inline t operator () (const t & v1, const t & v2) const {
					return v1 + v2;
				}
			};
		
		}
		
		// -------------------------------------------------------------------------------------
		// source type abstraction
		// -------------------------------------------------------------------------------------
		template < class array_value_type, size_t array_size >
		struct array_handle {

			using value_type = array_value_type;
			using iterator = array_value_type *;
			using const_iterator = const array_value_type *;
			using reference = array_value_type &;
			using const_reference = const array_value_type &;

			array_value_type (& data) [array_size];

			inline iterator begin () { return +data; }
			inline iterator end () { return data + array_size; }

			inline const_iterator begin () const { return +data; }
			inline const_iterator end () const { return data + array_size; }
		};

		template < class source_type >
		struct list_handle {

			using value_type = typename source_type::value_type;
			using iterator = typename source_type::iterator;
			using const_iterator = typename source_type::const_iterator;
			using reference = typename source_type::reference;
			using const_reference = typename source_type::const_reference;

			source_type & data;

			inline iterator begin () { return data.begin (); }
			inline iterator end () { return data.end (); }

			inline const_iterator begin () const { return data.begin (); }
			inline const_iterator end () const { return data.end(); }
		};

		
		template < class source_type >
		struct list_handle < const source_type > {

			using value_type = typename source_type::value_type;
			using iterator = typename source_type::const_iterator;
			using const_iterator = typename source_type::const_iterator;
			using reference = typename source_type::const_reference;
			using const_reference = typename source_type::const_reference;

			const source_type & data;

			inline const_iterator begin () const { return data.cbegin (); }
			inline const_iterator end () const { return data.cend (); }
		};
		

		// -------------------------------------------------------------------------------------
		// Conditions and Lambda References
		// -------------------------------------------------------------------------------------
		template < class item_type >
		struct null_condition {
			inline bool operator ()(const item_type & i) const { return true; }
		};

		template < class t >
		struct lambda /*: public lambda_ref < decltype (&t::operator ()) >*/ {};

		template < class c_type, class r_type, class ... p_v_type >
		struct lambda < r_type (c_type::*)(p_v_type...) const > {

			using return_type = r_type;
			using class_type = c_type;

			class_type address;

			inline return_type operator () (p_v_type ... arg_v) const { return address (arg_v...); }

		};
		// -------------------------------------------------------------------------------------
		// Type Transformation
		// -------------------------------------------------------------------------------------
		template < class member_type >
		struct accessor {};
		
		template < class class_t, class value_t >
		struct accessor < value_t class_t::* > {
		
			using class_type = class_t;
			using value_type = value_t;
		
			value_t class_type::* address;
		
			inline accessor(value_t class_type::*v) : address(v) {}
		
			inline value_type get(const class_type & ref) const {
				return ref.*address;
			}
		
		};
		
		template < class class_t, class value_t >
		struct accessor < value_t(class_t::*)() > {
		
			using class_type = class_t;
			using value_type = value_t;
		
			value_t(class_type::*address)();
		
			inline accessor(value_t(class_type::*v)()) : address(v) {}
		
			inline value_type get(const class_type & ref) const {
				return ref.*address();
			}
		
		};
		
		template < class t >
		struct extract_accessor_type {
			using value_type = typename t::value_type;
		};
		
		template < class ... t >
		struct extract_accessor_class {};
		
		template < class t, class ... v >
		struct extract_accessor_class < t, v ... > {
			using class_type = typename t::class_type;
		};
		
		template < size_t i, class class_type, class value_type, class ... accessor_types >
		struct tuple_for_each {
			inline static void copy(const class_type & src, const std::tuple < accessor_types... > & accessors, value_type & dst) {
				std::get < i - 1 >(dst) =
					std::get < i - 1 >(accessors).get(src);
				tuple_for_each  < i - 1, class_type, value_type, accessor_types ... >::copy(src, accessors, dst);
			}
		};
		
		template <class class_type, class value_type, class ... accessor_types >
		struct tuple_for_each < 0, class_type, value_type, accessor_types ... > {
			inline static void copy(const class_type & src, const std::tuple < accessor_types... > & accessors, value_type & dst) {}
		};
		
		template < class source_type, class ... accessor_types >
		struct transformer {

			using value_type = std::tuple <
				typename extract_accessor_type < accessor_types >::value_type ...
			>;
		
			using reference = value_type;
			using const_reference = value_type;
		
			using class_type = typename extract_accessor_class < accessor_types ... >::class_type;
		
			std::tuple < accessor_types ... > accessor_instances;
		
			inline transformer(accessor_types ... accessors) : accessor_instances(accessors...) {}
		
			inline reference transform(const class_type & src) const {
				value_type dst;
				tuple_for_each < sizeof...(accessor_types), class_type, value_type, accessor_types ... >::copy(src, accessor_instances, dst);
				return dst;
			}
		
		};
		
		template < class source_type, class t >
		struct transformer < source_type,  t > {
		
			using value_type = typename extract_accessor_type < t >::value_type;
			using reference = value_type;
			using const_reference = value_type;
			using class_type = typename extract_accessor_class < t >::class_type;
		
			t accessor;
		
			inline transformer(t v) : accessor(v) {}
		
			inline reference transform(const class_type & src) const {
				return accessor.get(src);
			}
		
		};
		
		template < class source_type >
		struct null_transformer {
			using value_type = typename source_type::value_type;
			using reference = typename source_type::reference;
			using const_reference = typename source_type::const_reference;

			inline reference transform( reference v) const { return v; }
		};

		// ----------------------

		template < 
			class source_it_type, 
			class condition_type,
			class transformer_type
		>
		struct iterator {

			condition_type & condition;
			transformer_type & transformer;

			mutable source_it_type
				source_it,
				source_end;

			using this_type = iterator < source_it_type, condition_type, transformer_type >;

			inline const this_type & operator ++ () const {
				search_next ();
				return *this;
			}

			inline bool operator == (const this_type & v) const { return source_it == v.source_it; }
			inline bool operator != (const this_type & v) const { return source_it != v.source_it; }

			inline typename transformer_type::reference operator * () const
			{
				return transformer.transform (*source_it);
			}

			inline const this_type & search_first () const {
				while (source_it != source_end && !condition (*source_it))
					++source_it;

				return *this;
			}

			inline const this_type & search_next () const {
				do
					++source_it;
				while (source_it != source_end && !condition (*source_it));

				return *this;
			}

		};

		template < 
			class source_type,
			class condition_type,
			class transformer_type
		>
		struct query {

			source_type source;

			condition_type			
				condition;

			transformer_type
				transformer;

			using value_type = typename transformer_type::value_type;

			using iterator = details::iterator < 
				typename source_type::iterator, 
				condition_type, 
				transformer_type 
			>;

			using const_iterator = const details::iterator < 
				typename source_type::const_iterator, 
				condition_type, 
				transformer_type 
			>;

			using reference = typename transformer_type::reference;
			using const_reference = typename transformer_type::const_reference;

			inline iterator begin () {
				iterator new_it = {
					condition,
					transformer,
					source.begin (),
					source.end ()
				};

				return new_it.search_first ();
			}

			inline iterator end () {
				return {
					condition,
					transformer,
					source.end (),
					source.end ()
				};
			}

			inline const_iterator begin () const {
				const_iterator new_it = {
					condition,
					transformer,
					source.begin (),
					source.end ()
				};

				return new_it.search_first ();
			}

			inline const_iterator end () const {
				return {
					condition,
					transformer,
					source.end (),
					source.end ()
				};
			}

			template < class condition_exp_type >
			inline query < 
				source_type, 
				lambda < decltype (&condition_exp_type::operator ()) >,
				transformer_type
			>
			where (condition_exp_type condition_ref) 
			{
				return {
					source,
					lambda < decltype (&condition_exp_type::operator ()) > {condition_ref},
					transformer
				};
			}

			template < class ... address_types >
			inline query < 
				source_type,
				condition_type,
				details::transformer < source_type, accessor < address_types > ... >
			>
			select(address_types ... accessors) 
			{
				return{
					source,
					condition,
					details::transformer < source_type, accessor < address_types > ... >(accessors...)
				};
			}

			template < class list_type = std::vector < value_type > >
			inline list_type to_list() {
				list_type dest;
			
				for (auto v : *this)
					dest.push_back(v);
			
				return dest;
			}
			
			template < class callback_type >
			inline void visit(const callback_type & call) {
				for (auto i : *this)
					call(i);
			}
			
			inline size_t size() {
				size_t s = 0;
			
				auto
					it = begin(),
					it_end = end();
			
				for (; it != it_end; ++it)
					++s;
			
				return s;
			}
			
			template < class addition = tools::add < value_type > >
			typename addition::result_type sum() {
				typename addition::result_type res = typename addition::result_type();
				addition adder;
			
				for (auto v : *this)
					res = adder(res, v);
			
				return res;
			}
			
			inline value_type first_or_default(const value_type & default_value) {
				auto it = begin();
			
				if (it != end())
					return *it;
			
				return default_value;
			}
			
			template < class expression_type >
			inline value_type first_or_default(const value_type & default_value, expression_type expression) {
				auto
					it = begin(),
					it_end = end();
			
				for (; it != it_end; ++it) {
					if (expression(*it))
						return *it;
				}
			
				return default_value;
			}
			
			template < class list_type = std::vector < value_type >, class comparer = std::less < value_type > >
			inline list_type distinct() {
				using key_map = std::map < value_type, int, comparer >;
			
				key_map keys;
				list_type ret;
			
				typename key_map::const_iterator
					map_it,
					map_end = keys.end();
			
				for (auto i : *this) {
			
					map_it = keys.find(i);
					if (map_it == map_end) {
						keys[i] = 0;
						ret.push_back(i);
					}
				}
			
				return ret;
			}

		};

	}

	template < class value_type, size_t size >
	inline details::query < 
		details::array_handle < value_type, size >,
		details::null_condition < value_type >,
		details::null_transformer < details::array_handle < value_type, size > >
	> from (value_type (&v)[size]) {
		return {
			details::array_handle < value_type, size > {v},
			details::null_condition < value_type > (),
			details::null_transformer < details::array_handle < value_type, size > > ()
		};
	}

	template < class source_type >
	inline details::query < 
		details::list_handle < source_type >,
		details::null_condition < typename source_type::value_type >,
		details::null_transformer < details::list_handle < source_type > >
	> from (source_type & v) {
		return {
			details::list_handle < source_type > {v},
			details::null_condition < typename source_type::value_type > (),
			details::null_transformer < details::list_handle < source_type > > ()
		};
	}

}

#endif // !_lincxx_h_
