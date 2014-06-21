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

			using value_type = typename list_type::value_type;
			using iterator = typename list_type::iterator;

			list_type & data;

			inline typename list_type::iterator begin () {
				return data.begin ();
			}

			inline typename list_type::iterator end () { 
				return data.end ();
			}

		};

		template < class item_type, size_t array_size >
		struct array_handle {

			using value_type = item_type;
			using iterator = item_type*;

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

	}
}

#endif