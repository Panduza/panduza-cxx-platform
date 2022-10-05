#ifndef _METADRIVERPSUFAKE_
#define _METADRIVERPSUFAKE_
#pragma once

#include "../meta_driver.hxx"

/// Fake psu meta driver for testing
class MetaDriverPsuFake : public MetaDriver {
    public:
        /// Constructor
        MetaDriverPsuFake()
        {
            LOG_F(INFO, "Meta Driver Psu Fake Constructor");
        };
        /// setup of the Meta driver PSU
        void setup();
        /// Send the info of the meta driver when receiving * in the "pza" topic
        void sendInfo();

        Json::Value generateAutodetectInfo();

};

/// Fake psu meta driver factory for testing
class MetaDriverFactoryPsuFake : public MetaDriverFactory {
    public:
        /// Constructor
        MetaDriverFactoryPsuFake();

        /// Create driver for psu fake
        std::shared_ptr<MetaDriver> createDriver(void *arg);
};

#endif