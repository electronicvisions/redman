#include "test/test_with_backend.h"

#include <boost/make_shared.hpp>

#include "redman/resources/Wafer.h"
#include "redman/resources/Hicann.h"

using namespace redman;
using namespace redman::resources;
namespace HMFC = HMF::Coordinate;
TYPED_TEST_CASE(TestWithBackend, BackendTypes);

template <typename T>
class AWaferWithBackend : public TestWithBackend<T> {
public:
	AWaferWithBackend()
	    : wafer(TestWithBackend<T>::backend, wafer_coordinate) {
	}
	HMFC::Wafer wafer_coordinate{0};
	WaferWithBackend wafer;
};
TYPED_TEST_CASE(AWaferWithBackend, BackendTypes);

TYPED_TEST(AWaferWithBackend, PersistsDisabledHicanns) {
	auto& wafer = TestFixture::wafer;
	HMFC::HICANNOnWafer hi;
	if (wafer.hicanns()->has(hi))
		wafer.hicanns()->disable(hi);
	wafer.save();

	const WaferWithBackend cwafer(TestFixture::backend, TestFixture::wafer_coordinate);
	ASSERT_FALSE(cwafer.hicanns()->has(hi));
}

TYPED_TEST(AWaferWithBackend, LoadsHicanns) {
	auto& wafer = TestFixture::wafer;
	auto hicanns = wafer.hicanns();
	HMFC::HICANNOnWafer hi;
	if (!hicanns->has(hi))
		hicanns->enable(hi);
	wafer.save();

	boost::shared_ptr<Hicann> hicann = wafer.get(hi);
	ASSERT_TRUE(static_cast<bool>(hicann));

	boost::shared_ptr<components::Neurons> nrns = hicann->neurons();
	auto absent = HMF::Coordinate::NeuronOnHICANN(geometry::Enum(5));

	if (nrns->has(absent))
		nrns->disable(absent);

	// Hicanns are cached / lazy-loaded

	boost::shared_ptr<const Hicann> chicann = wafer.get(hi);
	auto const& cnrns = chicann->neurons();
	ASSERT_FALSE(cnrns->has(absent));
}

TYPED_TEST(AWaferWithBackend, DoesNotLoadDisabledHicann) {
	auto& wafer = TestFixture::wafer;
	auto hicanns = wafer.hicanns();

	HMFC::HICANNOnWafer hi;

	if (hicanns->has(hi))
		hicanns->disable(hi);

	boost::shared_ptr<Hicann> hicann = wafer.get(hi);
	ASSERT_FALSE(static_cast<bool>(hicann));
}

TYPED_TEST(AWaferWithBackend, AllowsToInjectHicannResources) {
	auto& wafer = TestFixture::wafer;

	boost::shared_ptr<Hicann> hicann = boost::make_shared<Hicann>();
	auto absent = HMF::Coordinate::NeuronOnHICANN(geometry::Enum(5));
	hicann->neurons()->disable(absent);

	HMFC::HICANNOnWafer hi;
	wafer.inject(hi, hicann);
	auto loaded = wafer.get(hi);
	ASSERT_FALSE(loaded->neurons()->has(absent));
}
