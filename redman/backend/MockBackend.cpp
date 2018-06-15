#include "redman/backend/MockBackend.h"

// polymorphic classes need to be registered in each backend
#include "backends/export.ipp"

#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

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

} // backend
} // redman
