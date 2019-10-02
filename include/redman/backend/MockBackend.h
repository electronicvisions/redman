#pragma once

#include <string>
#include <sstream>
#include <unordered_map>

#include <boost/serialization/export.hpp>

#include "redman/backend/Backend.h"

namespace redman {
namespace backend {

class MockBackend : public redman::backend::Backend {
public:
	MockBackend() {};
	virtual ~MockBackend() {};

	virtual void init() {};

	virtual void load(std::string const& id, resources::Base&);

	virtual void store(std::string const& id, resources::Base const&);

private:
	std::unordered_map<std::string, std::string> mStore;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);

};

} // backend
} // redman

BOOST_CLASS_EXPORT_KEY(redman::backend::MockBackend)
