#ifndef _lincxx_details_equals_h_
#define _lincxx_details_equals_h_

#include "lincxx.details.h"
#include "lincxx.param.h"

namespace lincxx {
	namespace details {

		struct equals_evaluator {

			template < class this_type, class item_type >
			inline static bool evaluate (this_type & this_inst, item_type & item) {
				return
					this_inst.left.get_value (item)
					==
					this_inst.right.get_value (item);
			}

		};


		template < class lhe_type, class rhe_type >
		using equals = binary_oper < lhe_type, rhe_type, equals_evaluator >;

		template < class lhe_type, class rhe_type >
		inline auto operator == (const lhe_type & l, const rhe_type & r)
			->equals < decltype (param (l)), decltype (param (r)) > {
			return equals < decltype (param (l)), decltype (param (r)) > { param (l), param (r) };
		}

	}

}

#endif