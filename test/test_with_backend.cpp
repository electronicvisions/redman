#include <stdexcept>
#include "test/test_with_backend.h"

using namespace redman::backend;

boost::shared_ptr<Backend> TestWithBackend<XMLBackend>::backend;
boost::filesystem::path TestWithBackend<XMLBackend>::backendPath;

boost::shared_ptr<Backend> init_backend(std::string const& fname) {
	auto lib = loadLibrary(fname);
	boost::shared_ptr<Backend> backend = loadBackend(lib);

	if (!backend) {
		std::stringstream s;
		s << "unable to load: " << fname;
		throw std::runtime_error(s.str());
	}

	return backend;
}
