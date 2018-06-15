#include <stdexcept>
#include <sstream>
#include <dlfcn.h>

#include "redman/backend/Backend.h"
#include "redman/backend/Library.h"
#include "redman/backend/BackendDeleter.h"


namespace redman {
namespace backend {

not_found_error::not_found_error(std::string const& msg) : mMsg(msg) {
}

not_found_error::~not_found_error() throw() {
}

const char* not_found_error::what() const throw() {
	return mMsg.c_str();
}

void Backend::config(
	std::string const& key,
	std::string const& val)
{
	config_(key, config_value_t(val));
}

void Backend::config(
	std::string const& key,
	int const val)
{
	config_(key, config_value_t(val));
}

void Backend::config_(
	std::string const& key,
	config_value_t const& val)
{
	mConfig[key] = val;
	if (val.empty()) {
		mConfig.erase(key);
	}
}

bool Backend::exists(std::string const& key) const
{
	auto it = mConfig.find(key);
	return (it != mConfig.end());
}


boost::shared_ptr<Backend>
loadBackend(boost::shared_ptr<Library> lib)
{
	const char symbol[] = {"createBackend"};

	void* pobj = dlsym(lib->get(), symbol);

	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		std::ostringstream sstr;
		sstr << "Cannot load symbol " << symbol << ": "
			<< dlsym_error << std::endl;
		throw std::runtime_error(sstr.str());
	}

	// ISO C++ forbids casting between pointer-to-function and pointer-to-object.
	// Because dlsym's signature is pointer-to-object we have to jump through
	// some hoops so we don't trigger warnings in some compilers.
	// As POSIX requires that an object of type void* can hold a pointer to a function
	// we are save. Obviously this is not portable to other platforms...
	Backend* (*factory)();
	*reinterpret_cast<void **>(&factory) = pobj;

	Backend* backend = factory();

	return boost::shared_ptr<Backend>(backend, BackendDeleter(lib));
}

} // backend
} // redman
