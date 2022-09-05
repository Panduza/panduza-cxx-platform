#ifndef _METADRIVERIOFAKE_
#define _METADRIVERIOFAKE_
#pragma once

#include "../meta_driver.hxx"

/// @brief Fake io meta driver for testing
class MetaDriverIoFake : public MetaDriver
{
public:
    /// @brief Destructor
    ~MetaDriverIoFake() ;
    /// @brief Constructor
    MetaDriverIoFake() { LOG_F(9, "Meta Driver Io Fake Constructor"); };
    /// @brief Setup the fake io meta driver
    void setup();
    /// @brief Send info to the broker when receiving * in "pza" topic
    void sendInfo();
    /// @brief Auto toggle IO
    void autoToggle();
    /// @brief Message arrived from the mqtt broker
    /// @param msg message received
    void message_arrived(mqtt::const_message_ptr msg) override;
    /// @brief Set the direction of the IO
    /// @param direction direction of the IO to set
    void setDirection(std::string direction) { mDirection = direction; };
    /// @brief set the value of the IO
    /// @param value value of the IO to set
    void setValue(int value) { mValue = value; };

    /// @brief create an alternative thread to launch
    /// @return Shared ptr of the thread to launch in parallel
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
    /// @brief Constructor
    MetaDriverFactoryIoFake(){};

    /// @brief Create driver for io fake
    std::shared_ptr<MetaDriver> createDriver(void *arg);
};

#endif