#include <iostream>
#include <thread>
#include "meta_driver.hxx"

void MetaDriver::connected(const std::string &cause)
{
    // Log the successful connection and log the client id
    LOG_F(INFO, "Connection success");
    LOG_F(INFO, "Subscribing to topic pza");
    LOG_F(INFO, "for client %s", mClientID.c_str());

    // Subscribe to the basic topic which is "pza"
    subscribe("pza", 0);
}

void MetaDriver::reconnect()
{
    // Try to reconnect to the mqtt broker
    try
    {
        getClientMqtt()->connect(connOpts, nullptr, *this);
    }
    catch (const mqtt::exception &exc)
    {
        LOG_F(ERROR, "Error : %s", exc.what());
        exit(1);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
}

void MetaDriver::on_failure(const mqtt::token &tok)
{
    // Count out to the number of retry allowed for each failed
    LOG_F(WARNING, "connection attempt failed");
    if (++mRetryCnt > mRetry)
        exit(1);
    LOG_F(WARNING, "retry no: %d out of %d", mRetryCnt, mRetry);
    reconnect();
}

void MetaDriver::on_success(const mqtt::token &tok)
{
    // reset the retry counter when the connection is successfull
    mRetryCnt = 0;
}

void MetaDriver::connection_lost(const std::string &cause)
{
    // Warn the user and then try to reconnect
    LOG_F(WARNING, "Connection lost");
    if (!cause.empty())
        LOG_F(WARNING, "cause : %s", cause.c_str());
    getClientMqtt()->reconnect();
}

void MetaDriver::message_arrived(mqtt::const_message_ptr msg)
{
    // Generic message arrived callback, only get the pza subscription for all instance that do not call for a message_arrived callback
    LOG_F(INFO, "Message arrived, topic : %s, payload : %s", msg->get_topic().c_str(), msg->get_payload().c_str());
    if ((msg->get_topic().compare("pza") == 0) && (msg->get_payload().compare("*") == 0))
    {
        // Send the info for the meta driver
        LOG_F(1, "Sending infos");
        sendInfo();
    }
}

void MetaDriver::delivery_complete(mqtt::delivery_token_ptr token) {}

void MetaDriver::sendInfo()
{
    // Define the info json to send
    Json::Value info;
    info["type"] = "Unknown";
    info["version"] = "1.0";

    // publish info of the meta driver
    LOG_F(2, "Info sent is : %s", info.toStyledString().c_str());
    publish(getBaseTopic() + "/info", info, 0, false);
}

void MetaDriver::publish(std::string topic, Json::Value payload, int qoS, bool retained)
{
    try
    {
        mPubMutex.lock();
        // Get the clientMqtt and create the message with the different argument given
        std::shared_ptr<mqtt::async_client> clientMqtt = getClientMqtt();
        mqtt::message_ptr message = mqtt::make_message(topic, payload.toStyledString());

        LOG_F(8, "message created");

        // Add parameter to the message
        message->set_qos(qoS);
        message->set_retained(retained);

        // Log the message to send
        LOG_F(8, "Set QoS to : %d and Retained to : %d", qoS, retained);
        LOG_F(8, "QoS added to message");
        LOG_F(8, "Payload is : %s", payload.toStyledString().c_str());
        LOG_F(8, "Get the client back and publish the message, the client ID is : %s", clientMqtt->get_client_id().c_str());

        // pubish the message
        clientMqtt->publish(message);
        LOG_F(8, "Message published");
        mPubMutex.unlock();
    }
    catch (const mqtt::exception &exc)
    {
        LOG_F(ERROR, "%s", exc.get_message().c_str());
        exit(1);
    }
}

Json::Value MetaDriver::parseMsg(std::string payload)
{
    Json::Value parsedMsg;
    Json::Reader reader;

    // Parse the payload into parsedMsg to have it as a Json
    bool parsingSuccessful = reader.parse(payload, parsedMsg);
    if (!parsingSuccessful)
    {
        LOG_F(ERROR, "Failed to parse %s", reader.getFormattedErrorMessages().c_str());
    }
    LOG_F(4, "parsed message is : %s", parsedMsg.toStyledString().c_str());

    // return the parsed message
    return parsedMsg;
}

std::shared_ptr<std::thread> MetaDriver::createAlternativeThread() {}

void MetaDriver::subscribe(std::string topic, int qoS)
{
    // subscribe to the topic selected with the qoS indicated
    getClientMqtt()->subscribe(topic, qoS, nullptr, *this);
}

void MetaDriver::setBehaviour()
{
    // check the behaviour and set into the variable of the instance of the meta driver
    if (!mInterfaceTree["settings"]["behaviour"].isNull())
    {
        LOG_F(5, "Behaviour detected : %s", mInterfaceTree["settings"]["behaviour"].asString().c_str());
        mBehaviour = mInterfaceTree["settings"]["behaviour"].asString();
    }
    else
    {
        // If no behaviour, set to static
        LOG_F(5, "No behaviour detected, switch to static");
        mBehaviour = "static";
    }
}

void MetaDriver::run()
{
    LOG_F(5, "Starting Meta Driver run function");

    // create the meta driver clientMqtt and git some connection option
    mClientMqtt = std::make_shared<mqtt::async_client>(mBrokerAddr, mClientID);
    connOpts = mqtt::connect_options_builder()
                   .clean_session()
                   .finalize();

    // Install the callback(s) before connecting.
    mClientMqtt->set_callback(*this);

    try
    {
        LOG_F(INFO, "Connection to the MQTT server...");
        LOG_F(INFO, "Client ID : %s , Server addr : %s", mClientMqtt->get_client_id().c_str(), mClientMqtt->get_server_uri().c_str());

        // Try to connect to the Mqtt server
        getClientMqtt()->connect(connOpts, nullptr, *this);

        // Waiting the client to be connected
        while (!mClientMqtt->is_connected())
            ;
    }
    catch (const mqtt::exception &exc)
    {
        std::cerr << "\nERROR: Unable to connect to MQTT server: '"
                  << mBrokerAddr << "'" << exc << std::endl;
    }

    LOG_F(INFO, "Instance connected %s", mClientMqtt->get_client_id().c_str());

    // Start the setup function
    LOG_SCOPE_F(1, "start setup");
    setup();
}

void MetaDriver::initialize(std::string machine_name, std::string broker_name, std::string broker_addr, std::string broker_port, Json::Value interface_json)
{
    // create a random number for the client id
    std::string random = std::to_string(std::rand());

    // set all variable needed
    setMachineName(machine_name);
    setBrokerName(broker_name);
    setInterfaceTree(interface_json);
    setBrokerAddr(broker_addr);
    setBrokerPort(broker_port);
    mInterfaceTree["name"].isNull() ? setInterfaceName("") : setInterfaceName(mInterfaceTree["name"].asString());
    // setInterfaceName(mInterfaceTree["name"].asString());
    setDriverName(mInterfaceTree["driver"].asString());
    setProbeName(mInterfaceTree["settings"]["probe_name"].asString());
    setClientID(mMachineName + "_" + mDriverName + "_" + random);
    setBaseTopic();
    setBaseTopicCmds();
    setBaseTopicAtts();
    setBehaviour();

    // log for verification
    LOG_F(6, "Variables added : Machine name = %s", mMachineName.c_str());
    LOG_F(6, "Variables added : Broker name = %s", mBrokerName.c_str());
    LOG_F(6, "Variables added : Broker addr = %s", mBrokerAddr.c_str());
    LOG_F(6, "Variables added : Broker port = %s", mBrokerPort.c_str());
    LOG_F(6, "Variables added : Broker Client ID = %s", mClientID.c_str());
    LOG_F(6, "Variables added : Interface Tree = %s", mInterfaceTree.toStyledString().c_str());
    LOG_F(6, "Variables added : Interface Name = %s", mInterfaceName.c_str());
    LOG_F(6, "Variables added : Driver Name = %s", mDriverName.c_str());
    LOG_F(6, "Variables added : Probe Name = %s", mProbeName.c_str());
    LOG_F(6, "Variables added : Base Topic = %s", mBaseTopic.c_str());
    LOG_F(6, "Variables added : Base Topic Cmd = %s", mBaseTopicCmd.c_str());
    LOG_F(6, "Variables added : Base Topic Atts = %s", mBaseTopicAtts.c_str());
}

void MetaDriver::setup() {}

std::shared_ptr<MetaDriver> MetaDriverFactory::createDriver(void *arg){};