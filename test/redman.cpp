#include "redman/test/fixtures.h"

TYPED_TEST_SUITE(AManager, PolicyTypes);
TYPED_TEST_SUITE(AManagerWithEvenPredicate, PolicyTypes);

using namespace redman;

TYPED_TEST(AManagerWithEvenPredicate, ThrowsForInvalidResources) {
	auto& manager = TestFixture::manager;
	manager.enable_all();
	EXPECT_ANY_THROW(manager.has(1));
	EXPECT_ANY_THROW(manager.disable(1));
	EXPECT_ANY_THROW(manager.enable(1));
}

TYPED_TEST(AManagerWithEvenPredicate, HasHalfTheNumberOfResources) {
	auto& manager = TestFixture::manager;
	manager.enable_all();
	ASSERT_EQ(TestResource::size / 2, manager.available());
}

TYPED_TEST(AManagerWithEvenPredicate, DoesNotThrowForValidResources) {
	auto& manager = TestFixture::manager;
	manager.enable_all();
	EXPECT_NO_THROW(manager.has(2));
	EXPECT_NO_THROW(manager.disable(2));
	EXPECT_NO_THROW(manager.enable(2));
}

TYPED_TEST(AManagerWithEvenPredicate, ThrowsWhenProvidedWithInvalidSet) {
	auto& manager = TestFixture::manager;
	std::set<TestResource> resources{2, 4, 8};
	ASSERT_NO_THROW(manager.from_set(resources));
	resources.insert(3);
	ASSERT_ANY_THROW(manager.from_set(resources));
}

TYPED_TEST(AManager, StartsDisabledOrEnabledBasedOnPolicy) {
	auto& manager = TestFixture::manager;
	auto avail = manager.available();
	ASSERT_TRUE(avail == 0 || avail == TestResource::size);
}

TYPED_TEST(AManager, CanDisableAllResources) {
	auto& manager = TestFixture::manager;
	manager.disable_all();
	ASSERT_EQ(0, manager.available());
}

TYPED_TEST(AManager, CanEnableAllResources) {
	auto& manager = TestFixture::manager;
	manager.enable_all();
	ASSERT_EQ(TestResource::size, manager.available());
}

TYPED_TEST(AManager, CanCheckSingleResources) {
	auto& manager = TestFixture::manager;
	manager.enable_all();

	ASSERT_TRUE(manager.has(2));
}

TYPED_TEST(AManager, CanDisableSingleResources) {
	auto& manager = TestFixture::manager;
	manager.enable_all();
	manager.disable(4);
	manager.disable(7);
	manager.disable(22);
	ASSERT_EQ(TestResource::size - 3, manager.available());
	ASSERT_FALSE(manager.has(4));
}

TYPED_TEST(AManager, CanEnableSingleResources) {
	auto& manager = TestFixture::manager;
	manager.disable_all();
	manager.enable(4);
	manager.enable(7);
	manager.enable(22);
	ASSERT_EQ(3, manager.available());
	ASSERT_TRUE(manager.has(4));
	ASSERT_FALSE(manager.has(8));
}

TYPED_TEST(AManager, ThrowsWhenEnablingAResourceThatIsEnabled) {
	auto& manager = TestFixture::manager;
	manager.enable_all();

	ASSERT_ANY_THROW(manager.enable(8));
}

TYPED_TEST(AManager, ThrowsWhenDisablingAResourceThatIsDisabled) {
	auto& manager = TestFixture::manager;
	manager.disable_all();

	ASSERT_ANY_THROW(manager.disable(7));
}

TYPED_TEST(AManager, DoesNotThrowWhenForcingEnablingAResourceThatIsEnabled) {
	auto& manager = TestFixture::manager;
	manager.enable_all();

	ASSERT_NO_THROW(manager.enable(8, switch_mode::NONTHROW));
}

TYPED_TEST(AManager, DoesNotThrowWhenForcingDisablingAResourceThatIsDisabled) {
	auto& manager = TestFixture::manager;
	manager.disable_all();

	ASSERT_NO_THROW(manager.disable(7, switch_mode::NONTHROW));
}

TYPED_TEST(AManager, AllowsIterationOverEnabledResources) {
	auto& manager = TestFixture::manager;
	manager.enable_all();

	std::set<TestResource> set_from_iterator;

	manager.disable(33);
	manager.disable(324);

	std::copy(manager.begin(), manager.end(),
	          std::inserter(set_from_iterator, set_from_iterator.begin()));
	ASSERT_EQ(TestResource::size - 2, set_from_iterator.size());
	ASSERT_EQ(0, set_from_iterator.count(33));
	ASSERT_EQ(1, set_from_iterator.count(31));
}

TYPED_TEST(AManager, AllowsIterationOverDisabledResources) {
	auto& manager = TestFixture::manager;
	manager.enable_all();

	std::set<TestResource> set_from_iterator;

	manager.disable(33);
	manager.disable(324);

	std::copy(manager.begin_disabled(), manager.end_disabled(),
	          std::inserter(set_from_iterator, set_from_iterator.begin()));
	ASSERT_EQ(2, set_from_iterator.size());
	ASSERT_EQ(1, set_from_iterator.count(33));
	ASSERT_EQ(0, set_from_iterator.count(31));
}

TYPED_TEST(AManager, AllowsRangeBasedForOverEnabledResources) {
	auto& manager = TestFixture::manager;
	manager.enable_all();

	manager.disable(33);
	manager.disable(324);

	for (auto res : manager) {
		if (!manager.has(res))
			FAIL() << "Disabled resource used in iteration.";
	}
}

template <typename T>
struct TwoManagers : public ::testing::Test {
	TwoManagers() {
		manager.enable_all();
		other.enable_all();

		manager.disable(33);
		manager.disable(324);

		other.disable(55);
		other.disable(324);
	}

	redman::ResourceManager<TestResource, T> manager;
	redman::ResourceManager<TestResource, T> other;
};

TYPED_TEST_SUITE(TwoManagers, PolicyTypes);

TYPED_TEST(TwoManagers, CanBeMerged) {
	auto& manager = TestFixture::manager;
	auto& other = TestFixture::other;

	manager.merge(other);
	ASSERT_FALSE(manager.has(324));
	ASSERT_EQ(TestResource::size - 1, manager.available());
}

TYPED_TEST(TwoManagers, CanBeIntersected) {
	auto& manager = TestFixture::manager;
	auto& other = TestFixture::other;

	manager.intersection(other);
	ASSERT_FALSE(manager.has(33));
	ASSERT_FALSE(manager.has(55));
	ASSERT_FALSE(manager.has(324));
	ASSERT_EQ(TestResource::size - 3, manager.available());
}

TYPED_TEST(TwoManagers, CanBeDiffed) {
	auto& manager = TestFixture::manager;
	auto& other = TestFixture::other;

	manager.difference(other);
	ASSERT_TRUE(manager.has(55));
	ASSERT_EQ(1, manager.available());
}

TYPED_TEST(TwoManagers, CanBeDiffedSymmetrically) {
	auto& manager = TestFixture::manager;
	auto& other = TestFixture::other;

	manager.symmetric_difference(other);
	ASSERT_TRUE(manager.has(33));
	ASSERT_TRUE(manager.has(55));
	ASSERT_EQ(2, manager.available());
}

template<typename T>
class TwoParameterizedManagers : public ::testing::Test {
public:
	TwoParameterizedManagers()
		: lt200(LessThanPredicate(200)), lt300(LessThanPredicate(300)) {
		lt200.enable_all();
		lt300.enable_all();
	}

	redman::ResourceManager<TestResource, T, LessThanPredicate> lt200;
	redman::ResourceManager<TestResource, T, LessThanPredicate> lt300;
};

TYPED_TEST_SUITE(TwoParameterizedManagers, PolicyTypes);

TYPED_TEST(TwoParameterizedManagers, CanBeMergedEvenIfThisThrowsAwayElements) {
	auto& lt200 = TestFixture::lt200;
	auto& lt300 = TestFixture::lt300;

	lt200.disable_all();
	ASSERT_NO_THROW(lt200.merge(lt300));
	ASSERT_LT(0, lt200.available());
	ASSERT_LT(lt200.available(), lt300.available());

	// But from_set() will enforce that ALL resources in the set match
	// the predicate and thus throw:

	std::set<TestResource> resources;
	std::copy(lt300.begin(), lt300.end(),
	          std::inserter(resources, resources.begin()));

	ASSERT_ANY_THROW(lt200.from_set(resources));
}
