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
      mSynapseArrays(boost::make_shared<components::SynapseArrays>()),
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
	synapsearrays()->intersection(*other.synapsearrays());

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
		halco::hicann::v2::HICANNGlobal id,
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
		halco::hicann::v2::HICANNGlobal id,
		bool ignore_missing) {
	return boost::make_shared<HicannWithBackend>(backend, id, Hicann(), ignore_missing);
}

bool operator==(Hicann const& a, Hicann const& b)
{
	if (static_cast<bool>(a.mNeurons) != static_cast<bool>(b.mNeurons)) {
		return false;
	} else if (static_cast<bool>(a.mNeurons) && ((*a.mNeurons) != (*b.mNeurons))) {
		return false;
	}

	if (static_cast<bool>(a.mSynapses) != static_cast<bool>(b.mSynapses)) {
		return false;
	} else if (static_cast<bool>(a.mSynapses) && ((*a.mSynapses) != (*b.mSynapses))) {
		return false;
	}

	if (static_cast<bool>(a.mDrivers) != static_cast<bool>(b.mDrivers)) {
		return false;
	} else if (static_cast<bool>(a.mDrivers) && ((*a.mDrivers) != (*b.mDrivers))) {
		return false;
	}

	if (static_cast<bool>(a.mSynapticInputs) != static_cast<bool>(b.mSynapticInputs)) {
		return false;
	} else if (
	    static_cast<bool>(a.mSynapticInputs) && ((*a.mSynapticInputs) != (*b.mSynapticInputs))) {
		return false;
	}

	if (static_cast<bool>(a.mSynapseRows) != static_cast<bool>(b.mSynapseRows)) {
		return false;
	} else if (static_cast<bool>(a.mSynapseRows) && ((*a.mSynapseRows) != (*b.mSynapseRows))) {
		return false;
	}

	if (static_cast<bool>(a.mAnalogs) != static_cast<bool>(b.mAnalogs)) {
		return false;
	} else if (static_cast<bool>(a.mAnalogs) && ((*a.mAnalogs) != (*b.mAnalogs))) {
		return false;
	}

	if (static_cast<bool>(a.mBackgroundGenerators) != static_cast<bool>(b.mBackgroundGenerators)) {
		return false;
	} else if (
	    static_cast<bool>(a.mBackgroundGenerators) &&
	    ((*a.mBackgroundGenerators) != (*b.mBackgroundGenerators))) {
		return false;
	}

	if (static_cast<bool>(a.mFGBlocks) != static_cast<bool>(b.mFGBlocks)) {
		return false;
	} else if (static_cast<bool>(a.mFGBlocks) && ((*a.mFGBlocks) != (*b.mFGBlocks))) {
		return false;
	}

	if (static_cast<bool>(a.mVRepeaters) != static_cast<bool>(b.mVRepeaters)) {
		return false;
	} else if (static_cast<bool>(a.mVRepeaters) && ((*a.mVRepeaters) != (*b.mVRepeaters))) {
		return false;
	}

	if (static_cast<bool>(a.mHRepeaters) != static_cast<bool>(b.mHRepeaters)) {
		return false;
	} else if (static_cast<bool>(a.mHRepeaters) && ((*a.mHRepeaters) != (*b.mHRepeaters))) {
		return false;
	}

	if (static_cast<bool>(a.mSynapseSwitches) != static_cast<bool>(b.mSynapseSwitches)) {
		return false;
	} else if (
	    static_cast<bool>(a.mSynapseSwitches) && ((*a.mSynapseSwitches) != (*b.mSynapseSwitches))) {
		return false;
	}

	if (static_cast<bool>(a.mCrossbarSwitches) != static_cast<bool>(b.mCrossbarSwitches)) {
		return false;
	} else if (
	    static_cast<bool>(a.mCrossbarSwitches) &&
	    ((*a.mCrossbarSwitches) != (*b.mCrossbarSwitches))) {
		return false;
	}

	if (static_cast<bool>(a.mSynapseSwitchRows) != static_cast<bool>(b.mSynapseSwitchRows)) {
		return false;
	} else if (
	    static_cast<bool>(a.mSynapseSwitchRows) &&
	    ((*a.mSynapseSwitchRows) != (*b.mSynapseSwitchRows))) {
		return false;
	}

	if (static_cast<bool>(a.mSynapseArrays) != static_cast<bool>(b.mSynapseArrays)) {
		return false;
	} else if (
	    static_cast<bool>(a.mSynapseArrays) &&
	    ((*a.mSynapseArrays) != (*b.mSynapseArrays))) {
		return false;
	}

	if (static_cast<bool>(a.mHBuses) != static_cast<bool>(b.mHBuses)) {
		return false;
	} else if (static_cast<bool>(a.mHBuses) && ((*a.mHBuses) != (*b.mHBuses))) {
		return false;
	}

	if (static_cast<bool>(a.mVBuses) != static_cast<bool>(b.mVBuses)) {
		return false;
	} else if (static_cast<bool>(a.mVBuses) && ((*a.mVBuses) != (*b.mVBuses))) {
		return false;
	}

	if (static_cast<bool>(a.mMergers0) != static_cast<bool>(b.mMergers0)) {
		return false;
	} else if (static_cast<bool>(a.mMergers0) && ((*a.mMergers0) != (*b.mMergers0))) {
		return false;
	}

	if (static_cast<bool>(a.mMergers1) != static_cast<bool>(b.mMergers1)) {
		return false;
	} else if (static_cast<bool>(a.mMergers1) && ((*a.mMergers1) != (*b.mMergers1))) {
		return false;
	}
	if (static_cast<bool>(a.mMergers2) != static_cast<bool>(b.mMergers2)) {
		return false;
	} else if (static_cast<bool>(a.mMergers2) && ((*a.mMergers2) != (*b.mMergers2))) {
		return false;
	}

	if (static_cast<bool>(a.mMergers3) != static_cast<bool>(b.mMergers3)) {
		return false;
	} else if (static_cast<bool>(a.mMergers3) && ((*a.mMergers3) != (*b.mMergers3))) {
		return false;
	}

	if (static_cast<bool>(a.mDNCMergers) != static_cast<bool>(b.mDNCMergers)) {
		return false;
	} else if (static_cast<bool>(a.mDNCMergers) && ((*a.mDNCMergers) != (*b.mDNCMergers))) {
		return false;
	}

	return true;
}

bool operator!=(Hicann const& a, Hicann const& b)
{
	return !(a == b);
}

} // resources
} // redman
