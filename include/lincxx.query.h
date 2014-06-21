#ifndef	_lincxx_query_h_
#define _lincxx_query_h_

#include <array>
#include <functional>
#include <list>

namespace lincxx {

	namespace details {

		template < class source_handle, class exp_type = null_expression >
		struct query_handle {

			using query_handle_type = query_handle < source_handle, exp_type >;

			using value_type = typename source_handle::value_type;

			source_handle			__list_ref;
			const exp_type &		__exp_ref;

			// define filtering iterator
			class iterator;
			friend class iterator;

			struct iterator : public std::iterator < std::forward_iterator_tag, value_type > {
			private:
				const query_handle_type & _query;

				typename source_handle::iterator
					_source_it,
					_source_end;

				inline void search_first () {
					while (_source_it != _source_end && !_query.__exp_ref.evaluate (*_source_it))
						++_source_it;
				}

				inline void search_next () {
					do
						++_source_it;
					while (_source_it != _source_end && !_query.__exp_ref.evaluate (*_source_it));
				}

			public:

				inline iterator () {}

				inline iterator (
					const query_handle_type & ref_query,
					const typename source_handle::iterator & v_it, 
					const typename source_handle::iterator & v_end
				) : _query (ref_query), _source_it (v_it), _source_end (v_end) 
				{
					search_first ();
				}

				inline iterator (const iterator & v) : _query (v._query), _source_it (v._source_it), _source_end (v._source_end) {}

				inline iterator & operator ++ () {
					search_next ();
					return *this;
				}

				inline bool operator == (const iterator & v) { return _source_it == v._source_it; }
				inline bool operator != (const iterator & v) { return _source_it != v._source_it; }

				inline value_type & operator * () { return *_source_it; }

			};

			// ---------------------------------------------

			inline iterator begin () { return iterator (*this, __list_ref.begin (), __list_ref.end ()); }
			inline iterator end () { return iterator (*this, __list_ref.end (), __list_ref.end ()); }

			template < class filter_exp_type >
			inline query_handle < source_handle, filter_exp_type > where (const filter_exp_type & exp) const {
				return query_handle < source_handle, filter_exp_type> {
					__list_ref,
					exp
				};
			}

			inline std::list < value_type > to_list () {
				std::list < value_type > result;

				for (auto i : *this)
					result.push_back (i);

				return result;
			}

			template < class callback_type >
			inline void visit ( callback_type & call ) {
				for (auto i : *this)
					call (i);
			}

		};

	}

	template < class item_type, size_t array_size >
	inline details::query_handle < details::array_handle < item_type, array_size > > from (item_type (&array_inst) [array_size] ) {
		return details::query_handle < details::array_handle < item_type, array_size > > {
			{ &array_inst[0] }, 
			details::null_expression::instance
		};
	}

	template < class list_type >
	inline details::query_handle < details::list_handle < list_type > > from (list_type & list) {
		return details::query_handle < details::list_handle < list_type > > {
			{list},
			details::null_expression::instance
		};
	}

}

#endif