#include "redman/backends/xml/XMLBackend.h"

// polymorphic classes need to be registered in each backend
#include "redman/backends/export.ipp"

#include "redman/backend/interface.h"

#include <stdexcept>
#include <iostream>

#include "boost/serialization/path.h"
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/export.hpp>

using namespace redman::resources;

namespace redman {
namespace backend {

XMLBackend::XMLBackend() :
	mPath(".") {}

XMLBackend::~XMLBackend() {}

XMLBackend::path& XMLBackend::getPath()
{
	return mPath;
}

XMLBackend::path const& XMLBackend::getPath() const
{
	return mPath;
}

void XMLBackend::init()
{
	namespace fs = boost::filesystem;
	if (exists("path")) {
		getPath() = get<std::string>("path");
		if (!fs::is_directory(getPath())) {
			std::string err = "Path not available: " + getPath().string();
			throw std::runtime_error(err);
		}
	}
}

void XMLBackend::load(std::string const& id, Base& res)
{
	namespace fs = boost::filesystem;
	auto file = getFilename(id);
	if (!fs::exists(file)) {
		throw not_found_error("data set not found: " + file.native());
	}

	std::fstream stream(file.string(), std::ios::in | std::ios::binary);

	boost::archive::xml_iarchive ia(stream);

	boost::shared_ptr<Base const> ptr;
	ia >> boost::serialization::make_nvp("data", ptr);
	res.copy(*ptr);
}

void XMLBackend::store(std::string const& id, Base const& res)
{
	auto file = getFilename(id);
	std::fstream stream(file.string(), std::ios::out | std::ios::binary);

	boost::archive::xml_oarchive oa(stream);
	boost::shared_ptr<Base const> ptr(&res, boost::serialization::null_deleter());
	oa << boost::serialization::make_nvp("data", ptr);
}

XMLBackend::path
XMLBackend::getFilename(
	std::string const& id) const
{
	return getPath() / (id + ".xml");
}

template<typename Archiver>
void XMLBackend::serialize(Archiver & ar, unsigned int const /*version*/)
{
	using boost::serialization::make_nvp;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Backend);
	ar & make_nvp("path", mPath);
}
} // backend
} // redman

BOOST_CLASS_EXPORT_IMPLEMENT(redman::backend::XMLBackend)

#include "boost/serialization/serialization_helper.tcc"
EXPLICIT_INSTANTIATE_BOOST_SERIALIZE(redman::backend::XMLBackend)

extern "C" {

backend_t* createBackend()
{
	return new redman::backend::XMLBackend();
}

void destroyBackend(backend_t* backend)
{
	delete backend;
}

} // extern "C"
