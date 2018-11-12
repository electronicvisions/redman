#include <gtest/gtest.h>
#include <boost/shared_ptr.hpp>

#include "redman/backend/Backend.h"
#include "redman/resources/Wafer.h"
#include "redman/resources/Hicann.h"
#include "redman/resources/Fpga.h"

using namespace redman;
using namespace redman::backend;
using namespace redman::resources;
namespace HMFC = HMF::Coordinate;

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

	auto absent = HMF::Coordinate::NeuronOnHICANN(geometry::Enum(5));
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
	auto absent = HMF::Coordinate::HLineOnHICANN(geometry::Enum(5));

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

fixture_for_fpga_component(HicannsOnHS, hicanns);

#undef fixture_for_fpga_component

TEST_F(SomeHicannsOnHS, CanBeDisabled) {
	ASSERT_EQ(8, hicanns->available());

	auto absent = HMF::Coordinate::HICANNOnHS(geometry::Enum(5));
	hicanns->disable(absent);

	ASSERT_EQ(7, hicanns->available());
}

TEST_F(SomeHicannsOnHS, HaveTheCorrectCount) {
	ASSERT_EQ(HMFC::HICANNOnHS::end, 8);
	ASSERT_EQ(8, hicanns->available());
}

TEST_F(AFpga, ManagesAvailableHicanns) {
	HMFC::HICANNOnHS hi;
	auto hicanns = fpga.hicanns();
	ASSERT_TRUE(hicanns->has(hi));
	ASSERT_EQ(8, hicanns->available());

	ASSERT_NO_THROW(hicanns->disable(hi));

	ASSERT_FALSE(hicanns->has(hi));
	ASSERT_FALSE(fpga.hicanns()->has(hi));

	ASSERT_ANY_THROW(hicanns->disable(hi));
}

TEST_F(AFpga, CanIntegrateDisabledHicanns) {
	auto absent = HMF::Coordinate::HICANNOnHS(geometry::Enum(5));

	auto hicanns = fpga.hicanns();
	for (auto it=hicanns->begin_disabled(); it!=hicanns->end_disabled(); ++it) {
		FAIL() << "No HICANNs should be disabled";
	}

	hicanns->disable(absent);
	ASSERT_FALSE(hicanns->has(absent));
	for (auto it=hicanns->begin_disabled(); it!=hicanns->end_disabled(); ++it) {
		ASSERT_EQ(absent, *it);
		hicanns->enable(*it);
	}
	ASSERT_TRUE(hicanns->has(absent));

}
