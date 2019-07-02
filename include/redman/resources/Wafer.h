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
	Wafer();
	virtual ~Wafer();

	/** Check whether the given HICANN is enabled on this wafer.
	 */
	bool has(HMF::Coordinate::HICANNOnWafer const&) const;

	/** Check whether the given FPGA is enabled on this wafer.
	 */
	bool has(HMF::Coordinate::FPGAOnWafer const&) const;

	boost::shared_ptr<components::Hicanns>       hicanns();
	boost::shared_ptr<components::Fpgas>         fpgas();

#ifndef PYPLUSPLUS
	boost::shared_ptr<components::Hicanns const> hicanns() const;
	boost::shared_ptr<components::Fpgas const> fpgas() const;

	void copy(Base const&) override;
#else
	void copy(Base const&);
#endif // PYPLUSPLUS

	// factory function for Py++
	static boost::shared_ptr<Wafer> create();

private:
	boost::shared_ptr<components::Hicanns> mHicanns;
	boost::shared_ptr<components::Fpgas> mFpgas;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

class WaferWithBackend : public Wafer {
public:
	typedef HMF::Coordinate::Wafer id_type;

	WaferWithBackend(
		boost::shared_ptr<backend::Backend> backend,
		HMF::Coordinate::Wafer id,
		bool ignore_missing = true);
	virtual ~WaferWithBackend();

	id_type id() const { return mId; }

	/** Return the `Hicann` instance for the given coordinate.
	 * Defect data for HICANNs is lazy-loaded from the backend.  Alternatively,
	 * a `Hicann` instance can be provided via `inject()`, shadowing any data in
	 * the backend.
	 * \returns Empty shared pointer if hicann is disabled in `hicanns()`.
	 * \throws std::runtime_error If the resource manager used in `hicanns()`
	           is not available yet.
	 */
	boost::shared_ptr<Hicann> get(HMF::Coordinate::HICANNOnWafer const&) const;

	/** Return the `Fpga` instance for the given coordinate.
	 * Defect data for FPGAs is lazy-loaded from the backend.  Alternatively,
	 * a `Fpga` instance can be provided via `inject()`, shadowing any data in
	 * the backend.
	 * \returns Empty shared pointer if fpga is disabled in `fpgas()`.
	 * \throws std::runtime_error If the resource manager used in `fpgas()`
	           is not available yet.
	 */
	boost::shared_ptr<Fpga> get(HMF::Coordinate::FPGAOnWafer const&) const;

	/** Inject the given HICANN resource into the runtime cache.
	 * The provided resource will be returned by `get()`, circumventing access
	 * to the backend.  Note that a subsequent lookup via `get()` still dependends on the
	 * status of the HICANN in the `hicanns()` resource manager.
	 */
	void inject(HMF::Coordinate::HICANNOnWafer const&, boost::shared_ptr<Hicann>);

	/** Inject the given FPGA resource into the runtime cache.
	 * The provided resource will be returned by `get()`, circumventing access
	 * to the backend.  Note that a subsequent lookup via `get()` still depends on the
	 * status of the FPGA in the `fpgas()` resource manager.
	 */
	void inject(HMF::Coordinate::FPGAOnWafer const&, boost::shared_ptr<Fpga>);

	void save();

	// factory function for Py++
	static boost::shared_ptr<WaferWithBackend> create(
		boost::shared_ptr<backend::Backend> backend,
		HMF::Coordinate::Wafer id,
		bool ignore_missing = true);

private:
	void load(bool ignore_missing);

	id_type mId;
	boost::shared_ptr<backend::Backend> mBackend;
#ifndef PYPLUSPLUS
	mutable std::unordered_map<HMF::Coordinate::HICANNOnWafer, boost::shared_ptr<Hicann> > mHicannCache;
	mutable std::unordered_map<HMF::Coordinate::FPGAOnWafer, boost::shared_ptr<Fpga> > mFpgaCache;
#endif // PYPLUSPLUS

	friend class boost::serialization::access;
#ifndef PYPLUSPLUS
	WaferWithBackend() = default;
#else
	WaferWithBackend();
#endif // PYPLUSPLUS
	template<typename Archiver>
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
}

template<typename Archiver>
void WaferWithBackend::serialize(Archiver& ar, unsigned int const version)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Wafer);
	if (version > 0) {
		ar & make_nvp("id", mId);
	}
}

} // resources
} // redman

BOOST_CLASS_VERSION(redman::resources::Wafer, 2)
BOOST_CLASS_VERSION(redman::resources::WaferWithBackend, 1)