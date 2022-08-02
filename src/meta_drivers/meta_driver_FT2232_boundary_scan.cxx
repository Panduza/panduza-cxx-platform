#include "meta_driver_FT2232_boundary_scan.hxx"
#include "../meta_platform.hxx"

// Initalizing static variables
std::mutex MetaDriverFT2232BoundaryScan::mSetupMutex;

// ============================================================================
//

void MetaDriverFT2232BoundaryScan::setup()
{
    /// Create Meta Driver File
    std::shared_ptr<MetaDriver> meta_driver_file_instance = std::make_shared<MetaDriverFT2232BsdlLoader>(this);

    /// Initialize the meta_driver file instance
    meta_driver_file_instance->initialize(getMachineName(), getBrokerName(), getBrokerAddr(), getBrokerPort(), getInterfaceTree());

    /// Add the meta driver instance to the main meta driver list
    mMetaplatformInstance->addStaticDriverInstance(meta_driver_file_instance);

    // /// Enable alternative thread
    // mNeedThread = true;
}

// ============================================================================
//
void MetaDriverFT2232BoundaryScan::startIo()
{
    // Kill all reloadable instances
    mMetaplatformInstance->clearReloadableInterfaces();

    // If there is a jtagManager loaded, delete it and reset its flag
    if (mJtagManagerLoaded)
    {
        mJtagManager->deinit();
        mJtagManager.reset();
        mJtagManagerLoaded = false;
    }

    // If there is no Jtag Manager, create it and pass a flag to true
    if (!mJtagManagerLoaded)
    {
        mJtagManager = createJtagManager(mProbeName, mBSDLName);
        mJtagManagerLoaded = true;
    }

    // get some variable and key point
    const Json::Value interface_json = getInterfaceTree();
    const Json::Value repeated_json = interface_json["repeated"];
    const std::string format = "%r";
    const size_t posFormat = interface_json["name"].asString().find(format);

    // Loop into the repeated list of pins
    for (auto repeated_pin : repeated_json)
    {
        /// Create a json with the good pin name
        Json::Value interface_json_copy = interface_json;
        interface_json_copy["settings"]["pin"] = interface_json_copy["settings"]["pin"].asString().replace(0, format.length(), repeated_pin.asString());
        interface_json_copy["name"] = interface_json_copy["name"].asString().replace(posFormat, format.length(), repeated_pin.asString());
        LOG_F(2, "loading driver for pin : %s", interface_json_copy["name"].asString().c_str());

        // Create the Meta Driver
        std::shared_ptr<MetaDriver> meta_driver_io_instance = std::make_shared<MetaDriverFT2232Io>(getJtagManager());

        /// Initialize the meta Driver
        meta_driver_io_instance->initialize(getMachineName(), getBrokerName(), getBrokerAddr(), getBrokerPort(), interface_json_copy);

        /// add the meta driver to the main list
        mMetaplatformInstance->addReloadableDriverInstance(meta_driver_io_instance);
    }
}

// ============================================================================
//
std::shared_ptr<JtagFT2232> MetaDriverFT2232BoundaryScan::getJtagManager()
{
    if (mJtagManagerLoaded == false)
    {
        mJtagManager = createJtagManager(mProbeName, mBSDLName);
        mJtagManagerLoaded = true;
    }
    return mJtagManager;
}

// ============================================================================
//
std::shared_ptr<JtagFT2232> MetaDriverFT2232BoundaryScan::createJtagManager(std::string probe_name, std::string bsdl_name)
{
    // Create the Jtag manager with the probe name and bsdl name
    std::shared_ptr<JtagFT2232> jtagManager = std::make_shared<JtagFT2232>();
    jtagManager->initializeDriver(probe_name, bsdl_name);

    return jtagManager;
}

// ============================================================================
//
void MetaDriverFT2232BoundaryScan::sendInfo()
{
    // // Create the info payload
    // Json::Value info;
    // info["type"] = "Group";
    // info["version"] = "1.0";

    // LOG_F(4, "Info sent is : %s", info.toStyledString().c_str());

    // // publish the message info to the mqtt server for the pin
    // publish(getBaseTopic() + "/info", info, 0, false);
}

// ============================================================================
//
void MetaDriverFT2232BoundaryScan::checkInput()
{
    loguru::set_thread_name("MetaDriverFT2232BoundaryScanGroup");
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        LOG_F(9, "MetaDriverFT2232BoundaryScanGroup");
    }
}

// ============================================================================
//
std::shared_ptr<MetaDriver> MetaDriverFactoryFT2232BoundaryScan::createDriver(void *arg)
{
    // get the parameter and give it to the meta driver constructor
    Metaplatform *meta_platform_instance = (Metaplatform *)arg;
    auto MetaDriverFactoryFT2232BoundaryScanInstance = std::make_shared<MetaDriverFT2232BoundaryScan>(meta_platform_instance);

    return MetaDriverFactoryFT2232BoundaryScanInstance;
}