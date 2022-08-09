#ifndef _METADRIVERGROUPINFO
#define _METADRIVERGROUPINFO
#pragma once

#include "../meta_driver.hxx"
#include <jsoncpp/json/json.h>

class MetaDriverGroupInfo : public MetaDriver
{
public:
    MetaDriverGroupInfo(Json::Value payload);

    /// Setup the meta driver
    void setup();

    /// Send info
    void sendInfo();

    /// launched when a message arrived on subscribed topics
    void message_arrived(mqtt::const_message_ptr msg);

private:
    Json::Value mDataPayload;

};

#endif