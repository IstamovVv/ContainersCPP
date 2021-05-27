#pragma once
#include <cassert>
#include <iostream>
#include <vector>

namespace ist
{
	template<class Iter, class Cmp = std::less_equal<>>
	void merge(Iter first_start, Iter first_end, Iter second_start, Iter second_end, Cmp cmp = std::less_equal<>())
	{
		assert(first_start <= first_end && "transposed iterator range");
		assert(second_start <= second_end && "transposed iterator range");
		assert(first_start <= second_end && "transposed iterator range");

		using value_type = typename Iter::value_type;

		std::vector<value_type> tmp(second_end - first_start);

		size_t index = 0;
		Iter start = first_start;
		Iter end = second_end;

		while (first_start != first_end && second_start != second_end)
		{
			if (cmp(*first_start, *second_start))
			{
				tmp[index] = *first_start;
				++first_start;
				++index;
			}
			else
			{
				tmp[index] = *second_start;
				++second_start;
				++index;
			}
		}

		while (first_start != first_end)
		{
			tmp[index] = *first_start;
			++first_start;
			++index;
		}

		while (second_start != second_end)
		{
			tmp[index] = *second_start;
			++second_start;
			++index;
		}

		index = 0;
		while (start != end)
		{
			*start = tmp[index++];
			++start;
		}
	}

	template<class Iter, class Cmp = std::less_equal<>>
	void merge_sort(Iter start, Iter end, Cmp cmp = std::less_equal<>())
	{
		assert(start <= end && "transposed iterator range");

		if (end - start < 2) return;

		Iter middle = start;
		middle += (end - start) / 2;

		Iter first_start = start;
		Iter first_end = middle;

		Iter second_start = middle;
		Iter second_end = end;

		merge_sort(first_start, first_end);
		merge_sort(second_start, second_end);

		merge(first_start, first_end, second_start, second_end, cmp);
	}
}