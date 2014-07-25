#ifndef _lincxx_h_
#define _lincxx_h_

#include <list>
#include <map>
#include <vector>
#include <functional>

namespace lincxx {

	// --------------
	// common details
	// --------------
	namespace details {

		template < class source_type >
		struct source_proxy {

			source_type & source;

			using value_type = typename source_type::value_type;

			using iterator = typename source_type::iterator;

			inline source_proxy(source_type & source_ref) : source(source_ref) {}

			inline iterator begin() const { return source.begin(); }

			inline iterator end() const { return source.end(); }

		};

		template < class source_type >
		struct source_proxy < const source_type > {
		
			const source_type & source;
		
			using value_type = typename source_type::value_type;
		
			using iterator = typename source_type::const_iterator;
		
			inline source_proxy(const source_type & source_ref) : source(source_ref) {}
		
			inline iterator begin() const { return source.cbegin(); }
		
			inline iterator end() const { return source.cend(); }
		
		};

		template < class item_type, size_t array_size >
		struct source_proxy < item_type[array_size] > {

			using value_type = item_type;

			using iterator = item_type*;

			item_type * source;

			inline source_proxy(item_type(&source_ref)[array_size]) : source (+source_ref) {}

			inline iterator begin() const { return +source; }

			inline iterator end() const { return +source + array_size; }

		};

		template < class item_type >
		struct null_expression {
			inline bool operator ()(const item_type & i) const { return true; }
		};

		template < class t >
		struct lambda_ref /*: public lambda_ref < decltype (&t::operator ()) >*/ {};

		template < class c_type, class r_type, class ... p_v_type >
		struct lambda_ref < r_type(c_type::*)(p_v_type...) const > {

			using return_type = r_type;
			using class_type = c_type;

			class_type address;

			inline lambda_ref(const class_type & v) : address(v) {}

			inline return_type operator () (p_v_type ... arg_v) const { return address(arg_v...); }

		};

		template < class t >
		struct add : public std::binary_function < t, t, t > {
			inline t operator () (const t & v1, const t & v2) const {
				return v1 + v2;
			}
		};
	}

	// --------------
	// Type Transform
	// --------------
	namespace details {

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
		struct extract_accessor_class < t, v ... >{
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

		template < class ... accessor_types >
		struct transformer {

			using value_type = std::tuple <
				typename extract_accessor_type < accessor_types >::value_type ...
			>;

			using class_type = typename extract_accessor_class < accessor_types ... >::class_type;

			std::tuple < accessor_types ... > accessor_instances;

			inline transformer(accessor_types ... accessors) : accessor_instances(accessors...) {}

			inline value_type transform(const class_type & src) const {
				value_type dst;
				tuple_for_each < sizeof...(accessor_types), class_type, value_type, accessor_types ... >::copy(src, accessor_instances, dst);
				return dst;
			}

		};

		template < class t >
		struct transformer < t > {

			using value_type = typename extract_accessor_type < t >::value_type;
			using class_type = typename extract_accessor_class < t >::class_type;

			t accessor;

			inline transformer(t v) : accessor(v) {}

			inline value_type transform (const class_type & src) const {
				return accessor.get(src);
			}

		};

		template < class t >
		struct null_transformer {
			using value_type = t;
			inline t transform(const t & v) const { return v; }
		};

	}

	// ------------
	// Query handle
	// ------------
	namespace details {

		template <
			class source_proxy_type,
			class condition_type,
			class transformer_type
		>
		struct query {

			using query_type = query < source_proxy_type, condition_type, transformer_type >;
			using value_type = typename transformer_type::value_type;

			source_proxy_type	_source;
			condition_type		_condition;
			transformer_type	_transformer;

			// define filtering iterator
			template < class source_iterator >
			class iterator_base;

			template < class source_iterator >
			friend class iterator_base;

			class iterator : public std::iterator < std::forward_iterator_tag, value_type > {
			public:

				mutable typename source_proxy_type::iterator
					source_it,
					source_end;

			private:
				
				const query_type query;

				inline void search_first() const {
					while (source_it != source_end && !query._condition(*source_it))
						++source_it;
				}

				inline void search_next() const {
					do
						++source_it;
					while (source_it != source_end && !query._condition(*source_it));
				}

			public:

				inline iterator() {}

				inline iterator(
					const query_type & cquery,
					const typename source_proxy_type::iterator & it,
					const typename source_proxy_type::iterator & it_end
				) : source_it(it), source_end(it_end), query (cquery) {
					search_first();
				}

				inline iterator(const iterator & v) : source_it(v.source_it), source_end(v.source_end), query (v.query) {}

				inline iterator & operator ++ () {
					search_next();
					return *this;
				}

				inline bool operator == (const iterator & v) const { return source_it == v.source_it; }
				inline bool operator != (const iterator & v) const { return source_it != v.source_it; }

				inline value_type operator * (){ return query._transformer.transform (*source_it); }

			};
			// ---------------------------------------------

			using const_iterator = iterator;

			inline iterator begin() const {
				return iterator (
					*this,
					_source.begin(),
					_source.end()
				);
			}

			inline iterator end() const {
				return iterator(
					*this,
					_source.end(),
					_source.end()
				);
			}

			inline const_iterator cbegin() const {
				return begin();
			}

			inline const_iterator cend() const {
				return end();
			}

			template < class exp_type >
			inline query <
				source_proxy_type,
				lambda_ref < decltype (&exp_type::operator ()) >,
				transformer_type
			> where(exp_type expression) const
			{
				return{
					_source,
					lambda_ref < decltype (&exp_type::operator ()) > { expression },
					_transformer
				};
			}

			template < class ... address_types >
			inline query <
				source_proxy_type,
				condition_type,
				transformer < accessor < address_types > ... >
			> select ( address_types ... accessors) const {
				return{
					_source,
					_condition,
					transformer < accessor < address_types > ... >(accessors...)
				};
			}

			template < class list_type = std::vector < value_type > >
			inline list_type to_list() const {
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

			inline size_t size() const {
				size_t s = 0;

				auto
					it = begin(),
					it_end = end();

				for (; it != it_end; ++it)
					++s;

				return s;
			}

			template < class addition = details::add < value_type > > 
			typename addition::result_type sum() const {
				typename addition::result_type res = typename addition::result_type();
				addition adder;

				for (auto v : *this)
					res = adder(res, v);

				return res;
			}

			inline value_type first_or_default(const value_type & default_value) const {
				auto it = begin();

				if (it != end())
					return *it;

				return default_value;
			}

			template < class expression_type >
			inline value_type first_or_default(const value_type & default_value, expression_type expression) const {
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
			inline list_type distinct() const {
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

	template < class source_type >
	inline details::query <
		details::source_proxy < source_type >,
		details::null_expression < typename details::source_proxy < source_type >::value_type >,
		details::null_transformer < typename details::source_proxy < source_type >::value_type >
	>
	from(source_type & source)
	{
		return{
			details::source_proxy < source_type > ( source ), // list source
			details::null_expression <
				typename details::source_proxy < source_type >::value_type
			>(),
			details::null_transformer < 
				typename details::source_proxy < source_type >::value_type
			>()
		};
	}

	template < class source_type >
	inline const details::query <
		details::source_proxy < const source_type >,
		details::null_expression < typename details::source_proxy < const source_type >::value_type >,
		details::null_transformer < typename details::source_proxy < const source_type >::value_type >
	>
	from(const source_type & source)
	{
		return{
			details::source_proxy < const source_type >(source), // list source
			details::null_expression <
				typename details::source_proxy < const source_type >::value_type
			>(),
			details::null_transformer <
				typename details::source_proxy < const source_type >::value_type
			>()
		};
	}

}

#endif // !_lincxx_h_
