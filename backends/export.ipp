#include <boost/serialization/export.hpp>

#include "redman/resources/Wafer.h"
#include "redman/resources/Hicann.h"

BOOST_CLASS_EXPORT_IMPLEMENT(redman::resources::Base)
BOOST_CLASS_EXPORT_IMPLEMENT(redman::resources::Hicann)
BOOST_CLASS_EXPORT_IMPLEMENT(redman::resources::HicannWithBackend)
BOOST_CLASS_EXPORT_IMPLEMENT(redman::resources::Wafer)
BOOST_CLASS_EXPORT_IMPLEMENT(redman::resources::WaferWithBackend)

#define REDMAN_BACKEND_EXPORT
