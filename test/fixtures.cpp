#include "redman/test/fixtures.h"

TestResource::TestResource(value_type idx) : index(idx) {
	if (idx < begin || idx > end)
		throw std::invalid_argument("Index out of bounds.");
}

bool TestResource::operator<(TestResource const& other) const {
	return index < other.index;
}

bool TestResource::operator==(TestResource const& other) const {
	return index == other.index;
}

TestResource& TestResource::operator++() {
	++index;
	return *this;
}

TestResource& TestResource::operator+=(TestResource const& other) {
	index += other.index;
	return *this;
}
