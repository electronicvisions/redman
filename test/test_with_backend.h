#pragma once

#include <gtest/gtest.h>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/make_shared.hpp>

#include "redman/backend/Library.h"
#include "redman/backend/Backend.h"
#include "redman/backend/MockBackend.h"

boost::shared_ptr<redman::backend::Backend> init_backend(
    std::string const& fname);

template <typename T>
class TestWithBackend : public ::testing::Test {};
struct XMLBackend {};
struct MockBackend {};

template <>
class TestWithBackend<XMLBackend> : public ::testing::Test {
public:
	static void SetUpTestCase() {
		backend = init_backend("libredman_xml.so");
		ASSERT_TRUE(static_cast<bool>(backend));

		backendPath = boost::filesystem::unique_path();
		boost::filesystem::create_directories(backendPath);
		backend->config("path", backendPath.native());
		backend->init();
	}

	static void TearDownTestCase()
	{
		boost::filesystem::remove_all(backendPath);
		backendPath.clear();
	}

	static boost::shared_ptr<redman::backend::Backend> backend;
	static boost::filesystem::path backendPath;
};

class TestWithMockBackend : public ::testing::Test {
public:
	TestWithMockBackend()
		: backend(boost::make_shared<redman::backend::MockBackend>()) {}
	boost::shared_ptr<redman::backend::Backend> backend;
};

template <>
class TestWithBackend<MockBackend> : public TestWithMockBackend {};
typedef ::testing::Types<XMLBackend, MockBackend> BackendTypes;
