#pragma once

#include <string>
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
