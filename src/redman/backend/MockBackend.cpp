#include "redman/backend/MockBackend.h"

// polymorphic classes need to be registered in each backend
#include "redman/backends/export.ipp"

#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/unordered_map.hpp>

using namespace redman::resources;

namespace redman {
namespace backend {

void MockBackend::load(std::string const& id, Base& res) {
	auto it = mStore.find(id);
	if (it == mStore.end())
		throw not_found_error("couldn't find data set");
	auto str = it->second;

	std::istringstream is(str);
	boost::archive::binary_iarchive ia(is);
	boost::shared_ptr<Base const> ptr;
	ia >> ptr;
	res.copy(*ptr);
}

void MockBackend::store(std::string const& id, Base const& res) {
	std::ostringstream os;
	boost::archive::binary_oarchive oa(os);
	boost::shared_ptr<Base const> ptr(&res, boost::serialization::null_deleter());
	oa << ptr;

	auto str = os.str();
	auto it = mStore.insert({id, str});
	if (!it.second)
		it.first->second = str;
}

template <typename Archiver>
void MockBackend::serialize(Archiver& ar, unsigned int const)
{
	using boost::serialization::make_nvp;
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Backend);
	ar& make_nvp("store", mStore);
}

} // backend
} // redman

BOOST_CLASS_EXPORT_IMPLEMENT(redman::backend::MockBackend)

#include "boost/serialization/serialization_helper.tcc"
EXPLICIT_INSTANTIATE_BOOST_SERIALIZE(redman::backend::MockBackend)
