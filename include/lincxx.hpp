#ifndef _lincxx_h_
#define _lincxx_h_

#include <list>
#include <map>
#include <vector>
#include <functional>

namespace lincxx {

	namespace details {

		namespace extra {

			template < class _t >
			struct add : public std::binary_function < _t, _t, _t > {

				inline _t operator () (const _t & v1, const _t & v2) const {
					return v1 + v2;
				}
			};

		}

		template < class _base_t, class _base_value_t >
		struct common_methods {

			inline const _base_t & base_this () const {
				return *reinterpret_cast <const _base_t *> (this);
			}

			template < class _list_t = std::vector < _base_value_t > >
			inline _list_t to_list () const {
				_list_t dest;

				for (auto v : base_this ())
					dest.push_back (v);

				return dest;
			}

			template < class _callback_t >
			inline void visit (const _callback_t & call) const {

				for (auto i : base_this ())
					call (i);
			}

			inline size_t size () const {

				size_t s = 0;

				auto
					it = base_this ().begin (),
					it_end = base_this ().end ();

				for (; it != it_end; ++it)
					++s;

				return s;
			}

			inline _base_value_t first_or_default (const _base_value_t & default_value) const {
				auto it = base_this ().begin ();

				if (it != base_this ().end ())
					return *it;

				return default_value;
			}

			template < class _exp_t >
			inline _base_value_t first_or_default (const _base_value_t & default_value, _exp_t expression) const {
				auto
					it = base_this ().begin (),
					it_end = base_this ().end ();

				for (; it != it_end; ++it) {
					if (expression (*it))
						return *it;
				}

				return default_value;
			}

			template < 
				class _addition_t = extra::add < _base_value_t >
			>
			typename _addition_t::result_type sum () const {
				typename _addition_t::result_type res = typename _addition_t::result_type ();
				_addition_t adder;

				for (auto v : base_this ())
					res = adder (res, v);

				return res;
			}

			template < 
				class _list_t = std::vector < _base_value_t >,
				class _comp_t = std::less < _base_value_t >
			>
			inline _list_t distinct () const {
				using key_map = std::map < _base_value_t, int, _comp_t >;

				key_map keys;
				_list_t ret;

				typename key_map::const_iterator
					map_it,
					map_end = keys.end ();

				for (auto i : base_this ()) {

					map_it = keys.find (i);
					if (map_it == map_end) {
						keys [i] = 0;
						ret.push_back (i);
					}
				}

				return ret;
			}

		};
		
		// -------------------------------------------------------------------------------------
		// data source abstraction
		// -------------------------------------------------------------------------------------
		template <
			class _t,
			class _it_t = _t *,
			class _const_it_t = const _t *
		>
		struct iterable_base_types {
			using value_type = _t;
			using reference = _t &;
			using const_reference = const _t &;

			using iterator = _it_t;
			using const_iterator = _const_it_t;
		};

		template <
			class _src_t,
			class _t = typename _src_t::value_type,
			class _it_t = typename _src_t::iterator,
			class _const_it_t = typename _src_t::const_iterator
		>
		struct data_handle :
			public iterable_base_types <
				_t,
				_it_t,
				_const_it_t
			> 
		{

			_src_t & data;

			inline data_handle (_src_t & v) : data (v) {}

			inline iterator begin () { return std::begin (data); }
			inline iterator end () { return std::end (data); }

			inline const_iterator begin () const {
				const _src_t & c_data = data;
				return std::begin (c_data);
			}

			inline const_iterator end () const {
				const _src_t & c_data = data;
				return std::end (c_data);
			}

		};

		template <
			class _src_t,
			class _t,
			class _it_t,
			class _const_it_t
		>
		struct data_handle < const _src_t, _t, _it_t, _const_it_t > :
			public iterable_base_types <
				_t,
				_const_it_t,
				_const_it_t
			>
		{

			const _src_t & data;

			inline data_handle (const _src_t & v) : data (v) {}

			inline const_iterator begin () const {
				return std::begin (data);
			}

			inline const_iterator end () const {
				return std::end (data);
			}

		};

		template <
			class _t,
				size_t _size,
			class data_handle_t = data_handle < _t (&) [_size], _t, _t *, const _t * >
		>
		struct array_handle : public data_handle_t {
			inline array_handle (_t (&v) [_size]) : data_handle_t (v) {}
		};

		template < class _base_t, class _it_t >
		struct proxy_iterator_base {

			_it_t source_it;

			inline proxy_iterator_base (_it_t it) : source_it (it) {}

			inline bool operator == (const _base_t & v) const { return source_it == v.source_it; }
			inline bool operator != (const _base_t & v) const { return source_it != v.source_it; }

			inline auto operator * () -> decltype (*source_it) {
				return *source_it;
			}

			inline const _base_t & operator ++ () {
				++source_it;
				return *reinterpret_cast < const _base_t * > (this);
			}

		};

		// -------------------------------------------------------------------------------------
		// Conditions and Lambda References
		// -------------------------------------------------------------------------------------
		template < class _item_t >
		struct null_condition {
			inline bool operator ()(const _item_t & i) const { return true; }
		};

		template < class _t >
		struct lambda /*: public lambda_ref < decltype (&t::operator ()) >*/ {};

		template < class _class_t, class _ret_t, class ... _param_tv >
		struct lambda < _ret_t (_class_t::*)(_param_tv...) const > {

			using return_type = _ret_t;
			using class_type = _class_t;

			class_type address;

			inline return_type operator () (_param_tv ... arg_v) const { return address (arg_v...); }

		};
		// -------------------------------------------------------------------------------------
		// Type Transformation
		// -------------------------------------------------------------------------------------
		template < class _member_t >
		struct accessor {};
		
		template < class _class_t, class _value_t >
		struct accessor < _value_t _class_t::* > {
		
			using class_type = _class_t;
			using value_type = _value_t;
		
			_value_t class_type::* address;
		
			inline accessor(_value_t _class_t::*v) : address(v) {}
		
			inline _value_t get(const _class_t & ref) const {
				return ref.*address;
			}
		
		};
		
		template < class _class_t, class _value_t >
		struct accessor < _value_t(_class_t::*)() > {
		
			using class_type = _class_t;
			using value_type = _value_t;
		
			_value_t(_class_t::*address)();
		
			inline accessor(_value_t(_class_t::*v)()) : address(v) {}
		
			inline _value_t get(const _class_t & ref) const {
				return ref.*address();
			}
		
		};
		
		template < class _t >
		struct extract_accessor_type {
			using value_type = typename _t::value_type;
		};
		
		template < class ... _tv >
		struct extract_accessor_class {};
		
		template < class _t, class ... _v >
		struct extract_accessor_class < _t, _v ... > {
			using class_type = typename _t::class_type;
		};
		
		template < size_t i, class _class_t, class _value_t, class ... _accessor_tv >
		struct tuple_for_each {
			inline static void copy (const _class_t & src, const std::tuple < _accessor_tv... > & accessors, _value_t & dst) {
				std::get < i - 1 >(dst) =
					std::get < i - 1 >(accessors).get(src);
				tuple_for_each  < i - 1, _class_t, _value_t, _accessor_tv ... >::copy (src, accessors, dst);
			}
		};
		
		template <class _class_t, class _value_t, class ... _accessor_tv >
		struct tuple_for_each < 0, _class_t, _value_t, _accessor_tv ... > {
			inline static void copy(const _class_t & src, const std::tuple < _accessor_tv... > & accessors, _value_t & dst) {}
		};

		template < class _src_t, class _it_t, class ... _accessor_tv >
		struct select_iterator :
			public proxy_iterator_base < 
				select_iterator < _src_t, _it_t, _accessor_tv ... >,
				_it_t
			>
		{

			using value_type = std::tuple <
				typename extract_accessor_type < _accessor_tv >::value_type ...
			>;

			using class_type = typename extract_accessor_class < _accessor_tv ... >::class_type;

			std::tuple < _accessor_tv ... > accessor_instances;

			inline select_iterator (std::tuple < _accessor_tv ... > accessors, _it_t it) : 
				proxy_iterator_base < 
					select_iterator < _src_t, _it_t, _accessor_tv ... >, 
					_it_t 
				> (it),
				accessor_instances (accessors)
			{}

			inline value_type operator * () {
				value_type dst;
				tuple_for_each < sizeof...(_accessor_tv), class_type, value_type, _accessor_tv ... >::copy (*source_it, accessor_instances, dst);
				return dst;
			}

		};
		
		template < class _src_t, class _it_t, class et0 >
		struct select_iterator < _src_t, _it_t, et0 > :
			public proxy_iterator_base < 
				select_iterator < _src_t, _it_t, et0 >,
				_it_t
			>
		{

			using value_type = typename extract_accessor_type < et0 >::value_type;
			using this_type = select_iterator < _src_t, _it_t, et0 >;

			et0 accessor;

			inline select_iterator ( et0 v, _it_t it) : 
				proxy_iterator_base < 
					select_iterator < _src_t, _it_t, et0 >,
					_it_t
				> (it),
				accessor (v)
			{}

			inline value_type operator * () {
				return accessor.get (*source_it);
			}

		};

		template < class _src_t, class ... _accessor_tv >
		struct selector : 
			public iterable_base_types <
				std::tuple <
					typename extract_accessor_type < _accessor_tv >::value_type ...
				>,
				select_iterator < _src_t, typename _src_t::const_iterator, _accessor_tv... >,
				select_iterator < _src_t, typename _src_t::const_iterator, _accessor_tv... >
			>,
			public common_methods < 
				selector < _src_t, _accessor_tv... >, 
				std::tuple <
					typename extract_accessor_type < _accessor_tv >::value_type ...
				>
			> 
		{
			_src_t data;
			std::tuple < _accessor_tv ... > accessor_instances;

			inline selector (_src_t data_ref, _accessor_tv ... accessors) : data (data_ref), accessor_instances (accessors...) {}

			inline const_iterator begin () const { return{accessor_instances, data.begin ()}; }
			inline const_iterator end () const { return {accessor_instances, data.end ()}; }

		};

		template < class _src_t, class et0 >
		struct selector < _src_t, et0 > : 
			public common_methods < 
				selector < _src_t, et0 >,
				typename extract_accessor_type < et0 >::value_type
			>
		{

			using value_type = typename extract_accessor_type < et0 >::value_type;

			using const_iterator = typename select_iterator < _src_t, typename _src_t::const_iterator, et0 >;
			using iterator = const_iterator;

			_src_t data;
			et0 accessor;

			inline selector (_src_t data_ref, et0 v) : data (data_ref), accessor (v) {}

			inline const_iterator begin () const { return{accessor, data.begin ()}; }
			inline const_iterator end () const { return{accessor, data.end ()}; }

		};

		// ----------------------

		template < 
			class _src_t,
			class _it_t,
			class _cond_t
		>
		struct iterator :
			public proxy_iterator_base <
				iterator < _src_t, _it_t, _cond_t >,
				_it_t
			>
		{

			_cond_t condition;

			_it_t
				source_end;

			inline iterator ( _cond_t cond, _it_t it, _it_t end ) : 
				proxy_iterator_base < iterator < _src_t, _it_t, _cond_t >, _it_t > (it),
				condition (cond),
				source_end (end)
			{
				search_first ();
			}

			inline const iterator < _src_t, _it_t, _cond_t > & operator ++ () {
				search_next ();
				return *this;
			}
												    
			inline void search_first () {
				while (source_it != source_end && !condition (*source_it))
					++source_it;
			}

			inline void search_next () {
				do
					++source_it;
				while (source_it != source_end && !condition (*source_it));
			}

		};

		template < 
			class _src_t,
			class _cond_t
		>
		struct query : 
			public iterable_base_types <
				typename _src_t::value_type,
				details::iterator < _src_t, typename _src_t::iterator, _cond_t >,
				details::iterator < _src_t, typename _src_t::const_iterator, _cond_t>
			>,
			public common_methods < 
				query < _src_t, _cond_t >,
				typename _src_t::value_type
			> 
		{

			_src_t source;

			_cond_t			
				condition;

			inline query (_src_t v, _cond_t cond) : source (v), condition (cond) {}

			inline iterator begin () {
				return iterator (
					condition,
					source.begin (),
					source.end ()
				);
			}

			inline iterator end () {
				return iterator (
					condition,
					source.end (),
					source.end ()
				);
			}

			inline const_iterator begin () const {
				const _src_t & const_source = source;

				return const_iterator (
					condition,
					const_source.begin (),
					const_source.end ()
				);
			}

			inline const_iterator end () const {
				const _src_t & const_source = source;

				return const_iterator (
					condition,
					const_source.end (),
					const_source.end ()
				);
			}

			template < class condition_exp_type >
			inline query < 
				_src_t, 
				lambda < decltype (&condition_exp_type::operator ()) >
			>
			where (condition_exp_type condition_ref) const 
			{
				return {
					source,
					lambda < decltype (&condition_exp_type::operator ()) > {condition_ref}
				};
			}

			template <
				class ... _address_tv
			>
			inline selector <
				data_handle < 
					const query < _src_t, _cond_t >,
					value_type,
					iterator,
					const_iterator
				>,
				accessor < _address_tv > ...
			>
			select (_address_tv ... accessors)
			{
				return
					selector <
						data_handle  < const query < _src_t, _cond_t > >,
						accessor < _address_tv > ...
					> (
						data_handle  < const query < _src_t, _cond_t > > (*this),
						accessors ...
					);
			}


		};

	}

	template < 
		class _t, 
		size_t _size, 
		class query_t = details::query < details::array_handle < _t, _size >, details::null_condition < _t > >
	>
	inline query_t from (_t (&v)[_size]) {
		return query_t (
			details::array_handle < _t, _size > {v},
			details::null_condition < _t > ()
		);
	}

	template < 
		class _src_t, 
		class query_t = details::query < details::data_handle < _src_t >, details::null_condition < typename _src_t::value_type > >
	>
	inline query_t from (_src_t & v) {
			return query_t (
			details::data_handle < _src_t > (v),
			details::null_condition < typename _src_t::value_type > ()
		);
	}

}

#endif // !_lincxx_h_
