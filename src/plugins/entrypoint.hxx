#ifndef _PLUGINENTRYPOINT_
#define _PLUGINENTRYPOINT_
#pragma once

#include <boost/config.hpp>
#include "../meta_driver.hxx"

/// Entrypoint of the plugin template
class BOOST_SYMBOL_VISIBLE PluginEntrypoint
{
public:
    /// Return the driver name and factory of the plugin
    virtual std::map<std::string, MetaDriverFactory *> getInformationAndFactory() = 0;
};

#endif