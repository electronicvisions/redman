#include <boost/make_shared.hpp>

#include "redman/resources/Hicann.h"

namespace redman {
namespace resources {

Hicann::Hicann()
    : mNeurons(boost::make_shared<components::Neurons>()),
      mSynapses(boost::make_shared<components::Synapses>()),
      mDrivers(boost::make_shared<components::SynapseDrivers>()),
      mSynapticInputs(boost::make_shared<components::SynapticInputs>()),
      mHBuses(boost::make_shared<components::HorizontalBuses>()),
      mVBuses(boost::make_shared<components::VerticalBuses>()),
      mMergers0(boost::make_shared<components::Mergers0>()),
      mMergers1(boost::make_shared<components::Mergers1>()),
      mMergers2(boost::make_shared<components::Mergers2>()),
      mMergers3(boost::make_shared<components::Mergers3>()),
      mDNCMergers(boost::make_shared<components::DNCMergers>()) {
}

Hicann::~Hicann() {
}

void Hicann::copy(Base const& rhs) {
	*this = dynamic_cast<Hicann const&>(rhs);
}

boost::shared_ptr<Hicann> Hicann::create() {
	return boost::make_shared<Hicann>();
}

HicannWithBackend::HicannWithBackend(
		boost::shared_ptr<backend::Backend> backend,
		HMF::Coordinate::HICANNGlobal id,
		Hicann const& base,
		bool ignore_missing)
	: Hicann(base), mId(id), mBackend(backend) {
	load(ignore_missing);
}

HicannWithBackend::~HicannWithBackend() {
}

void HicannWithBackend::load(bool ignore_missing) {
	if (!mBackend)
		throw std::runtime_error("Hicann needs backend to load data.");

	try {
		mBackend->load(id_for_backend(), *this);
	} catch (const backend::not_found_error&) {
		if (!ignore_missing) {
			throw;
		}
	}
}

void HicannWithBackend::save() {
	if (!mBackend)
		throw std::runtime_error("Hicann needs backend to save data.");

	mBackend->store(id_for_backend(), *this);
}

std::string HicannWithBackend::id_for_backend() const
{
	std::stringstream id;
	id << "hicann-" << mId.toWafer() << "-" << mId.toHICANNOnWafer().id();
	return id.str();
}

boost::shared_ptr<HicannWithBackend> HicannWithBackend::create(
		boost::shared_ptr<backend::Backend> backend,
		HMF::Coordinate::HICANNGlobal id,
		bool ignore_missing) {
	return boost::make_shared<HicannWithBackend>(backend, id, Hicann(), ignore_missing);
}

} // resources
} // redman
