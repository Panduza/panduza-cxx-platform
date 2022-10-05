#ifndef _PLUGINENTRYPOINT_
#define _PLUGINENTRYPOINT_
#pragma once

#include <boost/config.hpp>
#include "../meta_driver.hxx"

/// Entrypoint of the plugin template
class BOOST_SYMBOL_VISIBLE PluginEntrypoint
{
public:
    /// @brief Function to return the information and the factory of the Boundary Scan plugin
    /// @return Name of the driver and object of the plugins factory as a map
    virtual std::map<std::string, MetaDriverFactory *> getInformationAndFactory() = 0;
};

#endif