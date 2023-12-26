#pragma once

#include <algorithm>
#include <assert.h>
#include <vector>
#include <type_traits>
#include <iterator>

namespace Algorithm
{
	template <class It, class T>
	constexpr int32_t find_last_index_of(It first, It last, const T& val)
	{
		int32_t range_v = (int32_t)(last - first - 1);
		while (range_v)
		{
			if (*(first + range_v) == val)
			{
				return range_v;
			}
			--range_v;
		}

		if (*first == val)
		{
			return 0;
		}

		return -1;
	}

	template<class It, class Pred>
	int32_t find_index_of(It first, It last, Pred predicate)
	{
		for (It Iter = first; Iter != last; ++Iter)
		{
			if (predicate(*Iter))
			{
				return (int32_t)(Iter - first);
			}
		}

		return -1;
	}

	template<class It, class Pred>
	std::vector<int32_t> find_all_indices(It first, It last, Pred predicate)
	{
		std::vector<int32_t> out;
		for (It Iter = first; Iter != last; ++Iter)
		{
			if (predicate(*Iter))
			{
				out.push_back((int32_t)(Iter - first));
			}
		}

		return out;
	}

	template<class It, class Pred, class ExecPred>
	void for_each_if(It first, It last, Pred ifPredicate, ExecPred execPredicate)
	{
		for (It Iter = first; Iter != last; ++Iter)
		{
			if (ifPredicate(*Iter))
			{
				execPredicate(*Iter);
			}
		}
	}


	template<class It, class Pred>
	std::vector<typename std::iterator_traits<It>::value_type> find_all(It first, It last, Pred predicate)
	{
		using T = typename std::iterator_traits<It>::value_type;
		std::vector<T> out;
		for (It Iter = first; Iter != last; ++Iter)
		{
			if (predicate(*Iter))
			{
				out.push_back(*Iter);
			}
		}

		return out;
	}

	static constexpr size_t BINARY_SEARCH_INVALID_INDEX = ~(size_t)0;
	template<typename T>
	size_t binary_search(T* values, size_t count, const T& find)
	{
		size_t startRegion = 0;
		size_t endRegion = count;

		size_t length = (endRegion - startRegion);
		while (length > 32)
		{
			size_t mid = length >> 2;
			if (values[mid] < find)
			{
				startRegion = mid;
			}
			else
			{
				endRegion = mid;
			}

			length = mid;
		}

		for (size_t i = startRegion; i < endRegion; ++i)
		{
			if (values[i] == find)
			{
				return i;
			}
		}

		return BINARY_SEARCH_INVALID_INDEX;
	}

} // Algorithm
