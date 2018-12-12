#pragma once

#include <string>
#include <sstream>
#include <unordered_map>

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
};

} // backend
} // redman
