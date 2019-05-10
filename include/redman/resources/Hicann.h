#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/shared_ptr.hpp>

#include "redman/resources/Base.h"
#include "redman/resources/components.h"
#include "redman/backend/Backend.h"

namespace redman {
namespace resources {

class Hicann : public Base
{
public:
	Hicann();
	virtual ~Hicann();

	boost::shared_ptr<components::Neurons>        neurons()        { return mNeurons; }
	boost::shared_ptr<components::Synapses>       synapses()       { return mSynapses; }
	boost::shared_ptr<components::SynapseDrivers> drivers()        { return mDrivers; }
	boost::shared_ptr<components::SynapticInputs> synaptic_inputs() { return mSynapticInputs; }
	boost::shared_ptr<components::SynapseRows>       synapserows()       { return mSynapseRows; }
	boost::shared_ptr<components::Analogs>           analogs()           { return mAnalogs; }
	boost::shared_ptr<components::BackgroundGenerators> backgroundgenerators() { return mBackgroundGenerators; }
	boost::shared_ptr<components::FGBlocks>          fgblocks()          { return mFGBlocks; }
	boost::shared_ptr<components::VRepeaters>        vrepeaters()        { return mVRepeaters; }
	boost::shared_ptr<components::HRepeaters>        hrepeaters()        { return mHRepeaters; }
	boost::shared_ptr<components::SynapseSwitches>    synapseswitches()    { return mSynapseSwitches; }
	boost::shared_ptr<components::CrossbarSwitches>   crossbarswitches()   { return mCrossbarSwitches; }
	boost::shared_ptr<components::SynapseSwitchRows> synapseswitchrows() { return mSynapseSwitchRows; }

	boost::shared_ptr<components::HorizontalBuses> hbuses()        { return mHBuses; }
	boost::shared_ptr<components::VerticalBuses>   vbuses()        { return mVBuses; }

	boost::shared_ptr<components::Mergers0>        mergers0()      { return mMergers0; }
	boost::shared_ptr<components::Mergers1>        mergers1()      { return mMergers1; }
	boost::shared_ptr<components::Mergers2>        mergers2()      { return mMergers2; }
	boost::shared_ptr<components::Mergers3>        mergers3()      { return mMergers3; }
	boost::shared_ptr<components::DNCMergers>      dncmergers()    { return mDNCMergers; }

	/** Perform intersection for all components
	 */
	void intersection(Hicann const& other);

#ifndef PYPLUSPLUS
	boost::shared_ptr<components::Neurons const>        neurons()  const { return mNeurons; }
	boost::shared_ptr<components::Synapses const>       synapses() const { return mSynapses; }
	boost::shared_ptr<components::SynapseDrivers const> drivers()  const { return mDrivers; }
	boost::shared_ptr<components::SynapticInputs const> synaptic_inputs() const { return mSynapticInputs; }
	boost::shared_ptr<components::SynapseRows const>       synapserows()       const { return mSynapseRows; }
	boost::shared_ptr<components::Analogs const>           analogs()           const { return mAnalogs; }
	boost::shared_ptr<components::BackgroundGenerators const> backgroundgenerators() const { return mBackgroundGenerators; }
	boost::shared_ptr<components::FGBlocks const>          fgblocks()          const { return mFGBlocks; }
	boost::shared_ptr<components::VRepeaters const>        vrepeaters()        const { return mVRepeaters; }
	boost::shared_ptr<components::HRepeaters const>        hrepeaters()        const { return mHRepeaters; }
	boost::shared_ptr<components::SynapseSwitches const>    synapseswitches()    const { return mSynapseSwitches; }
	boost::shared_ptr<components::CrossbarSwitches const>   crossbarswitches()   const { return mCrossbarSwitches; }
	boost::shared_ptr<components::SynapseSwitchRows const> synapseswitchrows() const { return mSynapseSwitchRows; }

	boost::shared_ptr<components::HorizontalBuses const> hbuses() const { return mHBuses; }
	boost::shared_ptr<components::VerticalBuses const>   vbuses() const { return mVBuses; }

	boost::shared_ptr<components::Mergers0 const>   mergers0()   const { return mMergers0; }
	boost::shared_ptr<components::Mergers1 const>   mergers1()   const { return mMergers1; }
	boost::shared_ptr<components::Mergers2 const>   mergers2()   const { return mMergers2; }
	boost::shared_ptr<components::Mergers3 const>   mergers3()   const { return mMergers3; }
	boost::shared_ptr<components::DNCMergers const> dncmergers() const { return mDNCMergers; }

	void copy(Base const&) override;
#else
	void copy(Base const&);
#endif // PYPLUSPLUS

	// factory function for Py++
	static boost::shared_ptr<Hicann> create();

private:
	boost::shared_ptr<components::Neurons> mNeurons;
	boost::shared_ptr<components::Synapses> mSynapses;
	boost::shared_ptr<components::SynapseDrivers> mDrivers;
	boost::shared_ptr<components::SynapticInputs> mSynapticInputs;
	boost::shared_ptr<components::SynapseRows> mSynapseRows;
	boost::shared_ptr<components::Analogs> mAnalogs;
	boost::shared_ptr<components::BackgroundGenerators> mBackgroundGenerators;
	boost::shared_ptr<components::FGBlocks> mFGBlocks;
	boost::shared_ptr<components::VRepeaters> mVRepeaters;
	boost::shared_ptr<components::HRepeaters> mHRepeaters;
	boost::shared_ptr<components::SynapseSwitches> mSynapseSwitches;
	boost::shared_ptr<components::CrossbarSwitches> mCrossbarSwitches;
	boost::shared_ptr<components::SynapseSwitchRows> mSynapseSwitchRows;

	boost::shared_ptr<components::HorizontalBuses> mHBuses;
	boost::shared_ptr<components::VerticalBuses> mVBuses;

	boost::shared_ptr<components::Mergers0> mMergers0;
	boost::shared_ptr<components::Mergers1> mMergers1;
	boost::shared_ptr<components::Mergers2> mMergers2;
	boost::shared_ptr<components::Mergers3> mMergers3;
	boost::shared_ptr<components::DNCMergers> mDNCMergers;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

class HicannWithBackend : public Hicann {
public:
	typedef HMF::Coordinate::HICANNGlobal id_type;

	HicannWithBackend(
		boost::shared_ptr<backend::Backend>,
		HMF::Coordinate::HICANNGlobal,
		Hicann const& = Hicann(),
		bool ignore_missing = true);
	virtual ~HicannWithBackend();

	id_type id() const { return mId; }

	void save();

	// factory function for Py++
	static boost::shared_ptr<HicannWithBackend> create(
		boost::shared_ptr<backend::Backend> backend,
		HMF::Coordinate::HICANNGlobal id,
		bool ignore_missing = true);

	std::string id_for_backend() const;
private:
	void load(bool ignore_missing);

	id_type mId;
	boost::shared_ptr<backend::Backend> mBackend;

	friend class boost::serialization::access;
#ifndef PYPLUSPLUS
	HicannWithBackend() = default;
#else
	HicannWithBackend();
#endif // PYPLUSPLUS
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

} // resources
} // redman

BOOST_CLASS_EXPORT_KEY(redman::resources::Hicann)
BOOST_CLASS_EXPORT_KEY(redman::resources::HicannWithBackend)

namespace redman {
namespace resources {

template<typename Archiver>
void Hicann::serialize(Archiver& ar, unsigned int const version)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Base);

	ar & make_nvp("neurons", mNeurons);
	ar & make_nvp("synapses", mSynapses);
	ar & make_nvp("drivers", mDrivers);
	if (version > 0) {
		ar & make_nvp("synaptic_inputs", mSynapticInputs);
	}

	ar & make_nvp("hbuses", mHBuses);
	ar & make_nvp("vbuses", mVBuses);

	ar & make_nvp("merger0", mMergers0);
	ar & make_nvp("merger1", mMergers1);
	ar & make_nvp("merger2", mMergers2);
	ar & make_nvp("merger3", mMergers3);
	ar & make_nvp("dnc_merger", mDNCMergers);
	if (version > 1) {
		ar & make_nvp("synapserows", mSynapseRows);
		ar & make_nvp("analogs", mAnalogs);
		ar & make_nvp("backgroundgenerators", mBackgroundGenerators);
		ar & make_nvp("fgblocks", mFGBlocks);
		ar & make_nvp("vrepeaters", mVRepeaters);
		ar & make_nvp("hrepeaters", mHRepeaters);
		ar & make_nvp("synapseswitches", mSynapseSwitches);
		ar & make_nvp("crossbarswitches", mCrossbarSwitches);
		ar & make_nvp("synapseswitchrows", mSynapseSwitchRows);
	}
}

template <typename Archiver>
void HicannWithBackend::serialize(Archiver& ar, unsigned int const version)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Hicann);
	if (version > 0) {
		ar & make_nvp("id", mId);
	}
}

} // resources
} // redman

BOOST_CLASS_VERSION(redman::resources::HicannWithBackend, 1)
BOOST_CLASS_VERSION(redman::resources::Hicann, 2)
