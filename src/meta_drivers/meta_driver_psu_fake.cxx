#include "meta_driver_psu_fake.hxx"

//MetaDriver Psu Fake

// void MetaDriverPsuFake::run(){
//     LOG_F(6, "PSU Fake Run");
// };

void MetaDriverPsuFake::setup()
{
    LOG_F(8, "direction, value, behaviour and loop variable loaded");

    subscribe(getBaseTopicCmds()+"/state/set", 0);
    subscribe(getBaseTopicCmds()+"/volts/set", 0);
    subscribe(getBaseTopicCmds()+"/amps/set", 0);

    LOG_F(8, "Subscribed to %s and %s", (getBaseTopicCmds()+"/voltage").c_str(), (getBaseTopicCmds()+"/ampere").c_str());
}

void MetaDriverPsuFake::sendInfo()
{
    Json::Value info;
    info["type"] = "psu";
    info["version"] = "1.0";
    LOG_F(INFO, "Info sent is : %s", info.toStyledString().c_str());

    publish(getBaseTopic()+"/info", info, 0, false);
}

Json::Value MetaDriverPsuFake::generateAutodetectInfo()
{
    Json::Value json;
    Json::Value template_json;

    template_json["name"] = "PSU";
    template_json["driver"] = "psu_fake";
    template_json["settings"]["behaviour"] = "manual";

    json["name"] = "psu_fake";
    json["version"] = "1.0";
    json["description"] = "Fake psu interface";
    json["template"] = template_json;
    json["autodetect"] = Json::arrayValue;

    return json;
}

MetaDriverFactoryPsuFake::MetaDriverFactoryPsuFake(){}

std::shared_ptr<MetaDriver> MetaDriverFactoryPsuFake::createDriver(void* arg)
{
    std::shared_ptr<MetaDriver> MetaDriverFactoryPsuFakeInstance = std::make_shared<MetaDriverPsuFake>();
    

    return MetaDriverFactoryPsuFakeInstance;
}