#pragma once

#include <set>
#include <cstdlib>
#include <stdexcept>

#ifndef PYPLUSPLUS
#include <type_traits>
#include <functional>
#endif // PYPLUSPLUS

#include <algorithm>

#include <boost/iterator/filter_iterator.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/version.hpp>

#include "redman/Whitelist.h"
#include "redman/Blacklist.h"
#include "redman/Predicate.h"

namespace redman {

/** Manages a set of working or defect resources.
 *  @tparam Resource Underlying type for the managed resource.
 *          Will usually be one of the *halbe* coordinate types.
 *  @tparam Policy Has to be one of `Blacklist` (store disabled resources)
 *          or `Whitelist` (store enabled resources).
 *  @tparam Predicate Every resource managed by this class has to
 *          match this predicate.  May be used to restrict resources
 *          to a subset of all possible coordinates.
 *  @tparam Compare Comparison function to be used by internal data structures.
 */
template<typename Resource, typename Policy,
	typename Predicate = DefaultPredicate<Resource>,
	typename Compare = std::less<Resource> >
class ResourceManager
{
#ifndef PYPLUSPLUS
	static_assert(std::is_base_of<redman::Policy, Policy>::value,
	              "Policy must be either black or whitelist");

	static_assert(std::is_base_of<redman::Predicate, Predicate>::value,
	              "Predicate has wrong base class.");

	typedef std::function<bool(Resource const&)> filter_type;
#endif // PYPLUSPLUS

	typedef ResourceManager<Resource, Policy, Predicate, Compare> manager;

public:
	typedef std::set<Resource, Compare> set_type;

#ifndef PYPLUSPLUS
	typedef Resource resource;
	typedef Policy policy;
	typedef Predicate predicate;

	typedef boost::filter_iterator<filter_type,
		PredicateIterator<Predicate> > iterator_type;
#endif // PYPLUSPLUS

	ResourceManager(Predicate const& f = Predicate()) :
		mPredicate(f), mSelection(), mHasValue(false)
	{}

	/** Construct the resource manager with initially enabled resources.
	 *  \throws std::invalid_argument When any resource in the set does not
	 *          fulfill the predicate.
	 *  \see from_set()
	 */
	ResourceManager(set_type const& set_available,
					Predicate const& f = Predicate()) :
		mPredicate(f), mSelection(), mHasValue(false)
	{
		from_set(set_available);
	}

#ifndef PYPLUSPLUS
	/// Return an iterator to the beginning of all enabled resources.
	iterator_type begin() const;
	/// Return an iterator to the end of all enabled resources.
	iterator_type end() const;

	/// Return an iterator to the beginning of all disabled resources.
	iterator_type begin_disabled() const;
	/// Return an iterator to the end of all disabled resources.
	iterator_type end_disabled() const;
#endif // PYPLUSPLUS

	/** Reset enabled resources to policy default.
	 *  When `Policy` is `Whitelist` (`Blacklist`), all resources will
	 *  be disabled (enabled) initially.
	 */
	void reset();

	/** Enable all resources contained in set.
	 *  This clears all currently enabled resources and enables only those
	 *  contained in `other`.  Note that for a resource to be managed by a
	 *  resource manager it has to fulfill its predicate.
	 *  \param  other The set to copy resources from.
	 *  \throws std::invalid_argument When any resource in the set does not
	 *          fulfill the predicate.
	 */
	void from_set(set_type const& other);

	/** Enable the given resource.
	 *  \param  val The resource to enable.
	 *  \throws std::invalid_argument When the resource does not
	 *          fulfill the predicate.
	 *  \throws std::invalid_argument When the resource is already enabled.
	 */
	void enable(Resource const& val);

	/** Disable the given resource.
	 *  \param  val The resource to enable.
	 *  \throws std::invalid_argument When the resource does not
	 *          fulfill the predicate.
	 *  \throws std::invalid_argument When the resource is already disabled.
	 */
	void disable(Resource const& val);

	/** Enable all resources.
	 */
	void enable_all();

	/** Disable all resources.
	 */
	void disable_all();

	/** Only keep resources that are not enabled in another resource manager.
	 *  \throws std::invalid_argument When any resource copied from the other
	 *          manager does not fulfill the predicate of this manager.
	 */
	void difference(manager const& other);

	/** Only keep resources that are not enabled in both resource managers.
	 *  \throws std::invalid_argument When any resource copied from the other
	 *          manager does not fulfill the predicate of this manager.
	 */
	void symmetric_difference(manager const& other);


	/** Only keep resources that are enabled in both resource managers.
	 *  \throws std::invalid_argument When any resource copied from the other
	 *          manager does not fulfill the predicate of this manager.
	 */
	void intersection(manager const& other);

	/** Also enable resources that are enabled in another resource manager.
	 *  \throws std::invalid_argument When any resource copied from the other
	 *          manager does not fulfill the predicate of this manager.
	 */
	void merge(manager const& other);

	/** Return the number of enabled resources.
	 */
	size_t available() const;

	/** Check if a resource is available.
	 */
	bool has(Resource const& val) const;

	/** Check if the manager has information about the resource different from default
	 *  values.
	 */
	bool has_value() const;

	bool operator==(ResourceManager const& rhs) const;
	bool operator!=(ResourceManager const& rhs) const;

private:
	/* mPredicate should essentially be treated as const.
	   The only reason it is not declared as const is boost::serialize. */
	Predicate mPredicate;
	set_type mSelection;
	bool mHasValue;

	/* increment version in components.h */
	friend class boost::serialization::access;
	template <typename Archiver>
	void serialize(Archiver& ar, unsigned int const version) {
		using namespace boost::serialization;

		ar & make_nvp("predicate", mPredicate)
		   & make_nvp("selection", mSelection);

		switch (version) {
			case 0:
				mHasValue = true;
				break;
			case 1:
				ar & make_nvp("hasvalue",  mHasValue);
				break;
			default: {
				throw std::runtime_error(
				    "redman::ResourceManager unknown serialization version");
			}
		}
	}
};

} // redman

// Implementation

#ifndef PYPLUSPLUS
namespace redman {

template<typename Res, typename Pol, typename Pred, typename Cmp>
void ResourceManager<Res, Pol, Pred, Cmp>::reset() {
	mSelection.clear();
	mHasValue = false;
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
void ResourceManager<Res, Pol, Pred, Cmp>::enable_all() {
	Pol::enable_all(mSelection, mPredicate);
	mHasValue = true;
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
void ResourceManager<Res, Pol, Pred, Cmp>::disable_all() {
	Pol::disable_all(mSelection, mPredicate);
	mHasValue = true;
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
bool ResourceManager<Res, Pol, Pred, Cmp>::has(Res const& val) const {
	if (!mPredicate(val))
		throw std::invalid_argument(
			"Resource rejected by predicate.");

	return Pol::has(mSelection, val);
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
size_t ResourceManager<Res, Pol, Pred, Cmp>::available() const {
	return Pol::available(mSelection, mPredicate);
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
void ResourceManager<Res, Pol, Pred, Cmp>::enable(Res const& val) {
	if (!mPredicate(val))
		throw std::invalid_argument(
			"Resource rejected by predicate.");

	Pol::enable(mSelection, val);
	mHasValue = true;
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
void ResourceManager<Res, Pol, Pred, Cmp>::disable(Res const& val) {
	if (!mPredicate(val))
		throw std::invalid_argument(
			"Resource rejected by predicate.");

	Pol::disable(mSelection, val);
	mHasValue = true;
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
void ResourceManager<Res, Pol, Pred, Cmp>::difference(manager const& other) {
	set_type diff;
	std::set_difference(
		begin(), end(),
		other.begin(), other.end(),
		std::inserter(diff, diff.begin()), Cmp());

	Pol::from_set(mSelection, mPredicate, diff);
	mHasValue = true;
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
void ResourceManager<Res, Pol, Pred, Cmp>::symmetric_difference(manager const& other) {
	set_type diff;
	std::set_symmetric_difference(
		begin(), end(),
		other.begin(), other.end(),
		std::inserter(diff, diff.begin()), Cmp());

	Pol::from_set(mSelection, mPredicate, diff);
	mHasValue = true;
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
void ResourceManager<Res, Pol, Pred, Cmp>::intersection(manager const& other) {
	set_type diff;
	std::set_intersection(
		begin(), end(),
		other.begin(), other.end(),
		std::inserter(diff, diff.begin()), Cmp());

	Pol::from_set(mSelection, mPredicate, diff);
	mHasValue = true;
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
void ResourceManager<Res, Pol, Pred, Cmp>::merge(manager const& other) {
	set_type diff;
	std::set_union(
		begin(), end(),
		other.begin(), other.end(),
		std::inserter(diff, diff.begin()), Cmp());

	Pol::from_set(mSelection, mPredicate, diff);
	mHasValue = true;
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
void ResourceManager<Res, Pol, Pred, Cmp>::from_set(set_type const& other) {
	auto const& predicate = mPredicate;
	auto is_invalid = [&predicate](Res const& val) -> bool {
		return !predicate(val);
	};

	if (any_of(other.begin(), other.end(), is_invalid))
		throw std::invalid_argument(
			"Resource in set rejected by predicate.");
	Pol::from_set(mSelection, mPredicate, other);
	mHasValue = true;
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
bool ResourceManager<Res, Pol, Pred, Cmp>::has_value() const{
	return mHasValue;
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
bool ResourceManager<Res, Pol, Pred, Cmp>::operator==(manager const& rhs) const {
	return (mSelection == rhs.mSelection) && (mHasValue == rhs.mHasValue);
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
bool ResourceManager<Res, Pol, Pred, Cmp>::operator!=(manager const& rhs) const {
	return !(*this == rhs);
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
auto ResourceManager<Res, Pol, Pred, Cmp>::begin() const -> iterator_type {
	filter_type filter = std::bind(
		&manager::has,
		this, std::placeholders::_1);

	return boost::make_filter_iterator(
		filter, redman::begin(mPredicate), redman::end(mPredicate));
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
auto ResourceManager<Res, Pol, Pred, Cmp>::end() const -> iterator_type {
	filter_type filter = std::bind(
		&manager::has,
		this, std::placeholders::_1);

	return boost::make_filter_iterator(
		filter, redman::end(mPredicate), redman::end(mPredicate));
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
auto ResourceManager<Res, Pol, Pred, Cmp>::begin_disabled() const -> iterator_type {
	filter_type filter = [this](Res const& r) -> bool {
		return !this->has(r);
	};

	return boost::make_filter_iterator(
		filter, redman::begin(mPredicate), redman::end(mPredicate));
}

template<typename Res, typename Pol, typename Pred, typename Cmp>
auto ResourceManager<Res, Pol, Pred, Cmp>::end_disabled() const -> iterator_type {
	filter_type filter = [this](Res const& r) -> bool {
		return !this->has(r);
	};

	return boost::make_filter_iterator(
		filter, redman::end(mPredicate), redman::end(mPredicate));
}

} // redman
#endif // PYPLUSPLUS
