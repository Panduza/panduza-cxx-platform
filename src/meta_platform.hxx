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

/// @brief Metaplatform class that will handle the platform and its plugins
class Metaplatform
{
public:
    /// @brief Constructor with argument
    /// @param argc Number of argument given from the start of the program
    /// @param argv Arguments
    Metaplatform(int argc, char *argv[]);

    /// @brief Setter to enable log
    /// @param log boolean to enable log
    void setLog(bool log) { mLogEnabled = log; }

    /// @brief getter to get log status
    /// @return return the log enable state
    bool getLog() const { return mLogEnabled; }

    /// @brief Set the tree direction directory
    /// @param tree Set the location of the tree
    void setTreeDirectory(std::string tree) { mTreeDirectory = tree; }

    /// @brief get the tree direction directory
    const std::string &getTreeDirectory() const { return mTreeDirectory; }

    /// @brief Initialize all driver instances and launch interfaces and threads needed
    int run();

    /// @brief Parse the tree file to generate the interfaces
    void generateInterfacesFromTreeFile();

    /// @brief loop into interfaces of the broker to find drivers
    void generateInterfacesFromBrokerData(std::string broker_name, Json::Value broker_json);

    /// @brief Check if driver of the interface is available and load if the case
    void searchMetaDriverFromInterface(Json::Value interface_json, std::string broker_name, std::string broker_addr, std::string broker_port);
    /// @brief Load the driver
    void loadMetaDriver(Json::Value interface_json, std::string broker_name, std::string broker_addr, std::string broker_port);

    /// @brief Gettter for the list of interfaces
    const std::list<std::shared_ptr<MetaDriver>> &getStaticInterfaces() const { return mDriverInstancesStatic; }

    /// @brief Add Meta Driver into static list
    void addStaticDriverInstance(std::shared_ptr<MetaDriver> driver_instance) { mDriverInstancesStatic.emplace_back(driver_instance); }
    /// @brief Add Meta Driver into reloadable list
    void addReloadableDriverInstance(std::shared_ptr<MetaDriver> driver_instance) { mDriverInstancesReloadableToLoad.emplace_back(driver_instance); }

    /// @brief Clear reloadable meta driver list
    void clearReloadableInterfaces();

    void loadPluginFromPath(boost::filesystem::path lib_path);

    int mLoguruVerbose;
private:
    /// @brief launching args, not used for now
    bool mLogEnabled;
    std::string mTreeDirectory;

    /// @brief Json datas needed
    Json::Value mConfigJson;
    std::string mMachineName;

    /// @brief Available drivers factories
    std::map<std::string, MetaDriverFactory *> mFactories;

    /// @brief driver instances lists
    std::list<
        std::shared_ptr<MetaDriver>>
        mDriverInstancesStatic;

    std::list<
        std::shared_ptr<MetaDriver>>
        mDriverInstancesReloadableToLoad;

    std::list<
        std::shared_ptr<MetaDriver>>
        mDriverInstancesReloadableLoaded;
};

#endif