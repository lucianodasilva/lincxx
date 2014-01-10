#ifndef _lincxx_details_greater_h_
#define _lincxx_details_greater_h_

#include "lincxx.details.h"
#include "lincxx.param.h"

namespace lincxx {
	namespace details {

		struct greater_evaluator {

			template < class this_type, class item_type >
			inline static bool evaluate (this_type & this_inst, item_type & item) {
				return
					this_inst.left.get_value (item)
					>
					this_inst.right.get_value (item);
			}

		};


		template < class lhe_type, class rhe_type >
		using greater = binary_oper < lhe_type, rhe_type, greater_evaluator >;

		template < class lhe_type, class rhe_type >
		inline auto operator > (const lhe_type & l, const rhe_type & r) 
			->greater < decltype (param (l)), decltype (param (r)) >
		{
			return greater < decltype (param (l)), decltype (param (r)) > { param (l), param (r) };
		}

	}

}

#endif