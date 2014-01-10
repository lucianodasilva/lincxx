#ifndef _lincxx_details_and_h_
#define _lincxx_details_and_h_

#include "lincxx.details.h"
#include "lincxx.param.h"

namespace lincxx {
	namespace details {

		struct and_evaluator {

			template < class this_type, class item_type >
			inline static bool evaluate (this_type & this_inst, item_type & item) {
				return
					this_inst.left.evaluate (item)
					&&
					this_inst.right.evaluate (item);
			}

		};


		template < class lhe_type, class rhe_type >
		using and = lincxx::details::binary_oper < lhe_type, rhe_type, and_evaluator >;

		template < class lhe_type, class rhe_type >
		inline auto operator && (const lhe_type & l, const rhe_type & r) 
			->and < lhe_type, rhe_type >
		{
			return and < lhe_type, rhe_type > { l, r };
		}

	}

}

#endif