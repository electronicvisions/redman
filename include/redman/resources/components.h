#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "redman/ResourceManager.h"
#include "redman/Whitelist.h"

#include "halco/hicann/v2/external.h"
#include "halco/hicann/v2/fg.h"
#include "halco/hicann/v2/hicann.h"
#include "halco/hicann/v2/l1.h"
#include "halco/hicann/v2/synapse.h"
#include "halco/hicann/v2/merger0onhicann.h"
#include "halco/hicann/v2/merger1onhicann.h"
#include "halco/hicann/v2/merger2onhicann.h"
#include "halco/hicann/v2/merger3onhicann.h"

namespace redman {

namespace resources {

namespace components {

namespace detail {

template <typename Derived, typename Resource, typename Policy,
          typename Predicate = DefaultPredicate<Resource>,
          typename Compare = std::less<Resource> >
class ResourceWithFactory
    : public ResourceManager<Resource, Policy, Predicate, Compare> {
	// factory function for Py++
	static boost::shared_ptr<Derived> create() {
		return boost::make_shared<Derived>();
	}
};

} // detail

} // components

} // resources

} // redman

#define ADD_RESOURCE(name, res, version)									  \
	namespace redman { namespace resources { namespace components {		      \
	class name : public detail::ResourceWithFactory<name, res, Blacklist> {}; \
	} } }																      \
	BOOST_CLASS_VERSION(redman::resources::components::name, version)

ADD_RESOURCE(Hicanns, halco::hicann::v2::HICANNOnWafer, 1)

ADD_RESOURCE(Neurons, halco::hicann::v2::NeuronOnHICANN, 1)

ADD_RESOURCE(Synapses, halco::hicann::v2::SynapseOnHICANN, 1)
ADD_RESOURCE(SynapseDrivers, halco::hicann::v2::SynapseDriverOnHICANN, 1)
ADD_RESOURCE(SynapticInputs, halco::hicann::v2::SynapticInputOnHICANN, 1)
ADD_RESOURCE(SynapseRows, halco::hicann::v2::SynapseRowOnHICANN, 1)
ADD_RESOURCE(Analogs, halco::hicann::v2::AnalogOnHICANN, 1)
ADD_RESOURCE(BackgroundGenerators, halco::hicann::v2::BackgroundGeneratorOnHICANN, 1)
ADD_RESOURCE(FGBlocks, halco::hicann::v2::FGBlockOnHICANN, 1)
ADD_RESOURCE(VRepeaters, halco::hicann::v2::VRepeaterOnHICANN, 1)
ADD_RESOURCE(HRepeaters, halco::hicann::v2::HRepeaterOnHICANN, 1)
ADD_RESOURCE(SynapseSwitches, halco::hicann::v2::SynapseSwitchOnHICANN, 1)
ADD_RESOURCE(CrossbarSwitches, halco::hicann::v2::CrossbarSwitchOnHICANN, 1)
ADD_RESOURCE(SynapseSwitchRows, halco::hicann::v2::SynapseSwitchRowOnHICANN, 1)
ADD_RESOURCE(SynapseArrays, halco::hicann::v2::SynapseArrayOnHICANN, 1)

ADD_RESOURCE(HorizontalBuses, halco::hicann::v2::HLineOnHICANN, 1)
ADD_RESOURCE(VerticalBuses, halco::hicann::v2::VLineOnHICANN, 1)

ADD_RESOURCE(Mergers0, halco::hicann::v2::Merger0OnHICANN, 1)
ADD_RESOURCE(Mergers1, halco::hicann::v2::Merger1OnHICANN, 1)
ADD_RESOURCE(Mergers2, halco::hicann::v2::Merger2OnHICANN, 1)
ADD_RESOURCE(Mergers3, halco::hicann::v2::Merger3OnHICANN, 1)
ADD_RESOURCE(DNCMergers, halco::hicann::v2::DNCMergerOnHICANN, 1)

ADD_RESOURCE(HighspeedLinksOnDNC, halco::hicann::v2::HighspeedLinkOnDNC, 1)
ADD_RESOURCE(Fpgas, halco::hicann::v2::FPGAOnWafer, 1)

#undef ADD_RESOURCE
