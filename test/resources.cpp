#include <gtest/gtest.h>
#include <boost/shared_ptr.hpp>

#include "redman/backend/Backend.h"
#include "redman/resources/Wafer.h"
#include "redman/resources/Hicann.h"
#include "redman/resources/Fpga.h"

using namespace redman;
using namespace redman::backend;
using namespace redman::resources;
namespace HMFC = halco::hicann::v2;

class AWafer : public ::testing::Test {
public:
	Wafer wafer;
};

class AHicann : public ::testing::Test {
public:
	Hicann hicann;
};

class AFpga : public ::testing::Test {
public:
	Fpga fpga;
};

#define fixture_for_hicann_component(TYPE, VAR)                                \
	class Some##TYPE : public AHicann {                                        \
	public:                                                                    \
		Some##TYPE() : VAR(hicann.VAR()) {                                     \
		}                                                                      \
		boost::shared_ptr<components::TYPE> VAR;                               \
	}

fixture_for_hicann_component(Neurons, neurons);
fixture_for_hicann_component(Synapses, synapses);

#undef fixture_for_hicann_component

TEST_F(SomeNeurons, CanBeDisabled) {
	ASSERT_EQ(512, neurons->available());

	auto absent = halco::hicann::v2::NeuronOnHICANN(halco::common::Enum(5));
	neurons->disable(absent);

	ASSERT_EQ(511, neurons->available());
}

TEST_F(SomeSynapses, HaveTheCorrectCount) {
	ASSERT_EQ(HMFC::SynapseRowOnHICANN::end, 2 * 224);
	ASSERT_EQ(HMFC::SynapseColumnOnHICANN::end, 256);
	ASSERT_EQ(2 * 224 * 256, synapses->available());
}

TEST_F(AWafer, ManagesAvailableHicanns) {
	HMFC::HICANNOnWafer hi;
	auto hicanns = wafer.hicanns();
	ASSERT_TRUE(hicanns->has(hi));
	ASSERT_EQ(384, hicanns->available());

	ASSERT_NO_THROW(hicanns->disable(hi));

	ASSERT_FALSE(hicanns->has(hi));
	ASSERT_FALSE(wafer.hicanns()->has(hi));

	ASSERT_ANY_THROW(hicanns->disable(hi));
}

TEST_F(AHicann, CanIntegrateDisabledHLines) {
	auto absent = halco::hicann::v2::HLineOnHICANN(halco::common::Enum(5));

	auto hbuses = hicann.hbuses();
	for (auto it=hbuses->begin_disabled(); it!=hbuses->end_disabled(); ++it) {
		FAIL() << "No HLines should be disabled";
	}

	hbuses->disable(absent);
	ASSERT_FALSE(hbuses->has(absent));
	for (auto it=hbuses->begin_disabled(); it!=hbuses->end_disabled(); ++it) {
		ASSERT_EQ(absent, *it);
		hbuses->enable(*it);
	}
	ASSERT_TRUE(hbuses->has(absent));

}

#define fixture_for_fpga_component(TYPE, VAR)                                                      \
	class Some##TYPE : public AFpga                                                                \
	{                                                                                              \
	public:                                                                                        \
		Some##TYPE() : VAR(fpga.VAR()) {}                                                          \
		boost::shared_ptr<components::TYPE> VAR;                                                   \
	}

fixture_for_fpga_component(HighspeedLinksOnDNC, hslinks);

#undef fixture_for_fpga_component

TEST_F(SomeHighspeedLinksOnDNC, CanBeDisabled) {
	ASSERT_EQ(8, hslinks->available());

	auto absent = halco::hicann::v2::HighspeedLinkOnDNC(halco::common::Enum(5));
	hslinks->disable(absent);

	ASSERT_EQ(7, hslinks->available());
}

TEST_F(SomeHighspeedLinksOnDNC, HaveTheCorrectCount) {
	ASSERT_EQ(HMFC::HighspeedLinkOnDNC::end, 8);
	ASSERT_EQ(8, hslinks->available());
}

TEST_F(AFpga, ManagesAvailableHighspeedLinks) {
	HMFC::HighspeedLinkOnDNC hi;
	auto hslinks = fpga.hslinks();
	ASSERT_TRUE(hslinks->has(hi));
	ASSERT_EQ(8, hslinks->available());

	ASSERT_NO_THROW(hslinks->disable(hi));

	ASSERT_FALSE(hslinks->has(hi));
	ASSERT_FALSE(fpga.hslinks()->has(hi));

	ASSERT_ANY_THROW(hslinks->disable(hi));
}

TEST_F(AFpga, CanIntegrateDisabledHighspeedLinks) {
	auto absent = halco::hicann::v2::HighspeedLinkOnDNC(halco::common::Enum(5));

	auto hslinks = fpga.hslinks();
	for (auto it=hslinks->begin_disabled(); it!=hslinks->end_disabled(); ++it) {
		FAIL() << "No HighspeedLinks should be disabled";
	}

	hslinks->disable(absent);
	ASSERT_FALSE(hslinks->has(absent));
	for (auto it=hslinks->begin_disabled(); it!=hslinks->end_disabled(); ++it) {
		ASSERT_EQ(absent, *it);
		hslinks->enable(*it);
	}
	ASSERT_TRUE(hslinks->has(absent));

}

TEST(IntersectionTest, IntersectionWorks) {
	Hicann hicann_a;
	Hicann hicann_b;

	HMFC::NeuronOnHICANN const nrn_0(halco::common::Enum(0));
	HMFC::NeuronOnHICANN const nrn_1(halco::common::Enum(1));

	hicann_a.neurons()->disable(nrn_0);
	hicann_b.neurons()->disable(nrn_1);

	ASSERT_FALSE(hicann_a.neurons()->has(nrn_0));
	ASSERT_TRUE(hicann_a.neurons()->has(nrn_1));

	hicann_a.intersection(hicann_b);

	ASSERT_FALSE(hicann_a.neurons()->has(nrn_0));
	ASSERT_FALSE(hicann_a.neurons()->has(nrn_1));
}
