#pragma once

#ifndef PYPLUSPLUS
#include <unordered_map>
#endif // PYPLUSPLUS

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "redman/backend/Backend.h"
#include "redman/resources/Base.h"
#include "redman/resources/Hicann.h"
#include "redman/resources/Fpga.h"
#include "redman/resources/components.h"

namespace redman {
namespace resources {

class Wafer : public Base {
public:
	typedef halco::hicann::v2::Wafer id_type;
	Wafer(id_type const& id = id_type());
	virtual ~Wafer();

	id_type id() const;

	/** Check whether the given HICANN is enabled on this wafer.
	 */
	bool has(halco::hicann::v2::HICANNOnWafer const&) const;

	/** Check whether the given FPGA is enabled on this wafer.
	 */
	bool has(halco::hicann::v2::FPGAOnWafer const&) const;

	struct get_behavior {
		get_behavior(bool ihm = false, bool ifm = false);
		bool ignore_hicann_missing;
		bool ignore_fpga_missing;
	};

	void set_get_behavior(get_behavior const& behavior);

	struct backend_behavior
	{
		backend_behavior(bool iwm = true, bool ihm = true, bool ifm = true);
		bool ignore_wafer_missing;
		bool ignore_hicann_missing;
		bool ignore_fpga_missing;
	};

	void set_backend(
	    id_type const& id,
	    boost::shared_ptr<backend::Backend> backend,
	    backend_behavior const& behavior = backend_behavior());

	/** Get the HICANN resource
	 * Priority: 1. runtime cache, 2. via backend, 3. new if ignore_hicann_missing
	 * 2 and 3 are then also inserted into the runtime cache
	 */
	boost::shared_ptr<resources::Hicann> get(halco::hicann::v2::HICANNOnWafer const&) const;

	/** Get the FPGA resource
	 * Priority: 1. runtime cache, 2. via backend, 3. new if ignore_fpga_missing
	 * 2 and 3 are then also inserted into the runtime cache
	 */
	boost::shared_ptr<resources::Fpga> get(halco::hicann::v2::FPGAOnWafer const&) const;

	boost::shared_ptr<components::Hicanns>       hicanns();
	boost::shared_ptr<components::Fpgas>         fpgas();

	/** Inject the given HICANN resource into the runtime cache.
	 * The provided resource will be returned by `get()`, circumventing access
	 * to the backend.
	 */
	void inject(halco::hicann::v2::HICANNOnWafer const&, boost::shared_ptr<Hicann>);

	/** Inject the given FPGA resource into the runtime cache.
	 * The provided resource will be returned by `get()`, circumventing access
	 * to the backend.
	 */
	void inject(halco::hicann::v2::FPGAOnWafer const&, boost::shared_ptr<Fpga>);

	/** Store Hicanns and Fpgas components to disk, backend must be set.
	 * Does not include Hicann and Fpga resources!
	 */
	void save();

	/** Perform intersection for all components
	 */
	void intersection(Wafer const& other);

#ifndef PYPLUSPLUS
	boost::shared_ptr<components::Hicanns const> hicanns() const;
	boost::shared_ptr<components::Fpgas const> fpgas() const;

	void copy(Base const&) override;
#else
	void copy(Base const&);
#endif // PYPLUSPLUS

	// factory function for Py++
	static boost::shared_ptr<Wafer> create(id_type const& id = id_type());

	/// call to load data from backend, reloads components but does
	/// not touch resource caches
	void load();

protected:
	void set_id(id_type const& id);

private:
	void set_backend_behavior(backend_behavior const& behavior);

	id_type mId;
	boost::shared_ptr<backend::Backend> mBackend;

	boost::shared_ptr<components::Hicanns> mHicanns;
	boost::shared_ptr<components::Fpgas> mFpgas;

	backend_behavior mBackendBehavior;
	get_behavior mGetBehavior;

#ifndef PYPLUSPLUS
	mutable std::unordered_map<halco::hicann::v2::HICANNOnWafer, boost::shared_ptr<Hicann> > mHicannResourceCache;
	mutable std::unordered_map<halco::hicann::v2::FPGAOnWafer, boost::shared_ptr<Fpga> > mFpgaResourceCache;
#endif // PYPLUSPLUS

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

// keep deserialization of old data alive ...
class WaferWithBackend : public Wafer {
public:
	WaferWithBackend(
	    boost::shared_ptr<backend::Backend> backend,
	    id_type const& id,
	    bool ignore_missing = true);
	virtual ~WaferWithBackend();

	static boost::shared_ptr<WaferWithBackend> create(
	    boost::shared_ptr<backend::Backend> backend,
	    id_type const& id,
	    bool ignore_missing = true);

private:

#ifndef PYPLUSPLUS
	WaferWithBackend() = default;
#else
	WaferWithBackend();
#endif // PYPLUSPLUS

	friend class boost::serialization::access;
	template <typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

} // resources
} // redman

BOOST_CLASS_EXPORT_KEY(redman::resources::Wafer)
BOOST_CLASS_EXPORT_KEY(redman::resources::WaferWithBackend)

namespace redman {
namespace resources {

template<typename Archiver>
void Wafer::serialize(Archiver& ar, unsigned int const version)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Base)
	   & make_nvp("hicanns", mHicanns);

	if (version > 0) {
		ar& make_nvp("fpgas", mFpgas);
	}
	if (version > 2) {
		ar & make_nvp("id", mId);
	}

}

template<typename Archiver>
void WaferWithBackend::serialize(Archiver& ar, unsigned int const version)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Wafer);
	if (version == 1 && typename Archiver::is_loading()) {
		id_type id;
		ar & make_nvp("id", id);
		set_id(id);
	}
}

} // resources
} // redman

BOOST_CLASS_VERSION(redman::resources::Wafer, 3)
BOOST_CLASS_VERSION(redman::resources::WaferWithBackend, 2)
