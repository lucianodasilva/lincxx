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
	}

	// ------------
	// Query handle
	// ------------
	namespace details {

		template <
			class source_proxy_type,
			class condition_type
		>
		struct query {

			using query_type = query < source_proxy_type, condition_type >;
			using value_type = typename source_proxy_type::value_type;

			source_proxy_type	_source;
			condition_type		_condition;

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
				
				const condition_type & condition;

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

				inline iterator() {}

				inline iterator(
					const condition_type & cexp,
					const typename source_proxy_type::iterator & it,
					const typename source_proxy_type::iterator & it_end
				) : source_it(it), source_end(it_end), condition (cexp) {
					search_first();
				}

				inline iterator(const iterator & v) : source_it(v.source_it), source_end(v.source_end), condition(v.condition) {}

				inline iterator & operator ++ () {
					search_next();
					return *this;
				}

				inline bool operator == (const iterator & v) const { return source_it == v.source_it; }
				inline bool operator != (const iterator & v) const { return source_it != v.source_it; }

				inline auto operator * () -> decltype (*source_it) { return *source_it; }

			};

			// ---------------------------------------------

			inline iterator begin() const {
				return iterator (
					_condition,
					_source.begin(),
					_source.end()
				);
			}

			inline iterator end() const {
				return iterator(
					_condition,
					_source.end(),
					_source.end()
				);
			}

			inline iterator cbegin() const {
				return begin();
			}

			inline iterator cend() const {
				return end();
			}

			template < class exp_type >
			inline query <
				source_proxy_type,
				lambda_ref < decltype (&exp_type::operator ()) >
			> where(exp_type expression)
			{
				return{
					_source,
					lambda_ref < decltype (&exp_type::operator ()) > { expression }
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

			inline const size_t size() {
				size_t s = 0;

				for (auto v : *this)
					++s;

				return s;
			}

			inline const value_type & first_or_default(const value_type & default_value) const {
				auto it = begin();

				if (it != end())
					return *it;

				return default_value;
			}

			template < class expression_type >
			inline const value_type & first_or_default(const value_type & default_value, expression_type expression) const {
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

	template < class source_type >
	details::query <
		details::source_proxy < source_type >,
		details::null_expression < typename details::source_proxy < source_type >::value_type >
	>
	from(source_type & source)
	{
		return{
			details::source_proxy < source_type > ( source ), // list source
			details::null_expression <
			typename details::source_proxy < source_type >::value_type
			>()
		};
	}

}

#endif // !_lincxx_h_
