#pragma once

#include <boost/serialization/export.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/filesystem.hpp>

#include <fstream>

#include "redman/backend/Backend.h"

namespace redman {
namespace backend {

class XMLBackend :
	public Backend
{
public:
	XMLBackend();
	virtual ~XMLBackend();

	virtual void init();

	virtual void load(std::string const& id, resources::Base&);

	virtual void store(std::string const& id, resources::Base const&);


private:
	typedef boost::filesystem::path path;

	path&       getPath();
	path const& getPath() const;

	path getFilename(std::string const& id) const;

	path  mPath;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

} // backend
} // redman

BOOST_CLASS_EXPORT_KEY(redman::backend::XMLBackend)
