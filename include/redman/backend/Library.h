#pragma once

#include <string>

// GCCXML has problems with atomics -> removed before boost shared_prt is included
#ifdef PYPLUSPLUS
#undef __ATOMIC_RELAXED
#undef __ATOMIC_ACQUIRE
#undef __ATOMIC_RELEASE
#endif // PYPLUSPLUS
#include <boost/shared_ptr.hpp>

namespace redman {
namespace backend {

class Library
{
public:
	void*       get();
	void const* get() const;

	~Library();

private:
	friend boost::shared_ptr<Library>
		loadLibrary(std::string const&);

	Library(std::string const& name, bool const& set_lang = true);

	void* ptr;
};

boost::shared_ptr<Library>
loadLibrary(std::string const& name);

} // backend
} // redman
