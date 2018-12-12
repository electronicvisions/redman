#pragma once

#include <stdexcept>

#ifndef PYPLUSPLUS
#include <type_traits>
#endif // PYPLUSPLUS

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/serialization/serialization.hpp>

namespace redman {

struct Predicate
{
};

namespace detail {

template <typename T>
struct void_type {
    typedef void type;
};

}

template<typename Resource, typename Enable = void>
struct DefaultPredicate;

template<typename Resource>
struct DefaultPredicate<Resource, typename detail::void_type<typename Resource::value_type>::type>
	: public Predicate
{
	typedef Resource resource_type;
	typedef resource_type index_type;

	bool operator() (resource_type const&) const
	{
		// By default all constructible resources are valid.
		return true;
	}

	resource_type construct(index_type const& idx) const
	{
		return resource_type(idx);
	}
};

template<typename Resource>
struct DefaultPredicate<Resource, typename detail::void_type<typename Resource::enum_type>::type>
	: public Predicate
{
	typedef Resource resource_type;
	typedef typename resource_type::enum_type index_type;

	bool operator() (resource_type const&) const
	{
		// By default all constructible resources are valid.
		return true;
	}

	resource_type construct(index_type const& idx) const
	{
		return resource_type(idx);
	}
};

template<typename Predicate>
class PredicateIterator :
	public boost::iterator_facade<
	PredicateIterator<Predicate>,
	typename Predicate::resource_type,
	boost::forward_traversal_tag,
	// Return copy instead of reference:
	typename Predicate::resource_type>
{
	typedef typename Predicate::index_type index_type;
	typedef typename Predicate::resource_type resource_type;

	typedef typename index_type::value_type index_value_type;

	inline resource_type make_resource(index_value_type v) const
	{
		return mPredicate.construct(index_type(v));
	}

	template<typename T>
	typename boost::enable_if_c<boost::is_unsigned<T>::value && index_type::begin==0>::type
	range_check(T const idx) const
	{
		if (idx < index_type::end && !mPredicate(make_resource(idx)))
			throw std::invalid_argument("Index not valid or rejected by predicate.");
	}

	template<typename T>
	typename boost::enable_if_c<!boost::is_unsigned<T>::value || index_type::begin!=0>::type
	range_check(T const idx) const
	{
		if (idx < index_type::begin || (idx < index_type::end && !mPredicate(make_resource(idx))))
			throw std::invalid_argument("Index not valid or rejected by predicate.");
	}

public:
	PredicateIterator(
			index_value_type idx,
			Predicate const& predicate)
		: mPredicate(predicate), mIndex(idx)
	{
		range_check(idx);
	}

	PredicateIterator(Predicate const& predicate=Predicate())
		: mPredicate(predicate), mIndex(index_type::end)
	{}

private:
	friend class boost::iterator_core_access;

	bool equal(PredicateIterator const& other) const
	{
		return mIndex == other.mIndex;
	}

	void increment()
	{
		if (mIndex >= index_type::end)
			return;

		index_value_type next = mIndex + 1;
		for (; next < index_type::end; ++next)
		{
			if (mPredicate(make_resource(next)))
				break;
		}

		mIndex = next;
	}

	resource_type dereference() const
	{
		if (mIndex >= index_type::end)
			throw std::runtime_error(
				"Can not construct resource for iterator pointing past end.");
		return make_resource(mIndex);
	}

	Predicate mPredicate;
	index_value_type mIndex;
};

} // redman

#ifndef PYPLUSPLUS
namespace redman {
template<typename Predicate>
typename std::enable_if<std::is_base_of<redman::Predicate, Predicate>::value, redman::PredicateIterator<Predicate> >::type
begin(Predicate const& pred)
{
	return { Predicate::index_type::begin, pred };
}

template<typename Predicate>
typename std::enable_if<std::is_base_of<redman::Predicate, Predicate>::value, redman::PredicateIterator<Predicate> >::type
end(Predicate const& pred)
{
	return { Predicate::index_type::end, pred };
}
} // redman

namespace boost {
namespace serialization {
template<typename Archive, typename Resource>
void serialize(Archive&, redman::DefaultPredicate<Resource>&, unsigned int const)
{}

} // serialization
} // boost
#endif // PYPLUSPLUS
