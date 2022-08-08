#ifndef _METADRIVERFT2232BSDLLOADER_
#define _METADRIVERFT2232BSDLLOADER_
#pragma once

#include <iostream>
#include <boost/crc.hpp>
#include <sstream>
#include "../meta_driver.hxx"

class MetaDriverFT2232BoundaryScan;

class MetaDriverFT2232BsdlLoader : public MetaDriver
{
public:
    /// Constructor
    MetaDriverFT2232BsdlLoader(MetaDriverFT2232BoundaryScan *meta_driver_ft2232_boundary_scan_instance);

    /// Create an alternative thread when needed
    std::shared_ptr<std::thread> createAlternativeThread() { return std::make_shared<std::thread>(&MetaDriverFT2232BsdlLoader::test, this); }

    /// Setup the meta driver
    void setup();

    /// Send info
    void sendInfo();

    /// launched when a message arrived on subscribed topics
    void message_arrived(mqtt::const_message_ptr msg);

    /// Actual thread, to change
    void test();

private:
    std::string mDecodedBSDL;
    std::string mBSDLName;

    MetaDriverFT2232BoundaryScan *mMetaDriverFT2232BoundaryScanInstance;
};

#endif