#pragma once

#include <set>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include "redman/Policy.h"

namespace redman {

struct Whitelist :
	public Policy
{
	template<typename Set, typename Predicate>
	static
	void enable_all(Set& set, Predicate const& predicate)
	{
#ifndef PYPLUSPLUS
		set.clear();
		for (auto res : predicate)
		{
			set.insert(res);
		}
#endif // PYPLUSPLUS
	}

	template<typename Set, typename Predicate>
	static
	void disable_all(Set& set, Predicate const&)
	{
		set.clear();
	}

	template<typename Set>
	static
	bool has(Set const& set, typename Set::const_reference val)
	{
		if (set.find(val) == set.end())
			return false;
		return true;
	}

	template<typename Set>
	static
	void enable(Set& set, typename Set::const_reference val, switch_mode::type mode = switch_mode::THROW)
	{
		std::pair<typename Set::iterator, bool> r = set.insert(val);
		if ((mode == switch_mode::THROW) && !r.second) {
			std::stringstream error_msg;
			error_msg << "could not enable resource: " << val;
			throw std::runtime_error(error_msg.str().c_str());
		}
	}

	template<typename Set>
	static
	void disable(Set& set, typename Set::const_reference val, switch_mode::type mode = switch_mode::THROW)
	{
		typename Set::const_iterator it = set.find(val);
		if (it == set.end()) {
			if (mode == switch_mode::NONTHROW) return;
			std::stringstream error_msg;
			error_msg << "could not disable resource: " << val;
			throw std::runtime_error(error_msg.str().c_str());
		}
		set.erase(it);
	}

	template<typename Set, typename Predicate>
	static
	size_t available(Set const& set, Predicate const&)
	{
		return set.size();
	}

	template<typename Set, typename Predicate>
	static
	void from_set(
		Set& set,
		Predicate const& predicate,
		Set const& other)
	{
#ifndef PYPLUSPLUS
		enable_all(set, predicate);
		Set res;
		std::set_intersection(
			set.begin(), set.end(),
			other.begin(), other.end(),
			std::inserter(res, res.begin()),
			typename Set::key_compare());
		set = std::move(res);
#endif // PYPLUSPLUS
	}
};

} // redman
