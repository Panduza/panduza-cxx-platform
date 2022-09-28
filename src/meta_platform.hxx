#ifndef _METAPLATFORM_
#define _METAPLATFORM_
#pragma once

#include <iostream>
#include <list>
#include <jsoncpp/json/json.h>
#include <mqtt/async_client.h>
#include "meta_driver.hxx"

#include <boost/dll/import.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/range/iterator_range.hpp>

/// Metaplatform class that will handle the platform and its plugins
class Metaplatform
{
public:
    /// Constructor with argument @param argc Number of argument given from the start of the program @param argv Arguments
    Metaplatform(int argc, char *argv[]);

    /// Setter to enable log @param log boolean to enable log
    void setLog(bool log) { mLogEnabled = log; }

    /// getter to get log status @return return the log enable state
    bool getLog() const { return mLogEnabled; }

    /// Set the tree direction directory @param tree Set the location of the tree
    void setTreeDirectory(std::string tree) { mTreeDirectory = tree; }

    /// get the tree direction directory
    const std::string &getTreeDirectory() const { return mTreeDirectory; }

    /// Initialize all driver instances and launch interfaces and threads needed
    int run();

    /// Parse the tree file to generate the interfaces
    void generateInterfacesFromTreeFile();

    /// loop into interfaces of the broker to find drivers
    void generateInterfacesFromBrokerData(std::string broker_name, Json::Value broker_json);

    /// Check if driver of the interface is available and load if the case
    void searchMetaDriverFromInterface(Json::Value interface_json, std::string broker_name, std::string broker_addr, std::string broker_port);
    /// Load the driver
    void loadMetaDriver(Json::Value interface_json, std::string broker_name, std::string broker_addr, std::string broker_port);

    /// Gettter for the list of interfaces
    const std::list<std::shared_ptr<MetaDriver>> &getStaticInterfaces() const { return mDriverInstancesStatic; }

    /// Add Meta Driver into static list
    void addStaticDriverInstance(std::shared_ptr<MetaDriver> driver_instance) { mDriverInstancesStatic.emplace_back(driver_instance); }
    /// Add Meta Driver into reloadable list
    void addReloadableDriverInstance(std::map<std::string,std::list<std::shared_ptr<MetaDriver>>> driver_instance) {LOG_F(ERROR, "%s", driver_instance.begin()->first.c_str()); mDriverInstancesReloadableToLoad.emplace(driver_instance.begin()->first,driver_instance.begin()->second); }

    /// Clear reloadable meta driver list
    void clearReloadableInterfaces(std::string key_list_to_reload);

    void loadPluginFromPath(boost::filesystem::path lib_path);

    int mLoguruVerbose;

private:
    /// launching args, not used for now
    bool mLogEnabled;
    std::string mTreeDirectory;

    /// Json datas needed
    Json::Value mConfigJson;
    std::string mMachineName;

    /// Available drivers factories
    std::map<std::string, MetaDriverFactory *> mFactories;

    /// driver instances lists
    std::list<
        std::shared_ptr<MetaDriver>>
        mDriverInstancesStatic;

    std::map<std::string, std::list<std::shared_ptr<MetaDriver>>> mDriverInstancesReloadableToLoad;

    std::map<std::string, std::list<std::shared_ptr<MetaDriver>>> mDriverInstancesReloadableLoaded;
};

#endif