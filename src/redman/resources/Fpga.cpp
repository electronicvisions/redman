#include <string>

#include <boost/make_shared.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "halco/hicann/v2/external.h"

#include "redman/resources/Fpga.h"
#include "redman/resources/components.h"

namespace redman {
namespace resources {

Fpga::Fpga() : mHSLinks(boost::make_shared<components::HighspeedLinksOnDNC>()) {}

Fpga::~Fpga() {}

void Fpga::intersection(Fpga const& other) {
	hslinks()->intersection(*other.hslinks());
}

boost::shared_ptr<components::HighspeedLinksOnDNC> Fpga::hslinks()
{
	return mHSLinks;
}

boost::shared_ptr<components::HighspeedLinksOnDNC const> Fpga::hslinks() const
{
	return mHSLinks;
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
void Fpga::serialize(Archiver& ar, unsigned int const version)
{
	using namespace boost::serialization;
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Base);

	if (version < 1) {
		throw std::runtime_error(
		    "redman::resources::Fpga: (de)serialization of version " + std::to_string(version) +
		    " not supported.");
	}

	ar& make_nvp("hslinks", mHSLinks);
}

template<typename Archiver>
void FpgaWithBackend::serialize(Archiver& ar, unsigned int const version)
{
	using namespace boost::serialization;
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Fpga);
	if (version > 1) {
		ar & make_nvp("id", mId);
	}
}

bool operator==(Fpga const& a, Fpga const& b)
{
	if (static_cast<bool>(a.mHSLinks) != static_cast<bool>(b.mHSLinks)) {
		return false;
	} else if (static_cast<bool>(a.mHSLinks) && ((*a.mHSLinks) != (*b.mHSLinks))) {
		return false;
	}
	return true;
}

bool operator!=(Fpga const& a, Fpga const& b)
{
	return !(a == b);
}

} // resources
} // redman

BOOST_CLASS_EXPORT_IMPLEMENT(redman::resources::Fpga)
BOOST_CLASS_EXPORT_IMPLEMENT(redman::resources::FpgaWithBackend)

#include "boost/serialization/serialization_helper.tcc"
EXPLICIT_INSTANTIATE_BOOST_SERIALIZE(redman::resources::Fpga)
EXPLICIT_INSTANTIATE_BOOST_SERIALIZE(redman::resources::FpgaWithBackend)
