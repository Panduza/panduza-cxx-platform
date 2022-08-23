#ifndef _METADRIVERPSUFAKE_
#define _METADRIVERPSUFAKE_
#pragma once

#include "../meta_driver.hxx"

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

class MetaDriverFactoryPsuFake : public MetaDriverFactory {
    public:
        MetaDriverFactoryPsuFake();

        std::shared_ptr<MetaDriver> createDriver(void *arg);
};

#endif