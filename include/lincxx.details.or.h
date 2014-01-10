#ifndef _lincxx_details_or_h_
#define _lincxx_details_or_h_

#include "lincxx.details.h"
#include "lincxx.param.h"

namespace lincxx {
	namespace details {

		struct or_evaluator {

			template < class this_type, class item_type >
			inline static bool evaluate (this_type & this_inst, item_type & item) {
				return
					this_inst.left.evaluate (item)
					||
					this_inst.right.evaluate (item);
			}

		};


		template < class lhe_type, class rhe_type >
		using or = binary_oper < lhe_type, rhe_type, or_evaluator >;

		template < class lhe_type, class rhe_type >
		inline auto operator || (const lhe_type & l, const rhe_type & r) 
			->or < lhe_type, rhe_type >
		{
			return or < lhe_type, rhe_type > { l, r };
		}

	}

}

#endif