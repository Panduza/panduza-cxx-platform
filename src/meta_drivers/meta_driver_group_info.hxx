#ifndef _METADRIVERGROUPINFO
#define _METADRIVERGROUPINFO
#pragma once

#include "../meta_driver.hxx"
#include <jsoncpp/json/json.h>

class MetaDriverGroupInfo : public MetaDriver
{
public:
    MetaDriverGroupInfo(Json::Value payload);

};

#endif