#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>

namespace redman {
namespace resources {

class Base {
public:
	virtual ~Base();

	virtual void copy(Base const&) = 0;

private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver&, unsigned int const) {}
};

} // resources
} // redman

BOOST_CLASS_EXPORT_KEY(redman::resources::Base)
BOOST_SERIALIZATION_ASSUME_ABSTRACT(redman::resources::Base)
