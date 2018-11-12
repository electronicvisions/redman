#include <string>

#include <boost/make_shared.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "hal/Coordinate/External.h"

#include "redman/resources/Fpga.h"
#include "redman/resources/components.h"

namespace redman {
namespace resources {

Fpga::Fpga() : mHicanns(boost::make_shared<components::HicannsOnHS>()) {}

Fpga::~Fpga() {}

boost::shared_ptr<components::HicannsOnHS> Fpga::hicanns()
{
	return mHicanns;
}

boost::shared_ptr<components::HicannsOnHS const> Fpga::hicanns() const
{
	return mHicanns;
}

void Fpga::copy(Base const& rhs)
{
	*this = dynamic_cast<Fpga const&>(rhs);
}

boost::shared_ptr<Fpga> Fpga::create()
{
	return boost::make_shared<Fpga>();
}

FpgaWithBackend::FpgaWithBackend(
    boost::shared_ptr<backend::Backend> backend,
    FpgaWithBackend::id_type id,
    Fpga const& base,
    bool ignore_missing)
    : Fpga(base), mId(id), mBackend(backend)
{
	load(ignore_missing);
}

FpgaWithBackend::~FpgaWithBackend() {}

FpgaWithBackend::id_type FpgaWithBackend::id() const
{
	return mId;
}

void FpgaWithBackend::load(bool ignore_missing)
{
	if (!mBackend)
		throw std::runtime_error("Fpga needs backend to load data.");

	try {
		mBackend->load(id_for_backend(), *this);
	} catch (const backend::not_found_error&) {
		if (!ignore_missing) {
			throw;
		}
	}
}

void FpgaWithBackend::save() const
{
	if (!mBackend)
		throw std::runtime_error("Fpga needs backend to save data.");

	mBackend->store(id_for_backend(), *this);
}

std::string FpgaWithBackend::id_for_backend() const
{
	return "fpga-" + std::to_string(mId.toWafer()) + "-" +
	       std::to_string(mId.toFPGAOnWafer().toEnum().value());
}

boost::shared_ptr<FpgaWithBackend> FpgaWithBackend::create(
    boost::shared_ptr<backend::Backend> backend, FpgaWithBackend::id_type id, bool ignore_missing)
{
	return boost::make_shared<FpgaWithBackend>(backend, id, Fpga(), ignore_missing);
}

template<typename Archiver>
void Fpga::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Base);

	ar& make_nvp("hicanns", mHicanns);
}

template<typename Archiver>
void FpgaWithBackend::serialize(Archiver& ar, unsigned int const)
{
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Fpga);
	// Do not add further serialization here!
	// This class is only a shim to encapsulate communication with the backend.
}

} // resources
} // redman

BOOST_CLASS_EXPORT_IMPLEMENT(redman::resources::Fpga)
BOOST_CLASS_EXPORT_IMPLEMENT(redman::resources::FpgaWithBackend)

#include "boost/serialization/serialization_helper.tcc"
EXPLICIT_INSTANTIATE_BOOST_SERIALIZE(redman::resources::Fpga)
EXPLICIT_INSTANTIATE_BOOST_SERIALIZE(redman::resources::FpgaWithBackend)
