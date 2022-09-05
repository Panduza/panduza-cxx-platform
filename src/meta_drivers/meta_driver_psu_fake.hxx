#ifndef _METADRIVERPSUFAKE_
#define _METADRIVERPSUFAKE_
#pragma once

#include "../meta_driver.hxx"

/// @brief Fake psu meta driver for testing
class MetaDriverPsuFake : public MetaDriver {
    public:
        /// @brief Constructor
        MetaDriverPsuFake()
        {
            LOG_F(INFO, "Meta Driver Psu Fake Constructor");
        };
        /// @brief setup of the Meta driver PSU
        void setup();
        /// @brief Send the info of the meta driver when receiving * in the "pza" topic
        void sendInfo();
};

/// Fake psu meta driver factory for testing
class MetaDriverFactoryPsuFake : public MetaDriverFactory {
    public:
        /// @brief Constructor
        MetaDriverFactoryPsuFake();

        /// Create driver for psu fake
        std::shared_ptr<MetaDriver> createDriver(void *arg);
};

#endif