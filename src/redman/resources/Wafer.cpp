#include <sstream>

#include <boost/make_shared.hpp>

#include "redman/resources/Wafer.h"

namespace redman {
namespace resources {

Wafer::Wafer(id_type const& id)
	: mId(id),
	  mHicanns(boost::make_shared<components::Hicanns>()),
	  mFpgas(boost::make_shared<components::Fpgas>()) {
}

Wafer::~Wafer() {
}

Wafer::id_type Wafer::id() const
{
	return mId;
}

void Wafer::set_id(id_type const& id)
{
	mId = id;
}

void Wafer::intersection(Wafer const& other) {
	hicanns()->intersection(*other.hicanns());
	fpgas()->intersection(*other.fpgas());
}

void Wafer::copy(Base const& rhs) {
	// copy used when loading via backend, need to manually
	// restore backend we don't want to serialize it
	// restore id as it's not serialized in all versions
	auto const backend = mBackend;
	auto const id = mId;
	*this = dynamic_cast<Wafer const&>(rhs);
	this->mBackend = backend;
	this->set_id(id);
}

boost::shared_ptr<Wafer> Wafer::create(id_type const& id) {
	return boost::make_shared<Wafer>(id);
}

bool Wafer::has(halco::hicann::v2::HICANNOnWafer const& h) const
{
	return mHicanns->has(h);
}

boost::shared_ptr<components::Hicanns> Wafer::hicanns() {
	return mHicanns;
}

boost::shared_ptr<components::Hicanns const> Wafer::hicanns() const {
	return mHicanns;
}

bool Wafer::has(halco::hicann::v2::FPGAOnWafer const& f) const
{
	return mFpgas->has(f);
}

Wafer::backend_behavior::backend_behavior(bool iwm, bool ihm, bool ifm)
    : ignore_wafer_missing(iwm), ignore_hicann_missing(ihm), ignore_fpga_missing(ifm)
{}

Wafer::get_behavior::get_behavior(bool ihm, bool ifm)
    : ignore_hicann_missing(ihm), ignore_fpga_missing(ifm)
{}

void Wafer::set_backend(
    id_type const& id,
    boost::shared_ptr<backend::Backend> backend,
    backend_behavior const& behavior)
{
	mId = id;
	mBackend = backend;
	mBackendBehavior = behavior;
}

void Wafer::set_get_behavior(get_behavior const& behavior)
{
	mGetBehavior = behavior;
}

void Wafer::set_backend_behavior(backend_behavior const& behavior)
{
	mBackendBehavior = behavior;
}

boost::shared_ptr<components::Fpgas> Wafer::fpgas() {
	return mFpgas;
}

boost::shared_ptr<components::Fpgas const> Wafer::fpgas() const {
	return mFpgas;
}

boost::shared_ptr<Hicann> Wafer::get(halco::hicann::v2::HICANNOnWafer const& h) const
{
	if (!mGetBehavior.ignore_hicann_missing && !has(h)) {
		return boost::shared_ptr<Hicann>();
	}

	auto it = mHicannResourceCache.find(h);
	if (it != mHicannResourceCache.end()) {
		return it->second;
	}

	if(mBackend) {
		halco::hicann::v2::HICANNGlobal gh(h, mId);
		auto tmp = HicannWithBackend(mBackend, gh, Hicann(), mBackendBehavior.ignore_hicann_missing);
		auto res = boost::make_shared<Hicann>(tmp);
		mHicannResourceCache.insert({h, res});
		return res;
	}
	auto res = boost::make_shared<Hicann>();
	mHicannResourceCache.insert({h, res});
	return res;
}

boost::shared_ptr<Fpga> Wafer::get(halco::hicann::v2::FPGAOnWafer const& f) const
{
	if (!mGetBehavior.ignore_fpga_missing && !has(f)) {
		return boost::shared_ptr<Fpga>();
	}

	auto it = mFpgaResourceCache.find(f);
	if (it != mFpgaResourceCache.end()) {
		return it->second;
	}

	if (mBackend) {
		halco::hicann::v2::FPGAGlobal gh(f, mId);
		auto tmp = FpgaWithBackend(mBackend, gh, Fpga(), mBackendBehavior.ignore_fpga_missing);
		auto res = boost::make_shared<Fpga>(tmp);
		mFpgaResourceCache.insert({f, res});
		return res;
	}
	auto res = boost::make_shared<Fpga>();
	mFpgaResourceCache.insert({f, res});
	return res;
}

void Wafer::inject(halco::hicann::v2::HICANNOnWafer const& h, boost::shared_ptr<Hicann> res)
{
	mHicannResourceCache[h] = res;
}

void Wafer::inject(halco::hicann::v2::FPGAOnWafer const& h, boost::shared_ptr<Fpga> res)
{
	mFpgaResourceCache[h] = res;
}

void Wafer::load()
{
	if (!mBackend)
		throw std::runtime_error("Wafer needs backend to load data.");

	Wafer other;
	std::stringstream id;
	id << "wafer-" << mId;

	try {
		mBackend->load(id.str(), *this);
	} catch (const backend::not_found_error&) {
		if (!mBackendBehavior.ignore_wafer_missing) {
			throw;
		}
	}
}

void Wafer::save()
{
	if (!mBackend)
		throw std::runtime_error("Wafer needs backend to commit data.");

	std::stringstream id;
	id << "wafer-" << mId;
	mBackend->store(id.str(), *this);
}

WaferWithBackend::WaferWithBackend(
    boost::shared_ptr<backend::Backend> backend,
    id_type const& id,
    bool ignore_missing)
{
	backend_behavior const behavior {ignore_missing, ignore_missing, ignore_missing};
	this->set_backend(id, backend, behavior);
	load();
}

boost::shared_ptr<WaferWithBackend> WaferWithBackend::create(
       boost::shared_ptr<backend::Backend> backend,
       id_type const& id,
       bool ignore_missing)
{
	auto wwb = boost::make_shared<WaferWithBackend>(backend, id, ignore_missing);
	return wwb;
}

WaferWithBackend::~WaferWithBackend() {
}

bool operator==(Wafer const& a, Wafer const& b)
{
	if (static_cast<bool>(a.mHicanns) != static_cast<bool>(b.mHicanns)) {
		return false;
	} else if (static_cast<bool>(a.mHicanns) && ((*a.mHicanns) != (*b.mHicanns))) {
		return false;
	}

	if (static_cast<bool>(a.mFpgas) != static_cast<bool>(b.mFpgas)) {
		return false;
	} else if (static_cast<bool>(a.mFpgas) && ((*a.mFpgas) != (*b.mFpgas))) {
		return false;
	}

	if (static_cast<bool>(a.mBackend) != static_cast<bool>(b.mBackend)) {
		return false;
	} else if (static_cast<bool>(a.mBackend) && ((*a.mBackend) != (*b.mBackend))) {
		return false;
	}

	return true;
}

bool operator!=(Wafer const& a, Wafer const& b)
{
	return !(a == b);
}

} // resources
} // redman
