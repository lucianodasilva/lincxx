#ifndef _lincxx_h_
#define _lincxx_h_

#include <list>
#include <map>

#include <functional>

namespace lincxx {

	// --------------
	// common details
	// --------------
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
			using const_iterator = typename list_type::const_iterator;

			const list_type & data;

			inline typename list_type::const_iterator begin () const {
				return data.begin ();
			}

			inline typename list_type::const_iterator end () const {
				return data.end ();
			}

		};

		template < class item_type, size_t array_size >
		struct array_handle {

			using value_type = item_type;
			using const_iterator = item_type*;

			item_type * data;

			inline item_type * begin () const { return data; }
			inline item_type * end () const { return data + array_size; }

		};

		struct null_expression {

			static null_expression instance;

			template < class item_type >
			inline bool evaluate (item_type & item) const { return true; }

		};

		null_expression null_expression::instance;

		template < class value_type > 
		struct lambda_expression {

			std::function < bool (value_type &) > exp;

			inline lambda_expression (const lambda_expression < value_type > & v ) : exp (v.exp) {}
			inline lambda_expression (const std::function < bool (value_type &) > & e) : exp (e) {}

			template < class item_type >
			inline bool evaluate (item_type & item) const { return exp (item); }

		};

	}

	// -----------------
	// Parameter Implementation
	// -----------------
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
			static inline type create (field_type (struct_type::*v)) { return{v}; }
		};

		template < class struct_type, class method_type >
		struct param_traits < method_type (struct_type::*) () > {
			using type = method_param < struct_type, method_type >;
			static inline type create (method_type (struct_type::*v) ()) { return{v}; }
		};

		template < class t >
		struct param_traits < literal_param < t > > {
			using type = literal_param < t >;
			static inline type create (const type & v) { return v; }
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

	// ---------
	// Operators
	// ---------
	namespace details {

		struct or_op_eval {

			template < class this_type, class item_type >
			inline static bool evaluate (this_type & this_inst, item_type & item) {
				return
					this_inst.left.evaluate (item)
					||
					this_inst.right.evaluate (item);
			}

		};

		template < class lhe_type, class rhe_type >
		using or_op = binary_oper < lhe_type, rhe_type, or_op_eval >;

		template < class lhe_type, class rhe_type >
		inline or_op < lhe_type, rhe_type > operator || (const lhe_type & l, const rhe_type & r) {
			return{l, r};
		}

		// ------------------------------------------------------------------------------------

		struct and_op_eval {

			template < class this_type, class item_type >
			inline static bool evaluate (this_type & this_inst, item_type & item) {
				return
					this_inst.left.evaluate (item)
					&&
					this_inst.right.evaluate (item);
			}

		};


		template < class lhe_type, class rhe_type >
		using and_op = lincxx::details::binary_oper < lhe_type, rhe_type, and_op_eval >;

		template < class lhe_type, class rhe_type >
		inline and_op < lhe_type, rhe_type > operator && (const lhe_type & l, const rhe_type & r) {
			return{l, r};
		}

		// ------------------------------------------------------------------------------------

		struct eq_op_eval {

			template < class this_type, class item_type >
			inline static bool evaluate (this_type & this_inst, item_type & item) {
				return
					this_inst.left.get_value (item)
					==
					this_inst.right.get_value (item);
			}

		};

		template < class lhe_type, class rhe_type >
		using eq_op = binary_oper < lhe_type, rhe_type, eq_op_eval >;

		template < class lhe_type, class rhe_type >
		inline
			eq_op <
			typename details::param_traits < lhe_type >::type,
			typename details::param_traits < rhe_type >::type
			>
			operator == (const lhe_type & l, const rhe_type & r) {
			return{param (l), param (r)};
		}

		// ------------------------------------------------------------------------------------

		struct gr_op_eval {

			template < class this_type, class item_type >
			inline static bool evaluate (this_type & this_inst, item_type & item) {
				return
					this_inst.left.get_value (item)
					>
					this_inst.right.get_value (item);
			}

		};

		template < class lhe_type, class rhe_type >
		using gr_op = binary_oper < lhe_type, rhe_type, gr_op_eval >;

		template < class lhe_type, class rhe_type >
		inline
			gr_op <
			typename details::param_traits < lhe_type >::type,
			typename details::param_traits < rhe_type >::type
			>
			operator > (const lhe_type & l, const rhe_type & r) {
			return{param (l), param (r)};
		}

		// ------------------------------------------------------------------------------------

		struct ls_op_eval {

			template < class this_type, class item_type >
			inline static bool evaluate (this_type & this_inst, item_type & item) {
				return
					this_inst.left.get_value (item)
					<
					this_inst.right.get_value (item);
			}

		};

		template < class lhe_type, class rhe_type >
		using ls_op = binary_oper < lhe_type, rhe_type, ls_op_eval >;

		template < class lhe_type, class rhe_type >
		inline
			ls_op <
			typename details::param_traits < lhe_type >::type,
			typename details::param_traits < rhe_type >::type
			>
			operator < (const lhe_type & l, const rhe_type & r) {
			return{param (l), param (r)};
		}

	}

	// ------------
	// Query handle
	// ------------
	namespace details {

		template < class source_handle, class exp_type = null_expression >
		struct query_handle {

			using query_handle_type = query_handle < source_handle, exp_type >;

			using value_type = typename source_handle::value_type;

			source_handle	_list;
			exp_type		_exp;

			inline query_handle (const source_handle & src, const exp_type & exp) : _list (src), _exp (exp) {}

			// define filtering iterator
			class const_iterator;
			friend class const_iterator;

			class const_iterator : public std::iterator < std::forward_iterator_tag, value_type > {
			private:
				const query_handle_type & _query;

				inline void search_first () {
					while (source_it != source_end && !_query._exp.evaluate (*source_it))
						++source_it;
				}

				inline void search_next () {
					do
						++source_it;
					while (source_it != source_end && !_query._exp.evaluate (*source_it));
				}

			public:

				typename source_handle::const_iterator
					source_it,
					source_end;

				inline const_iterator () {}

				inline const_iterator (
					const query_handle_type & ref_query,
					const typename source_handle::const_iterator & v_it,
					const typename source_handle::const_iterator & v_end
					) : _query (ref_query), source_it (v_it), source_end (v_end) {
					search_first ();
				}

				inline const_iterator (const const_iterator & v) : _query (v._query), source_it (v.source_it), source_end (v.source_end) {}

				inline const_iterator & operator ++ () {
					search_next ();
					return *this;
				}

				inline bool operator == (const const_iterator & v) { return source_it == v.source_it; }
				inline bool operator != (const const_iterator & v) { return source_it != v.source_it; }

				inline value_type & operator * () { return *source_it; }

			};

			// ---------------------------------------------

			inline const_iterator begin () const { return const_iterator (*this, _list.begin (), _list.end ()); }
			inline const_iterator end () const { return const_iterator (*this, _list.end (), _list.end ()); }

			template <
				class lhe_type,
				class rhe_type,
				class eva_type
			>
			inline query_handle < source_handle, binary_oper < lhe_type, rhe_type, eva_type > > where (const binary_oper < lhe_type, rhe_type, eva_type > & exp) const {
				return query_handle < source_handle, binary_oper < lhe_type, rhe_type, eva_type > > (
					_list,
					exp
				);
			}

			inline query_handle < source_handle, lambda_expression < value_type > > where (const std::function < bool (value_type &) > & exp) const {
				return query_handle < source_handle, lambda_expression < value_type > > (
					_list,
					lambda_expression < value_type > (exp)
				);
			}

			inline std::list < value_type > to_list () const {
				std::list < value_type > result;

				for (auto i : *this)
					result.push_back (i);

				return result;
			}

			template < class callback_type >
			inline void visit (const callback_type & call) const {
				for (auto i : *this)
					call (i);
			}

			// other tools
			inline const size_t count () {
				size_t c = 0;
				
				auto
					it = begin (),
					it_end = end ();
				
				for (; it != it_end; ++it)
					++c;

				return c;
			}

			inline const value_type & first_or_default (const value_type & default_value) {
				auto it = begin ();

				if (it != end ())
					return *it;

				return default_value;
			}

			template < 
				class lhe_type,
				class rhe_type,
				class eva_type
			>
			inline const value_type & first_or_default (const value_type & default_value, const binary_oper < lhe_type, rhe_type, eva_type > & exp) {
				auto
					it = begin (),
					it_end = end ();

				for (; it != it_end; ++it) {
					if (exp.evaluate (*it))
						return *it;
				}

				return default_value;
			}

			template < class compare = std::less < value_type > >
			inline std::list < value_type > distinct () {

				using key_map = std::map < value_type, int, compare > ;
					
				key_map keys;
				std::list < value_type > ret;

				typename key_map::const_iterator
					map_it,
					map_end = keys.end ();

				for (auto i : *this) {

					map_it = keys.find (i);
					if (map_it == map_end) {
						keys [i] = 0;
						ret.push_back (i);
					}
				}

				return ret;
			}

		};

	}

	template < class item_type, size_t array_size >
	inline details::query_handle < details::array_handle < item_type, array_size > > from (item_type (&array_inst) [array_size]) {
		return details::query_handle < details::array_handle < item_type, array_size > > {
			{ &array_inst [0] },
			details::null_expression::instance
		};
	}

	template < class list_type >
	inline details::query_handle < details::list_handle < list_type > > from (const list_type & list) {
		return details::query_handle < details::list_handle < list_type > > {
			{ list },
			details::null_expression::instance
		};
	}

}

#endif // !_lincxx_h_
