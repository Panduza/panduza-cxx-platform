#ifndef _METADRIVERIOFAKE_
#define _METADRIVERIOFAKE_
#pragma once

#include "../meta_driver.hxx"

class MetaDriverIoFake : public MetaDriver
{
public:
    ~MetaDriverIoFake() { LOG_F(9, "Meta Driver Io Fake Destructor"); }

    MetaDriverIoFake() { LOG_F(9, "Meta Driver Io Fake Constructor"); };

    void setup();

    void sendInfo();

    void autoToggle();

    void message_arrived(mqtt::const_message_ptr msg) override;

    void setDirection(std::string direction) { mDirection = direction; };
    void setValue(int value) { mValue = value; };

    std::shared_ptr<std::thread> createAlternativeThread();

private:
    std::string mDirection = "in";
    int mValue = 1;
};

class MetaDriverFactoryIoFake : public MetaDriverFactory
{
public:
    MetaDriverFactoryIoFake(){};

    std::shared_ptr<MetaDriver> createDriver(void *arg);
};

#endif