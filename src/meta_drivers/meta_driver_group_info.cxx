#include "meta_driver_group_info.hxx"

MetaDriverGroupInfo::MetaDriverGroupInfo(Json::Value payload)
{
    mDataPayload = payload;
}

void MetaDriverGroupInfo::setup()
{
    publish(getBaseTopic() + "/Group_info/atts/content" , mDataPayload, 0 , true);
}

void MetaDriverGroupInfo::sendInfo()
{
    // Create the info payload
    Json::Value info;
    info["type"] = "Group Info";
    info["version"] = "1.0";

    LOG_F(4, "Info sent is : %s", info.toStyledString().c_str());

    // publish the message info to the mqtt server for the pin
    publish(getBaseTopic() + "/Group_info/info", info, 0, false);
}

void MetaDriverGroupInfo::message_arrived(mqtt::const_message_ptr msg)
{

    loguru::set_thread_name("Sub callback");

    // If the message topid is "pza" and the payload is "*"
    if ((msg->get_topic().compare("pza") == 0) && (msg->get_payload().compare("*") == 0))
    {
        LOG_F(3, "Sending infos");
        sendInfo();
        publish(getBaseTopic() + "/Group_info/atts/content" , mDataPayload, 0 , true);
    }
}