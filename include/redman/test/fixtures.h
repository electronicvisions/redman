#pragma once

#include <algorithm>
#include <stdexcept>
#include <gtest/gtest.h>
#include <boost/operators.hpp>

#include "redman/ResourceManager.h"

struct TestResource : public boost::operators<TestResource> {
	typedef unsigned value_type;
	enum : value_type {
		end = 1338,
		begin = 2,
		size = end - begin
	};

	TestResource(value_type idx);
	bool operator<(TestResource const& other) const;
	bool operator==(TestResource const& other) const;
	TestResource& operator++();
	TestResource& operator+=(TestResource const& other);

	value_type index;

	friend std::ostream& operator<<(std::ostream& os, TestResource const& tr) {
		return os << tr.index;
	}
};

struct LessThanPredicate : public redman::DefaultPredicate<TestResource> {
	LessThanPredicate(size_t _lt) : lt(_lt) {}

	bool operator()(resource_type const& res) const {
		return res.index < lt;
	}

	size_t lt;
};

struct EvenPredicate : public redman::DefaultPredicate<TestResource> {
	bool operator()(resource_type const& res) const {
		return !(res.index % 2);
	}
};

typedef ::testing::Types<redman::Whitelist, redman::Blacklist> PolicyTypes;

template <typename T>
struct AManager : public ::testing::Test {
	redman::ResourceManager<TestResource, T> manager;
};

template <typename T>
struct AManagerWithEvenPredicate : public ::testing::Test {
	redman::ResourceManager<TestResource, T, EvenPredicate> manager;
};
