#ifndef _METADRIVERFT2232BOUNDARYSCAN_
#define _METADRIVERFT2232BOUNDARYSCAN_
#pragma once

// #include "ft2232_boundary_scan/jtag_manager.hxx"
#include "../meta_driver.hxx"
#include "ft2232_boundary_scan/jtag_core/jtag_core.hxx"
#include "ft2232_boundary_scan/jtag_manager.hxx"
#include "meta_driver_FT2232_bsdl_loader.hxx"
#include "meta_driver_FT2232_io.hxx"

#include <thread>

class Io;
class Metaplatform;

class MetaDriverFT2232BoundaryScan : public MetaDriver
{
public:
    /// Destructor
    ~MetaDriverFT2232BoundaryScan() { LOG_F(9, "Meta Driver Io Fake Destructor"); }

    /// Constructor with parent pointer
    MetaDriverFT2232BoundaryScan(Metaplatform *meta_platform_interface)
    {
        mMetaplatformInstance = meta_platform_interface;
    }

    /// Setup the meta driver
    void setup();

    /// Start the creation of Io Meta Driver
    void startIo();

    /// Return JtagManager Object
    std::shared_ptr<JtagFT2232> getJtagManager();

    /// Create a Jtag manager and initialize it
    std::shared_ptr<JtagFT2232> createJtagManager(std::string probe_name, std::string bsdl_name);

    /// Send Info function
    void sendInfo();

    /// Reads states of input IOs
    void checkInput();

    /// Create a shared ptr thread if needed
    std::shared_ptr<std::thread> createAlternativeThread() { return std::make_shared<std::thread>(&MetaDriverFT2232BoundaryScan::checkInput, this); }

    /// set the BSDL name
    void setBSDLName(std::string bsdl_file_name) { mBSDLName = bsdl_file_name; }

private:
    std::string mBSDLName;
    std::string mMode;
    std::string mProbeName;

    static std::mutex mSetupMutex;

    Metaplatform *mMetaplatformInstance;

    std::mutex mutx;

    // static std::list<std::shared_ptr<JtagFT2232>> JTAG_MANAGERS;
    std::shared_ptr<JtagFT2232> mJtagManager;
    bool mJtagManagerLoaded = false;
};

class MetaDriverFactoryFT2232BoundaryScan : public MetaDriverFactory
{
public:
    MetaDriverFactoryFT2232BoundaryScan(){};

    std::shared_ptr<MetaDriver> createDriver(void *arg);
};

#endif