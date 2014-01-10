#ifndef	_lincxx_param_h_
#define _lincxx_param_h_

namespace lincxx {
	namespace details {

		struct item_param {

			template < class item_type >
			inline item_type get_value (item_type & item) const {
				return item;
			}

		};

		template < class val_type >
		struct literal_param {
			val_type val;

			template < class item_type >
			inline val_type get_value (item_type & item) const {
				return val;
			}
		};

		template < class struct_type, class field_type > 
		struct field_param {

			using address_type = field_type (struct_type::*);

			address_type field_address;

			inline field_type get_value (struct_type & item) const {
				return (item.*field_address);
			}

		};

		template < class struct_type, class method_type >
		struct method_param {

			using address_type = method_type (struct_type::*)();

			address_type method_address;

			inline method_type get_value (struct_type & item) const {
				return (item.*method_address) ();
			}

		};

	}

	details::item_param item;

	template < class val_type >
	inline details::literal_param < val_type > param (const val_type & v) {
		return details::literal_param < val_type > { v };
	}

	template < class struct_type, class field_type >
	inline details::field_param < struct_type, field_type > param (field_type (struct_type::*field_address)) {
		return details::field_param < struct_type, field_type > { field_address };
	}

	template < class struct_type, class method_type >
	inline details::method_param < struct_type, method_type > param (method_type (struct_type::*method_address) ()) {
		return details::method_param < struct_type, method_type > { method_address };
	}

	template < class value_t >
	inline details::literal_param < value_t > param (const details::literal_param < value_t > & v) { return v; }

	template < class struct_type, class field_type >
	inline details::field_param < struct_type, field_type > param (const details::field_param < struct_type , field_type > & v) { return v; }

	template < class struct_type, class method_type >
	inline details::method_param < struct_type, method_type > param (const details::method_param < struct_type, method_type > & v) { return v; }

	inline details::item_param param (const details::item_param & v) { return v; }
}

#endif