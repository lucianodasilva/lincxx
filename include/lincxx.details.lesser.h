#ifndef _lincxx_details_lesser_h_
#define _lincxx_details_lesser_h_

#include "lincxx.details.h"
#include "lincxx.param.h"

namespace lincxx {
	namespace details {

		struct lesser_evaluator {

			template < class this_type, class item_type >
			inline static bool evaluate (this_type & this_inst, item_type & item) {
				return
					this_inst.left.get_value (item)
					<
					this_inst.right.get_value (item);
			}

		};


		template < class lhe_type, class rhe_type >
		using lesser = binary_oper < lhe_type, rhe_type, lesser_evaluator >;

		template < class lhe_type, class rhe_type >
		inline 
			lesser <
				typename details::param_traits < lhe_type >::type,
				typename details::param_traits < rhe_type >::type
			>
		operator < (const lhe_type & l, const rhe_type & r) 
		{
			return { param (l), param (r) };
		}

	}

}

#endif