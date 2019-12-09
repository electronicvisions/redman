#include <boost/make_shared.hpp>

#include "redman/resources/Hicann.h"

namespace redman {
namespace resources {

Hicann::Hicann()
    : mNeurons(boost::make_shared<components::Neurons>()),
      mSynapses(boost::make_shared<components::Synapses>()),
      mDrivers(boost::make_shared<components::SynapseDrivers>()),
      mSynapticInputs(boost::make_shared<components::SynapticInputs>()),
      mSynapseRows(boost::make_shared<components::SynapseRows>()),
      mAnalogs(boost::make_shared<components::Analogs>()),
      mBackgroundGenerators(boost::make_shared<components::BackgroundGenerators>()),
      mFGBlocks(boost::make_shared<components::FGBlocks>()),
      mVRepeaters(boost::make_shared<components::VRepeaters>()),
      mHRepeaters(boost::make_shared<components::HRepeaters>()),
      mSynapseSwitches(boost::make_shared<components::SynapseSwitches>()),
      mCrossbarSwitches(boost::make_shared<components::CrossbarSwitches>()),
      mSynapseSwitchRows(boost::make_shared<components::SynapseSwitchRows>()),
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

void Hicann::intersection(Hicann const& other) {
	neurons()->intersection(*other.neurons());
	synapses()->intersection(*other.synapses());
	drivers()->intersection(*other.drivers());
	synaptic_inputs()->intersection(*other.synaptic_inputs());
	synapserows()->intersection(*other.synapserows());
	analogs()->intersection(*other.analogs());
	backgroundgenerators()->intersection(*other.backgroundgenerators());
	fgblocks()->intersection(*other.fgblocks());
	vrepeaters()->intersection(*other.vrepeaters());
	hrepeaters()->intersection(*other.hrepeaters());
	synapseswitches()->intersection(*other.synapseswitches());
	crossbarswitches()->intersection(*other.crossbarswitches());
	synapseswitchrows()->intersection(*other.synapseswitchrows());

	hbuses()->intersection(*other.hbuses());
	vbuses()->intersection(*other.vbuses());

	mergers0()->intersection(*other.mergers0());
	mergers1()->intersection(*other.mergers1());
	mergers2()->intersection(*other.mergers2());
	mergers3()->intersection(*other.mergers3());
	dncmergers()->intersection(*other.dncmergers());
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
	id << "hicann-" << mId.toWafer() << "-" << mId.toHICANNOnWafer().toEnum();
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
