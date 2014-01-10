#ifndef	_lincxx_query_h_
#define _lincxx_query_h_

#include <array>
#include <functional>
#include <list>

namespace lincxx {

	namespace details {

		template < class source_handle, class exp_type = null_expression, class call_type = null_callback >
		struct query_handle {

			source_handle			__list_ref;
			const exp_type &		__exp_ref;
			const call_type &		__call_ref;

			template < class callback_type >
			inline query_handle < source_handle, exp_type, callback_handle < callback_type > > operator [] (callback_type & call) {
				return query_handle < source_handle, exp_type, callback_handle < callback_type > > {
					__list_ref,
					__exp_ref,
					callback_handle <callback_type> { call }
				};
			}

			template < class filter_exp_type >
			inline query_handle < source_handle, filter_exp_type, call_type > where (const filter_exp_type & exp) const {
				return query_handle < source_handle, filter_exp_type, call_type> {
					__list_ref,
					exp,
					__call_ref
				};
			}

			inline auto select ()
				-> std::list < typename std::remove_reference < decltype (*std::begin (__list_ref)) >::type > {

				std::list <
					typename std::remove_reference <
					decltype (*std::begin (__list_ref)) >::type
				> result;

				for (auto i : __list_ref) {
					if (__exp_ref.evaluate (i)) {
						__call_ref.call (i);
						result.push_back (i);
					}
				}

				return result;

			}

		};

	}

	template < class item_type, size_t array_size >
	inline details::query_handle < details::array_handle < item_type, array_size > > from (item_type (&array_inst) [array_size] ) {
		return details::query_handle < details::array_handle < item_type, array_size > > {
			{ &array_inst[0] }, 
			details::null_expression::instance,
			details::null_callback::instance
		};
	}

	template < class list_type >
	inline details::query_handle < details::list_handle < list_type > > from (const list_type & list) {
		return details::query_handle < details::list_handle < list_type > > {
			{list}, 
			details::null_expression::instance,
			details::null_callback::instance
		};
	}

}

#endif