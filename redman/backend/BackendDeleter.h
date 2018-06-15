#pragma once

#include <boost/shared_ptr.hpp>

#include "redman/backend/Library.h"

namespace redman {
namespace backend {

class Backend;

class BackendDeleter
{
public:
	BackendDeleter(boost::shared_ptr<Library> lib);

	void operator () (Backend* b) const;

private:
	void destroy_backend(
		void* library,
		Backend* backend) const;

	boost::shared_ptr<Library> lib;
};

} // backend
} // redman
