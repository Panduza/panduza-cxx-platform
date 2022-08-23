#ifndef _METADRIVERFILEFAKE_
#define _METADRIVERFILEFAKE_
#pragma once

#include <iostream>
#include <boost/crc.hpp>
#include <sstream>
#include "../meta_driver.hxx"

class MetaDriverFileFake : public MetaDriver
{
public:
    /// Constructor
    MetaDriverFileFake();

    /// Create an alternative thread when needed
    // std::shared_ptr<std::thread> createAlternativeThread() { return std::make_shared<std::thread>(&MetaDriverFileFake::test, this); }

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
};

class MetaDriverFactoryFileFake : public MetaDriverFactory
{
public:
    MetaDriverFactoryFileFake(){};

    std::shared_ptr<MetaDriver> createDriver(void *arg);
};

#endif