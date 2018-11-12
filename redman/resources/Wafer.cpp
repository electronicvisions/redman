#include <sstream>

#include <boost/make_shared.hpp>

#include "redman/resources/Wafer.h"

namespace redman {
namespace resources {

Wafer::Wafer()
	: mHicanns(boost::make_shared<components::Hicanns>()),
	  mFpgas(boost::make_shared<components::Fpgas>()) {
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

bool Wafer::has(HMF::Coordinate::FPGAOnWafer const& f) const
{
	return mFpgas->has(f);
}

boost::shared_ptr<components::Fpgas> Wafer::fpgas() {
	return mFpgas;
}

boost::shared_ptr<components::Fpgas const> Wafer::fpgas() const {
	return mFpgas;
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

	auto it = mHicannCache.find(h);
	if (it != mHicannCache.end()) {
		return it->second;
	}

	HMF::Coordinate::HICANNGlobal gh(h, mId);
	auto tmp = HicannWithBackend(mBackend, gh);
	auto res = boost::make_shared<Hicann>(tmp);
	mHicannCache.insert({h, res});
	return res;
}

void WaferWithBackend::inject(HMF::Coordinate::HICANNOnWafer const& h, boost::shared_ptr<Hicann> res)
{
	mHicannCache[h] = res;
}

boost::shared_ptr<Fpga> WaferWithBackend::get(HMF::Coordinate::FPGAOnWafer const& h) const
{
	if (!mBackend)
		throw std::runtime_error("Wafer needs backend to load FPGA.");

	if (!has(h)) {
		return boost::shared_ptr<Fpga>();
	}

	auto it = mFpgaCache.find(h);
	if (it != mFpgaCache.end()) {
		return it->second;
	}

	HMF::Coordinate::FPGAGlobal gh(h, mId);
	auto tmp = FpgaWithBackend(mBackend, gh);
	auto res = boost::make_shared<Fpga>(tmp);
	mFpgaCache.insert({h, res});
	return res;
}

void WaferWithBackend::inject(HMF::Coordinate::FPGAOnWafer const& h, boost::shared_ptr<Fpga> res)
{
	mFpgaCache[h] = res;
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
