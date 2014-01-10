#ifndef _lincxx_common_h_
#define _lincxx_common_h_

namespace lincxx {
	namespace details {
		
		template <
			class lhe_type,
			class rhe_type,
			class eva_type
		>
		struct binary_oper {

			using this_type = binary_oper < lhe_type, rhe_type, eva_type >;

			lhe_type left;
			rhe_type right;

			template < class item_type >
			inline bool evaluate (item_type & t) const { return eva_type::evaluate (*this, t); }

		};

		template < class list_type >
		struct list_handle {

			const list_type & data;

			inline auto begin () -> decltype (data.begin ()) {
				return data.begin ();
			}

			inline auto end () -> decltype (data.end ()) { 
				return data.end ();
			}

		};

		template < class item_type, size_t array_size >
		struct array_handle {

			item_type * data;

			inline item_type * begin () const { return data; }
			inline item_type * end () const { return data + array_size; }

		};

		struct null_expression {

			static null_expression instance;

			template < class item_type >
			inline bool evaluate (item_type & item) { return true; }
		};

		null_expression null_expression::instance;

		struct null_callback {

			static null_callback instance;

			template < class item_type >
			inline void call (const item_type & item) const {}
		};

		null_callback null_callback::instance;

		template < class callback_type >
		struct callback_handle {

			callback_type address;

			template < class item_type >
			inline void call (item_type & item) const { address (item); }

		};

	}
}

#endif