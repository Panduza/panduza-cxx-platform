#include "meta_driver_io_fake.hxx"

MetaDriverIoFake::~MetaDriverIoFake()
{
    mKillThread = true;
    if(mAlternativeThread)
    {
        mAlternativeThread->join();
        delete mAlternativeThread;
    }
    mKillThread = false;
    
}

void MetaDriverIoFake::setup()
{
    // Subscribe to the different topic needed
    subscribe(getBaseTopicCmds() + "/value/set", 0);
    subscribe(getBaseTopicCmds() + "/direction/set", 0);

    LOG_F(1, "Subscribed to %s and %s", (getBaseTopicCmds() + "/value/set").c_str(), (getBaseTopicCmds() + "/direction/set").c_str());
    LOG_F(1, "behaviour = %s", getBehaviour().c_str());
    if (getBehaviour() == "auto_toggle"){
        mAlternativeThread = new std::thread(&MetaDriverIoFake::autoToggle, this);
    }
}


void MetaDriverIoFake::sendInfo()
{
    Json::Value info;

    info["type"] = "io";
    info["version"] = "1.0";

    LOG_F(4, "Info sent is : %s", info.toStyledString().c_str());

    publish(getBaseTopic() + "/info", info, 0, false);
}


void MetaDriverIoFake::autoToggle()
{
    Json::Value direction_payload;
    direction_payload["direction"] = "out";
    publish(getBaseTopicCmds() + "/direction/set", direction_payload, 0, false);

    while (1)
    {
        if (mValue == 1)
        {
            mValue = 0;

            Json::Value payload;

            payload["value"] = mValue;

            publish(getBaseTopicCmds() + "/value/set", payload, 0, false);
        }
        else if (mValue == 0)
        {
            mValue = 1;

            Json::Value payload;

            payload["value"] = mValue;

            publish(getBaseTopicCmds() + "/value/set", payload, 0, false);
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}


// Callback for when a message arrives.
void MetaDriverIoFake::message_arrived(mqtt::const_message_ptr msg)
{
    VLOG_SCOPE_F(2, "Message arrived in io fake");

    LOG_F(3, "Message arrived, topic : %s, payload : %s", msg->get_topic().c_str(), msg->get_payload().c_str());

    if ((msg->get_topic().compare("pza") == 0) && (msg->get_payload().compare("*") == 0))
    {
        LOG_F(3, "Sending infos");

        sendInfo();
    }
    else
    {
        Json::Value parsedMsg = parseMsg(msg->get_payload());

        LOG_F(5, "parsed info is %s", parsedMsg.toStyledString().c_str());

        if (msg->get_topic().compare(getBaseTopicCmds() + "/value/set") == 0)
        {
            publish(getBaseTopicAtts() + "/value", parsedMsg, 0, true);

            setValue(parsedMsg.get("value", "").asInt());
        }
        if (msg->get_topic().compare(getBaseTopicCmds() + "/direction/set") == 0)
        {
            publish(getBaseTopicAtts() + "/direction", parsedMsg, 0, true);

            setDirection(parsedMsg.get("direction", "").asString());
        }
    }
}

std::shared_ptr<std::thread> MetaDriverIoFake::createAlternativeThread(){
    return std::make_shared<std::thread>(&MetaDriverIoFake::autoToggle, this);
}

Json::Value MetaDriverIoFake::generateAutodetectInfo()
{
    Json::Value json;
    Json::Value template_json;

    template_json["name"] = "IO";
    template_json["driver"] = "io_fake";
    template_json["settings"]["behaviour"] = "manual | auto_toggle";
    
    json["name"] = "io_fake";
    json["version"] = "1.0";
    json["description"] = "Fake io interface";
    json["template"] = template_json;
    json["autodetect"] = Json::arrayValue;

    return json;
}

std::shared_ptr<MetaDriver> MetaDriverFactoryIoFake::createDriver(void *arg)
{
    std::shared_ptr<MetaDriver> MetaDriverFactoryIoFakeInstance = std::make_shared<MetaDriverIoFake>();

    return MetaDriverFactoryIoFakeInstance;
}