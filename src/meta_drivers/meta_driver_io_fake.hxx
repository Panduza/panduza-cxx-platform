#ifndef _METADRIVERIOFAKE_
#define _METADRIVERIOFAKE_
#pragma once

#include "../meta_driver.hxx"

/// Fake io meta driver for testing
class MetaDriverIoFake : public MetaDriver
{
public:
    ~MetaDriverIoFake() ;

    MetaDriverIoFake() { LOG_F(9, "Meta Driver Io Fake Constructor"); };

    void setup();

    void sendInfo();

    void autoToggle();

    void message_arrived(mqtt::const_message_ptr msg) override;

    void setDirection(std::string direction) { mDirection = direction; };
    void setValue(int value) { mValue = value; };

    std::shared_ptr<std::thread> createAlternativeThread();

private:
    bool mKillThread = false;
    std::thread *mAlternativeThread;

    std::string mDirection = "in";
    int mValue = 1;
};

/// Fake io meta driver factory for testing
class MetaDriverFactoryIoFake : public MetaDriverFactory
{
public:
    MetaDriverFactoryIoFake(){};

    std::shared_ptr<MetaDriver> createDriver(void *arg);
};

#endif