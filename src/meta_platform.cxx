#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <thread>
#include <functional>
#include "mqtt/async_client.h"
#include "loguru/loguru.hxx"
#include "plugins/entrypoint.hxx"

#include "meta_platform.hxx"
#include "meta_drivers/meta_driver_io_fake.hxx"
#include "meta_drivers/meta_driver_psu_fake.hxx"
#include "meta_drivers/meta_driver_file_fake.hxx"

using BSFMap = std::map<std::string,MetaDriverFactory *>;

Metaplatform::Metaplatform(int argc, char *argv[])
{
    // check for variables, not really implanted for now
    if (argc > 0)
    {
        for (int arg_it = 0; arg_it < argc; ++arg_it)
        {
            std::string arg = argv[arg_it];

            if (arg == "-h" || arg == "-help")
            {
                LOG_F(WARNING, "Available command : ");
                LOG_F(WARNING, "-help or -h : produce help message");
                LOG_F(WARNING, "-tree : dir of the tree");
                LOG_F(WARNING, "-log : enable log");

                exit(1);
            }
        }
    }
}

// ============================================================================
//
int Metaplatform::run()
{
    mLoguruVerbose = loguru::g_stderr_verbosity;
    // create seed for the random in the program
    srand(time(0));
    // Append factories
    // \todo Change this
    mFactories["io_fake"] = new MetaDriverFactoryIoFake();
    mFactories["psu_fake"] = new MetaDriverFactoryPsuFake();
    // mFactories["Scan_service"] = new MetaDriverFactoryFT2232BoundaryScan();
    
    // Create base path to load the plugin
    boost::filesystem::path libraries_path("/usr/share/panduza-cxx/libraries");
    boost::filesystem::path plugins_path("/usr/share/panduza-cxx/plugins");

    LOG_F(INFO, "Loading generic plugin...");
    loadPluginFromPath(libraries_path);
    LOG_F(INFO, "Loading custom plugin...");
    loadPluginFromPath(plugins_path);

    if(std::getenv("AUTODETECT"))
    {
        char *AUTODETECT = std::getenv("AUTODETECT");
        int autodetect_int = (*AUTODETECT) - 48; //as 0 is 48 and 1 is 49 in ascii

        if(autodetect_int == 1)
        {
            LOG_F(INFO,"GOING INTO AUTODETECT");
            autodetectInterfaces();

            LOG_F(INFO, "Available interfaces template generated, stopping the program...");
            return 0;
        }
    }

    // start the whole process of creating instances from the tree
    generateInterfacesFromTreeFile();
    LOG_F(8, "Number of Instances : %ld", getStaticInterfaces().size());

    if(getStaticInterfaces().size() <= 0)
    {
        LOG_F(ERROR, "No instances to load from the tree, exiting...");
        exit(0);
    }

    // loop into the interfaces created and run all interfaces
    for (const auto &interface : getStaticInterfaces())
    {
        LOG_SCOPE_F(INFO, "start interface %s", interface->getDriverName().c_str());
        interface->run();
    }

    // While to check if new instance need to be loaded
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (mDriverInstancesReloadableToLoad.size() >= 1)
        {
            // Move the front driver instance to the loaded driver list and
            mDriverInstancesReloadableLoaded.emplace(mDriverInstancesReloadableToLoad.begin()->first, mDriverInstancesReloadableToLoad.begin()->second);
            std::string list_instances_key = mDriverInstancesReloadableToLoad.begin()->first;
            mDriverInstancesReloadableToLoad.erase(list_instances_key);

            // Run the driver instance
            for (auto io_interface: mDriverInstancesReloadableLoaded[list_instances_key])
            {
                io_interface->run();
            }
        }
    }

    return 1;
}

void Metaplatform::clearReloadableInterfaces(std::string key_list_to_reload)
{
    // Loop into loaded driver instances and stop them
    for (auto driver_instance : mDriverInstancesReloadableLoaded)
    {
        if (driver_instance.first == key_list_to_reload)
        {
            std::list<std::shared_ptr<MetaDriver>> io_list = driver_instance.second;
            for(auto io_instance : io_list)
            {
                io_instance.reset();
            }
        }
    }
    mDriverInstancesReloadableLoaded.erase(key_list_to_reload);
}

void Metaplatform::loadPluginFromPath(boost::filesystem::path lib_path)
{
    if(!boost::filesystem::exists(lib_path))
    {
        return;
    }

    for(auto& file_path : boost::make_iterator_range(boost::filesystem::directory_iterator(lib_path), {}))
    {
        boost::dll::shared_library lib(file_path, boost::dll::load_mode::append_decorations);
        if (!lib.has("get_factory")) {
            // no such symbol
            continue;
        }
        // create pointer on function
        typedef boost::shared_ptr<PluginEntrypoint> (entrypoint_create_t)();
        boost::function<entrypoint_create_t> creator;
        creator = boost::dll::import_alias<entrypoint_create_t>(
            file_path,
            "get_factory",
            boost::dll::load_mode::append_decorations
        );
        // call the plugin and get the factory
        boost::shared_ptr<PluginEntrypoint> plugin_instance = creator();
        BSFMap pluginFactoryMap = plugin_instance->getInformationAndFactory();
        mFactories.insert(pluginFactoryMap.begin(),pluginFactoryMap.end());
    }
}

// ============================================================================
//
void Metaplatform::generateInterfacesFromTreeFile()
{
    // Try to open the file
    std::ifstream config_file("/etc/panduza/data/tree/tree.json", std::ifstream::binary);
    if (!config_file.is_open())
    {
        LOG_F(ERROR, "Config file not found in /etc/panduza/tree, exiting...");
        exit(0);
    }
    LOG_F(1, "Config file found in /etc/panduza/");

    // Parse config and store it
    Json::Value config_json;
    config_file >> config_json;
    mConfigJson = config_json;

    // Extract machine name
    mMachineName = config_json["machine"].asString();

    // Walk though brokers and load attached interfaces
    const Json::Value brokers_json = config_json["brokers"];
    for (auto broker_it = brokers_json.begin(); broker_it != brokers_json.end(); ++broker_it)
    {
        LOG_F(2, "Search interface through broker : %s", broker_it.name().c_str());
        const std::string broker_name = broker_it.name();
        const Json::Value broker_json = (*broker_it);
        generateInterfacesFromBrokerData(broker_name, broker_json);
    }
}

// ============================================================================
//
void Metaplatform::generateInterfacesFromBrokerData(std::string broker_name, Json::Value broker_json)
{
    // Put into variable some needed datas
    std::string broker_addr = broker_json["addr"].asString();
    std::string broker_port = broker_json["port"].asString();

    for (auto interface : broker_json["interfaces"])
    {
        // local variables
        const auto interface_name = interface["name"].asString().c_str();
        LOG_F(3, "Search for drivers in interface : %s", interface["name"].asString().c_str());

        // load attached Interface and search driver in the interface
        searchMetaDriverFromInterface(interface, broker_name, broker_addr, broker_port);
    }
}

// ============================================================================
//
void Metaplatform::searchMetaDriverFromInterface(Json::Value interface_json, std::string broker_name, std::string broker_addr, std::string broker_port)
{

    const std::string driver_name = interface_json["driver"].asString();

    // Check if the driver need have to be loaded
    LOG_F(3, "search driver %s", driver_name.c_str());
    auto driver_found = mFactories.find(driver_name);

    // if the driver is found, start the loading of the driver
    if (driver_found != mFactories.end())
    {
        std::string driver_found_name = driver_found->first;
        LOG_F(4, "Driver %s found, loading...", driver_found_name.c_str());
        loadMetaDriver(interface_json, broker_name, broker_addr, broker_port);
    }

    // just send an error if the driver don't have to be loaded
    else
    {
        LOG_F(ERROR, "Driver %s not found", driver_name.c_str());
    }
}

// ============================================================================
//
void Metaplatform::loadMetaDriver(Json::Value interface_json, std::string broker_name, std::string broker_addr, std::string broker_port)
{
    const std::string driver_name = interface_json["driver"].asString();

    // Find the matching factory and create the instance of the interface
    MetaDriverFactory *factory = mFactories[driver_name];

    // Create the driver and send the object
    auto driver_instance = factory->createDriver(this);

    // Initializing the meta driver instance
    LOG_F(5, "Driver %s created, initializing variables...", driver_name.c_str());
    LOG_F(INFO, "datas are : %s, %s, %s, %s, %s", mMachineName.c_str(), broker_name.c_str(), broker_addr.c_str(), broker_port.c_str(), interface_json.toStyledString().c_str());
    driver_instance->initialize(mMachineName, broker_name, broker_addr, broker_port, interface_json);

    LOG_F(5, "Driver %s initialized.", driver_name.c_str());

    // Append the created interface to the intefraces managed by the platform
    mDriverInstancesStatic.emplace_back(driver_instance);
}

void Metaplatform::autodetectInterfaces()
{
    std::ofstream file;
    Json::Value json;
    
    boost::filesystem::create_directory("/etc/panduza/platform");
    file.open("/etc/panduza/platform/cxx.json");

    if(file.is_open())
    {
        for (auto mFactory: mFactories)
        {
            LOG_F(3, "Generating template interface for %s", mFactory.first.c_str());
            json["drivers"].append(mFactory.second->createDriver(this)->generateAutodetectInfo());
        }

        file << json.toStyledString() << std::endl;

        file.close();
    }
    else
    {
        LOG_F(ERROR, "File not created...");
        exit(1);
    }
}