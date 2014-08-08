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
		template < class container_t >
		struct source_value_type {
			using type =
				typename std::remove_reference < container_t >::type::value_type;
		};

		template < class item_type, size_t size >
		struct source_value_type < item_type(&)[size] > {
			using type = item_type;
		};

		template < class container_t >
		struct source_iterator_type {
			using type =
				typename std::remove_reference < container_t >::type::iterator;
		};

		template < class item_type, size_t size >
		struct source_iterator_type < item_type(&)[size] > {
			using type = item_type *;
		};

		template < class container_t >
		struct source_const_iterator_type {
			using type =
				typename std::remove_reference < container_t >::type::const_iterator;
		};

		template < class item_type, size_t size >
		struct source_const_iterator_type < item_type(&)[size] > {
			using type = const item_type *;
		};

		template < class item_type, size_t size >
		struct source_const_iterator_type < const item_type(&)[size] > {
			using type = const item_type *;
		};

		template < class container_t, class value_type = typename source_value_type < container_t >::type >
		struct source_is_const {
			static const bool value = false;
		};

		template < class container_t, class value_type >
		struct source_is_const < container_t, const value_type > {
			static const bool value = true;
		};

		template < class container_t, bool is_const = source_is_const < container_t >::value >
		struct source {

			using this_type = source < container_t, is_const >;

			using value_type = typename source_value_type < container_t >::type;
			using iterator = typename source_iterator_type < container_t >::type;
			using const_iterator = typename source_const_iterator_type < container_t >::type;

			container_t data;

			inline source(const container_t & v) : data(v) {}
			inline source(this_type && origin) : data(std::move(origin.data)) {}

			inline iterator begin() { return std::begin(data); }
			inline iterator end() { return std::end(data); }

			inline const_iterator begin() const { return std::begin(data); }
			inline const_iterator end() const { return std::end(data); }

		};

		template < class container_t >
		struct source < container_t, true > {

			using this_type = source < container_t, true >;

			using value_type = typename source_value_type < container_t >::type;
			using iterator = typename source_iterator_type < container_t >::type;
			using const_iterator = typename source_const_iterator_type < container_t >::type;

			container_t data;

			inline source(const container_t & v) : data(v) {}
			inline source(this_type && origin) : data(std::move(origin.data)) {}

			inline const_iterator begin() const { return std::begin(data); }
			inline const_iterator end() const { return std::end(data); }

		};
		// -------------------------------------------------------------------------------------

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

		template < class ... accessor_types >
		struct transformer {

			using value_type = std::tuple <
				typename extract_accessor_type < accessor_types >::value_type ...
			>;

			using iterator_value_type = value_type;

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
			using iterator_value_type = value_type;
			using class_type = typename extract_accessor_class < t >::class_type;

			t accessor;

			inline transformer(t v) : accessor(v) {}

			inline value_type transform(const class_type & src) const {
				return accessor.get(src);
			}

		};

		template < class t, bool is_const >
		struct null_transformer {
			using value_type = t;
			using iterator_value_type = t &;
			inline iterator_value_type transform( iterator_value_type v) const { return v; }
		};

		template < class t >
		struct null_transformer < t, true > {
			using value_type = const t;
			using iterator_value_type = const t &;
			inline iterator_value_type transform(iterator_value_type v) const { return v; }
		};
		// -------------------------------------------------------------------------------------

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
		struct lambda < r_type(c_type::*)(p_v_type...) const > {

			using return_type = r_type;
			using class_type = c_type;

			class_type address;

			inline lambda(const class_type & v) : address(v) {}

			inline return_type operator () (p_v_type ... arg_v) const { return address(arg_v...); }

		};
		// -------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------
		// Query Traits
		// -------------------------------------------------------------------------------------
		template <
			class source_t,
			class condition_t = null_condition < typename source_t::value_type>,
			class transformer_t = null_transformer < typename source_t::value_type, source_is_const < source_t >::value >
		>
		struct query_traits {

			static const bool is_const = source_is_const < source_t >::value;

			using value_type = typename source_t::value_type;
			using source_type = source_t;
			using condition_type = condition_t;
			using transformer_type = transformer_t;

		};

		template <
			class traits_t,
			class condition_t
		>
		using where_query_traits = query_traits < typename traits_t::source_type, condition_t, typename traits_t::transformer_type >;

			template <
				class traits_t,
				class transformer_t
			>
			using select_query_traits = query_traits < typename traits_t::source_type, typename traits_t::condition_type, transformer_t >;
				// -------------------------------------------------------------------------------------

				// -------------------------------------------------------------------------------------
				// Filtering Iterator
				// -------------------------------------------------------------------------------------
				template <
					class query_traits_t,
					class source_iterator
				>
				class query_iterator_base : public std::iterator < std::forward_iterator_tag, typename query_traits_t::source_type::value_type > {
				private:

					typename query_traits_t::condition_type		condition;
					typename query_traits_t::transformer_type	transform;

					mutable source_iterator
						source_it,
						source_end;

					inline void search_first() const {
						while (source_it != source_end && !condition(*source_it))
							++source_it;
					}

					inline void search_next() const {
						do
							++source_it;
						while (source_it != source_end && !condition(*source_it));
					}

				public:

					using this_type = query_iterator_base < query_traits_t, source_iterator >;

					inline query_iterator_base() {}

					inline query_iterator_base(
						const typename query_traits_t::condition_type & cond,
						const typename query_traits_t::transformer_type & trans,
						const source_iterator & it,
						const source_iterator & end
						) : condition(cond), transform(trans), source_it(it), source_end(end) {
						search_first();
					}

					inline query_iterator_base(const this_type & v) :
						condition(v.condition),
						transform(v.transform),
						source_it(v.source_it),
						source_end(v.source_end)
					{}

					inline this_type & operator ++ () {
						search_next();
						return *this;
					}

					inline bool operator == (const this_type & v) const { return source_it == v.source_it; }
					inline bool operator != (const this_type & v) const { return source_it != v.source_it; }

					inline typename query_traits_t::transformer_type::iterator_value_type operator * () { return transform.transform(*source_it); }

				};
				// -------------------------------------------------------------------------------------

				// -------------------------------------------------------------------------------------
				// Query instance
				// -------------------------------------------------------------------------------------
				template < class traits_t >
				struct query {
				public:

					using this_type = query < traits_t >;

					template < class query_traits_t >
					using query_type = typename std::conditional <
						traits_t::is_const,
						const query < query_traits_t >,
						query < query_traits_t >
					> ::type;

					using value_type = typename traits_t::transformer_type::value_type;

					using iterator = query_iterator_base < traits_t, typename traits_t::source_type::iterator > ;
					using const_iterator = query_iterator_base < traits_t, typename traits_t::source_type::const_iterator >;

					typename traits_t::source_type
						in_source;

					typename traits_t::condition_type
						in_condition;

					typename traits_t::transformer_type
						in_transformer;

					//inline iterator begin() {
					//	return iterator(in_condition, in_transformer, in_source.begin(), in_source.end());
					//}
					//
					//inline iterator end() {
					//	return iterator(in_condition, in_transformer, in_source.end(), in_source.end());
					//}

					inline const_iterator begin() const {
						return const_iterator(in_condition, in_transformer, in_source.begin(), in_source.end());
					}

					inline const_iterator end() const {
						return const_iterator(in_condition, in_transformer, in_source.end(), in_source.end());
					}

					template < class condition_exp >
					inline query_type < where_query_traits < traits_t, lambda < decltype (&condition_exp::operator ()) > > >
						where(condition_exp condition) const {
							return{
								std::move (in_source),
								lambda < decltype (&condition_exp::operator ()) >(condition),
								in_transformer
							};
						}

					template < class ... address_types >
					inline query_type < select_query_traits < traits_t, transformer < accessor < address_types > ... > > >
						select(address_types ... accessors) const {
						return{
							in_source,
							in_condition,
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

					template < class addition = tools::add < value_type > >
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

	template < class T, class source_type = details::source < T > >
	inline details::query < details::query_traits < source_type > > from(T && v) {
		return{
			source_type(v),
			details::null_condition <
			typename details::source_value_type < source_type >::type
			>(),
			details::null_transformer <
				typename details::source_value_type < source_type >::type,
				details::query_traits < source_type >::is_const 
			>()
		};
	}
}
#endif // !_lincxx_h_
