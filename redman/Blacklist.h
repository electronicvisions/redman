#pragma once

#include <set>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>

#include "redman/Policy.h"

namespace redman {

struct Blacklist :
	public Policy
{
	template<typename Set, typename Predicate>
	static
	void enable_all(Set& set, Predicate const&)
	{
		set.clear();
	}

	template<typename Set, typename Predicate>
	static
	void disable_all(Set& set, Predicate const& predicate)
	{
#ifndef PYPLUSPLUS
		set.clear();
		for (auto res : predicate)
		{
			set.insert(res);
		}
#endif // PYPLUSPLUS
	}

	template<typename Set>
	static
	bool has(Set const& set, typename Set::const_reference val)
	{
		if (set.find(val) == set.end())
			return true;
		return false;
	}

	template<typename Set>
	static
	void enable(Set& set, typename Set::const_reference val)
	{
		typename Set::const_iterator it = set.find(val);
		if (it == set.end())
			throw std::runtime_error("could not enable resource");
		set.erase(it);
	}

	template<typename Set>
	static
	void disable(Set& set, typename Set::const_reference val)
	{
		std::pair<typename Set::iterator, bool> r = set.insert(val);
		if (!r.second)
			throw std::runtime_error("could not disable resource");
	}

	template<typename Set, typename Predicate>
	static
	size_t available(Set const& set, Predicate const& pred)
	{
		size_t all = 0;


#ifndef PYPLUSPLUS
		for (auto it = begin(pred); it != end(pred); ++it)
			++all;
#endif // PYPLUSPLUS

		return all - set.size();
	}

	template<typename Set, typename Predicate>
	static
	void from_set(
		Set& set,
		Predicate const& predicate,
		Set const& other)
	{
#ifndef PYPLUSPLUS
		disable_all(set, predicate);
		Set res;
		std::set_difference(
			set.begin(), set.end(),
			other.begin(), other.end(),
			std::inserter(res, res.begin()),
			typename Set::key_compare());
		set = std::move(res);
#endif // PYPLUSPLUS
	}
};

} // redman
