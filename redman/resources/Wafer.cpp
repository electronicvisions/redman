#include <sstream>

#include <boost/make_shared.hpp>

#include "redman/resources/Wafer.h"

namespace redman {
namespace resources {

Wafer::Wafer()
	: mHicanns(boost::make_shared<components::Hicanns>()) {
}

Wafer::~Wafer() {
}

void Wafer::copy(Base const& rhs) {
	*this = dynamic_cast<Wafer const&>(rhs);
}

boost::shared_ptr<Wafer> Wafer::create() {
	return boost::make_shared<Wafer>();
}

bool Wafer::has(HMF::Coordinate::HICANNOnWafer const& h) const
{
	return mHicanns->has(h);
}

boost::shared_ptr<components::Hicanns> Wafer::hicanns() {
	return mHicanns;
}

boost::shared_ptr<components::Hicanns const> Wafer::hicanns() const {
	return mHicanns;
}

WaferWithBackend::WaferWithBackend(
		boost::shared_ptr<backend::Backend> backend,
		HMF::Coordinate::Wafer id,
		bool ignore_missing)
	: mId(id), mBackend(backend) {
	load(ignore_missing);
}

boost::shared_ptr<WaferWithBackend> WaferWithBackend::create(
	boost::shared_ptr<backend::Backend> backend,
	HMF::Coordinate::Wafer id,
	bool ignore_missing)
{
	return boost::make_shared<WaferWithBackend>(backend, id, ignore_missing);
}

WaferWithBackend::~WaferWithBackend() {
}

boost::shared_ptr<Hicann> WaferWithBackend::get(HMF::Coordinate::HICANNOnWafer const& h) const
{
	if (!mBackend)
		throw std::runtime_error("Wafer needs backend to load HICANN.");

	if (!has(h)) {
		return boost::shared_ptr<Hicann>();
	}

	auto it = mCache.find(h);
	if (it != mCache.end()) {
		return it->second;
	}

	HMF::Coordinate::HICANNGlobal gh(h, mId);
	auto tmp = HicannWithBackend(mBackend, gh);
	auto res = boost::make_shared<Hicann>(tmp);
	mCache.insert({h, res});
	return res;
}

void WaferWithBackend::inject(HMF::Coordinate::HICANNOnWafer const& h, boost::shared_ptr<Hicann> res)
{
	mCache[h] = res;
}

void WaferWithBackend::load(bool ignore_missing)
{
	if (!mBackend)
		throw std::runtime_error("Wafer needs backend to load data.");

	Wafer other;
	std::stringstream id;
	id << "wafer-" << mId;

	try {
		mBackend->load(id.str(), *this);
	} catch (const backend::not_found_error&) {
		if (!ignore_missing) {
			throw;
		}
	}
}

void WaferWithBackend::save()
{
	if (!mBackend)
		throw std::runtime_error("Wafer needs backend to commit data.");

	std::stringstream id;
	id << "wafer-" << mId;
	mBackend->store(id.str(), *this);
}

} //resources
} // redman
