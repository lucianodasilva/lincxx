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

		template < class t >
		struct param_traits {
			using type = literal_param < t >;
			static inline type create (const t & v) { return{v}; }
		};

		template < class struct_type, class field_type >
		struct param_traits < field_type (struct_type::*) > {
			using type = field_param < struct_type, field_type >;
			static inline type create (field_type (struct_type::*v)) { return{v};}
		};

		template < class struct_type, class method_type >
		struct param_traits < method_type (struct_type::*) () > {
			using type = method_param < struct_type, method_type >;
			static inline type create (method_type (struct_type::*v) ()) { return{v}; }
		};

		template < class t >
		struct param_traits < literal_param < t > >  {
			using type = literal_param < t >;
			static inline type create (const type & v) {return v;}
		};

		template < class struct_type, class field_type >
		struct param_traits < field_param < struct_type, field_type > > {
			using type = field_param < struct_type, field_type >;
			static inline type create (const type & v) { return v; }
		};

		template < class struct_type, class method_type >
		struct param_traits < method_param < struct_type, method_type > > {
			using type = method_param < struct_type, method_type >;
			static inline type create (const type & v) { return v; }
		};

		template < >
		struct param_traits < item_param > {
			using type = item_param;
			static inline type create (const type & v) { return v; }
		};

	}

	details::item_param item;

	template < class type >
	inline typename details::param_traits < type >::type param (const type & v) {
		return details::param_traits < type >::create (v);
	}
}

#endif