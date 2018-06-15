#include "test/fixtures.h"

using namespace redman;

TEST(ACustomPredicate, CanBeIteratedOver) {
	EvenPredicate pred;
	size_t count = 0;

	for (TestResource res : pred) {
		EXPECT_LE(TestResource(TestResource::begin), res);
		EXPECT_GE(TestResource(TestResource::end), res);
		ASSERT_TRUE(pred(res));
		++count;
	}

	EXPECT_EQ(TestResource::size / 2, count);
}

TEST(APredicate, CanBeIteratedOver) {
	using namespace redman;
	DefaultPredicate<TestResource> pred;
	size_t count = 0;

	for (TestResource res : pred) {
		EXPECT_LE(TestResource(TestResource::begin), res);
		EXPECT_GE(TestResource(TestResource::end), res);
		ASSERT_TRUE(pred(res));
		++count;
	}

	EXPECT_EQ(TestResource::size, count);
}

TEST(APredicate, ProvidesRangeBasedForViaADL) {
	// Check that ADL works for predicate begin/end free functions
	redman::DefaultPredicate<TestResource> pred;

	for (TestResource res : pred) {
		ASSERT_TRUE(pred(res));
	}

	EXPECT_NE(begin(pred), end(pred));
}
