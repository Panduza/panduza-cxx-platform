#include "meta_driver_io_fake.hxx"

void MetaDriverIoFake::setup()
{
    // Subscribe to the different topic needed
    subscribe(getBaseTopicCmds() + "/value/set", 0);
    subscribe(getBaseTopicCmds() + "/direction/set", 0);

    LOG_F(1, "Subscribed to %s and %s", (getBaseTopicCmds() + "/value/set").c_str(), (getBaseTopicCmds() + "/direction/set").c_str());
    LOG_F(1, "behaviour = %s", getBehaviour().c_str());
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

std::shared_ptr<std::thread> MetaDriverIoFake::createAlternativeThread(){}

std::shared_ptr<MetaDriver> MetaDriverFactoryIoFake::createDriver()
{
    std::shared_ptr<MetaDriver> MetaDriverFactoryIoFakeInstance = std::make_shared<MetaDriverIoFake>();

    return MetaDriverFactoryIoFakeInstance;
}