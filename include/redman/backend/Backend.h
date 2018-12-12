#pragma once

#include <stdexcept>
#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

namespace redman {

namespace resources {
class Base;
} // resources

namespace backend {

class not_found_error : public std::exception
{
public:
	not_found_error(std::string const& msg);
	virtual ~not_found_error() throw();

	virtual const char* what() const throw();

private:
	std::string mMsg;
};

class Library;
class Backend
{
public:
	virtual ~Backend() {}

	void config(std::string const& key,
	            std::string const& val);

	void config(std::string const& key,
	            int const val);

	virtual void init() = 0;

	virtual void load(std::string const& id, resources::Base&) = 0;

	virtual void store(std::string const& id, resources::Base const&) = 0;

protected:
	typedef boost::variant<std::string, int> config_value_t;
	typedef std::map<std::string, config_value_t> config_map_t;

	void config_(std::string const& key,
	             config_value_t const& val);

	bool exists(std::string const& key) const;

	template<typename T>
	T& get(std::string const& key);

	template<typename T>
	T const& get(std::string const& key) const;

#ifndef PYPLUSPLUS
	// returns the number of provided keys existant in the map
	template<typename ... Keys>
	int exist(Keys const& ... keys) const;

private:
	template<typename Return>
	Return sum() const;

	template<typename Return, typename T, typename ... Ts>
	Return sum(T const& t, Ts const& ... ts) const;
#endif

private:
	config_map_t  mConfig;
};

boost::shared_ptr<Backend>
loadBackend(boost::shared_ptr<Library> lib);

} // backend
} // redman


// implementation
namespace redman {
namespace backend {

template<typename T>
T& Backend::get(std::string const& key)
{
	return boost::get<T>(mConfig.at(key));
}

template<typename T>
T const& Backend::get(std::string const& key) const
{
	return boost::get<T>(mConfig.at(key));
}

#ifndef PYPLUSPLUS
template<typename ... Keys>
int Backend::exist(Keys const& ... keys) const
{
	return sum<int>(exists(std::string(keys))...);
}

template<typename Return>
inline
Return Backend::sum() const
{
	return 0;
}

template<typename Return, typename T, typename ... Ts>
inline
Return Backend::sum(T const& t, Ts const& ... ts) const
{
	return static_cast<Return>(t) + sum<Return>(ts...);
}
#endif // PYPLUSPLUS

} // backend
} // redman
