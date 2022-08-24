#ifndef _METADRIVER_
#define _METADRIVER_
#pragma once

#include <iostream>
#include <jsoncpp/json/json.h>
#include <algorithm>

#include "loguru/loguru.hxx"
#include "mqtt/async_client.h"

/// Meta Drivers template
class MetaDriver : public virtual mqtt::callback, public virtual mqtt::iaction_listener
{
public:
    /// Retry count on reconnect, resetted when successful
    int mRetryCnt = 0;

    /// Callback functions on failure
    void on_failure(const mqtt::token &tok) override;
    /// Callback function on success
    void on_success(const mqtt::token &tok) override;
    /// Callback function when mqtt need to reconnect
    void reconnect();
    /// (Re)connection success
    void connected(const std::string &cause) override;
    /// Callback for when the connection is lost. This will initiate the attempt to manually reconnect.
    void connection_lost(const std::string &cause) override;
    /// Callback function when a message arrives.
    virtual void message_arrived(mqtt::const_message_ptr msg) override;
    /// Called when delivery for a message has been completed, and all acknowledgments have been received.
    void delivery_complete(mqtt::delivery_token_ptr token) override;

    /// constructor
    MetaDriver() { LOG_F(9, "Meta Driver Constructor"); };

    /// creating mqtt connection, connects it and launch setup function of the Meta driver
    virtual void run();

    /// add datas to variables of the meta driver
    void initialize(std::string machine_name, std::string broker_name, std::string borker_addr, std::string broker_port, Json::Value config_json);

    /// create an virtual function that will be empty or not depending of the meta driver loading
    virtual void setup();

    /// send info json when needed with type and version
    virtual void sendInfo();

    /// publish function : send the payload into the mqtt broker
    void publish(std::string topic, Json::Value payload, int qoS, bool retained);

    /// parse the received payload
    Json::Value parseMsg(std::string payload);

    /// create an alternative thread to be call in meta platform to be launched
    virtual std::shared_ptr<std::thread> createAlternativeThread();

    /// subscribe to topics
    virtual void subscribe(std::string topic, int qoS);

    /// Setter of the interface tree
    void setInterfaceTree(Json::Value interface_tree) { mInterfaceTree = interface_tree; };
    /// Setter of the machine name
    void setMachineName(std::string machine_name) { mMachineName = machine_name; };
    /// Setter of the broker name
    void setBrokerName(std::string broker_name) { mBrokerName = broker_name; };
    /// Setter of the broker address
    void setBrokerAddr(std::string broker_addr) { mBrokerAddr = broker_addr; };
    /// Setter of the broker port
    void setBrokerPort(std::string broker_port) { mBrokerPort = broker_port; };
    /// Setter of the cliend id
    void setClientID(std::string client_id) { mClientID = client_id; };
    /// Setter of the interface name
    void setInterfaceName(std::string interface_name) { mInterfaceName = interface_name; };
    /// Setter of the driver name
    void setDriverName(std::string driver_name) { mDriverName = driver_name; };
    /// Setter of the probe name
    void setProbeName(std::string probe_name) { mProbeName = probe_name; };
    /// Setter of the base topic
    void setBaseTopic() { mBaseTopic = "pza/" + mMachineName + "/" + mInterfaceTree["driver"].asString(); };
    /// Setter of the base commands topic
    void setBaseTopicCmds() { mBaseTopicCmd = mBaseTopic + "/cmds"; };
    /// Setter of the base attributs topic
    void setBaseTopicAtts() { mBaseTopicAtts = mBaseTopic + "/atts"; };
    /// Setter of the behaviour parameter
    void setBehaviour();
    /// Setter of the behaviour parameter by defining the parameter
    void setBehaviour(std::string behaviour) { mBehaviour = behaviour; };
    /// Getter of the interface tree
    const Json::Value &getInterfaceTree() const { return mInterfaceTree; };
    /// Getter of the machine name
    const std::string &getMachineName() const { return mMachineName; };
    /// Getter of the broker name
    const std::string &getBrokerName() const { return mBrokerName; };
    /// Getter of the broker addr
    const std::string &getBrokerAddr() const { return mBrokerAddr; };
    /// Getter of the broker port
    const std::string &getBrokerPort() const { return mBrokerPort; };
    /// Getter of the driver name
    const std::string &getDriverName() const { return mDriverName; };
    /// Getter of the probe name
    const std::string &getProbeName() const { return mProbeName; };
    /// Getter of the base topic
    const std::string &getBaseTopic() const { return mBaseTopic; };
    /// Getter of the base commands topic
    const std::string &getBaseTopicCmds() const { return mBaseTopicCmd; };
    /// Getter of the base attributs topic
    const std::string &getBaseTopicAtts() const { return mBaseTopicAtts; };
    /// Getter of the behaviour parameter
    const std::string &getBehaviour() const { return mBehaviour; };
    /// Getter of the Mqtt Client
    const std::shared_ptr<mqtt::async_client> &getClientMqtt() const { return mClientMqtt; };

private:
    /// The maximum number of retry
    int mRetry = 5;

    /// Json value of the interface tree
    Json::Value mInterfaceTree;

    /// Different value needed
    std::string mMachineName;
    std::string mBrokerName;
    std::string mBrokerAddr;
    std::string mBrokerPort;
    std::string mClientID;
    std::string mInterfaceName;
    std::string mDriverName;
    std::string mProbeName;
    std::string mBaseTopic;
    std::string mBaseTopicCmd;
    std::string mBaseTopicAtts;

    /// This list of object is used to communicate with the broker
    std::shared_ptr<mqtt::async_client> mClientMqtt;

    mqtt::connect_options connOpts;

    std::string mBehaviour;

    std::mutex mPubMutex;
};

/// Meta Drivers Factory template
class MetaDriverFactory
{
public:
    /// constructor
    MetaDriverFactory(){};

    /// virtual createDriver function
    virtual std::shared_ptr<MetaDriver> createDriver(void *arg) = 0;
};

#endif