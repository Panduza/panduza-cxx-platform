#ifndef _METADRIVERPSUFAKE_
#define _METADRIVERPSUFAKE_
#pragma once

#include "../meta_driver.hxx"

/// Fake psu meta driver for testing
class MetaDriverPsuFake : public MetaDriver {
    public:
        MetaDriverPsuFake()
        {
            LOG_F(INFO, "Meta Driver Psu Fake Constructor");
        };

        void setup();

        void sendInfo();
        
        // void run();
};

/// Fake psu meta driver factory for testing
class MetaDriverFactoryPsuFake : public MetaDriverFactory {
    public:
        MetaDriverFactoryPsuFake();

        /// Create driver for psu fake
        std::shared_ptr<MetaDriver> createDriver(void *arg);
};

#endif