#pragma once

#include <boost/serialization/export.hpp>
#include <boost/serialization/version.hpp>
#include <boost/shared_ptr.hpp>

#include "pywrap/compat/macros.hpp"

#include "redman/backend/Backend.h"
#include "redman/resources/Base.h"
#include "redman/resources/components.h"


namespace halco {
namespace hicann {
namespace v2 {
class FPGAGlobal;
class HighspeedLinkOnDNC;
}
}
}

namespace HMF {
namespace Coordinate {
using halco::hicann::v2::FPGAGlobal;
using halco::hicann::v2::HighspeedLinkOnDNC;
}
}

namespace redman {
namespace resources {

class Fpga : public Base
{
public:
	Fpga();
	virtual ~Fpga();

	/** HICANNs where the highspeed communication channel can be used
	 */
	boost::shared_ptr<components::HighspeedLinksOnDNC> hslinks();

#ifndef PYPLUSPLUS
	boost::shared_ptr<components::HighspeedLinksOnDNC const> hslinks() const;
#endif // PYPLUSPLUS

	/** Perform intersection for all components
	 */
	void intersection(Fpga const& other);

	void copy(Base const&) PYPP_OVERRIDE;

	// factory function for Py++
	static boost::shared_ptr<Fpga> create();

private:
	boost::shared_ptr<components::HighspeedLinksOnDNC> mHSLinks;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const version);
};

class FpgaWithBackend : public Fpga
{
public:
	typedef halco::hicann::v2::FPGAGlobal id_type;

	FpgaWithBackend(
	    boost::shared_ptr<backend::Backend>,
	    id_type,
	    Fpga const& = Fpga(),
	    bool ignore_missing = true);
	virtual ~FpgaWithBackend();

	/** unique identifier
	 */
	id_type id() const;

	/** save using backend
	 */
	void save() const;

	// factory function for Py++
	static boost::shared_ptr<FpgaWithBackend> create(
	    boost::shared_ptr<backend::Backend> backend, id_type id, bool ignore_missing = true);

	/** id used for the seralization backend, e.g. the basename of the file for xml
	 */
	std::string id_for_backend() const;

private:
	void load(bool ignore_missing);

	id_type mId;
	boost::shared_ptr<backend::Backend> mBackend;

	friend class boost::serialization::access;
	PYPP_DEFAULT(FpgaWithBackend());
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

} // resources
} // redman

BOOST_CLASS_EXPORT_KEY(redman::resources::Fpga)
BOOST_CLASS_EXPORT_KEY(redman::resources::FpgaWithBackend)

BOOST_CLASS_VERSION(redman::resources::Fpga, 1)
BOOST_CLASS_VERSION(redman::resources::FpgaWithBackend, 2)
