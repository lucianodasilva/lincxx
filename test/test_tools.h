#pragma once
#ifndef _test_tools_h_
#define _test_tools_h_

#include <gtest/gtest.h>
#include <lincxx.hpp>
#include <vector>

namespace lincxx_test {

	template < class source_type >
	inline size_t source_size (source_type & source) {
		return source.size ();
	}

	template < class item_type, size_t size >
	inline size_t source_size (item_type (&source) [size]) {
		return size;
	}

	template < class source_type, class expression >
	inline std::vector < typename source_type::value_type > prepare_expected_result (source_type & source, expression & condition) {
		std::vector < typename source_type::value_type > list;

		for (auto v : source) {
			if (condition (v))
				list.push_back (v);
		}

		return list;
	}

	template < class item_type, size_t size, class expression >
	inline std::vector < item_type > prepare_expected_result (item_type (&source) [size], expression & condition) {
		std::vector < item_type > list;

		for (auto v : source) {
			if (condition (v))
				list.push_back (v);
		}

		return list;
	}

}

#endif