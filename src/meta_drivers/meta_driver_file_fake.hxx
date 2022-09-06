#ifndef _METADRIVERFILEFAKE_
#define _METADRIVERFILEFAKE_
#pragma once

#include <iostream>
#include <boost/crc.hpp>
#include <sstream>
#include "../meta_driver.hxx"

/// Fake file meta driver for testing
class MetaDriverFileFake : public MetaDriver
{
public:
    /// Constructor
    MetaDriverFileFake();

    /// Setup the meta driver
    void setup();

    /// Send info
    void sendInfo();

    /// launched when a message arrived on subscribed topics @param msg Messaged received from the broker on a subscribed topic
    void message_arrived(mqtt::const_message_ptr msg);

private:
    std::string mDecodedBSDL;
    std::string mBSDLName;
};

/// Fake file meta driver factory for testing
class MetaDriverFactoryFileFake : public MetaDriverFactory
{
public:
    MetaDriverFactoryFileFake(){};

    /// Create driver for file fake
    std::shared_ptr<MetaDriver> createDriver(void *arg);
};

#endif