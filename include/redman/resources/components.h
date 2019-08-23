#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "redman/ResourceManager.h"
#include "redman/Whitelist.h"

#include "hal/Coordinate/HMFGeometry.h"
#include "hal/Coordinate/Merger0OnHICANN.h"
#include "hal/Coordinate/Merger1OnHICANN.h"
#include "hal/Coordinate/Merger2OnHICANN.h"
#include "hal/Coordinate/Merger3OnHICANN.h"

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

ADD_RESOURCE(Hicanns, HMF::Coordinate::HICANNOnWafer, 1)

ADD_RESOURCE(Neurons, HMF::Coordinate::NeuronOnHICANN, 1)

ADD_RESOURCE(Synapses, HMF::Coordinate::SynapseOnHICANN, 1)
ADD_RESOURCE(SynapseDrivers, HMF::Coordinate::SynapseDriverOnHICANN, 1)
ADD_RESOURCE(SynapticInputs, HMF::Coordinate::SynapticInputOnHICANN, 1)

ADD_RESOURCE(HorizontalBuses, HMF::Coordinate::HLineOnHICANN, 1)
ADD_RESOURCE(VerticalBuses, HMF::Coordinate::VLineOnHICANN, 1)

ADD_RESOURCE(Mergers0, HMF::Coordinate::Merger0OnHICANN, 1)
ADD_RESOURCE(Mergers1, HMF::Coordinate::Merger1OnHICANN, 1)
ADD_RESOURCE(Mergers2, HMF::Coordinate::Merger2OnHICANN, 1)
ADD_RESOURCE(Mergers3, HMF::Coordinate::Merger3OnHICANN, 1)
ADD_RESOURCE(DNCMergers, HMF::Coordinate::DNCMergerOnHICANN, 1)

ADD_RESOURCE(HighspeedLinksOnDNC, HMF::Coordinate::HighspeedLinkOnDNC, 1)
ADD_RESOURCE(Fpgas, HMF::Coordinate::FPGAOnWafer, 1)

#undef ADD_RESOURCE
