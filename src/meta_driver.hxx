#ifndef _METADRIVER_
#define _METADRIVER_
#pragma once

#include <iostream>
#include <jsoncpp/json/json.h>
#include <algorithm>

#include "loguru/loguru.hxx"
#include "mqtt/async_client.h"

class MetaDriver : public virtual mqtt::callback, public virtual mqtt::iaction_listener
{
public:
    /// Flag if the driver need another thread
    bool mNeedThread = false;
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
    /// Callback for when the connection is lost.
    /// This will initiate the attempt to manually reconnect.
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

    /// Getter and Setter
    void setInterfaceTree(Json::Value interface_tree) { mInterfaceTree = interface_tree; };
    void setMachineName(std::string machine_name) { mMachineName = machine_name; };
    void setBrokerName(std::string broker_name) { mBrokerName = broker_name; };
    void setBrokerAddr(std::string broker_addr) { mBrokerAddr = broker_addr; };
    void setBrokerPort(std::string broker_port) { mBrokerPort = broker_port; };
    void setClientID(std::string client_id) { mClientID = client_id; };
    void setInterfaceName(std::string interface_name) { mInterfaceName = interface_name; };
    void setDriverName(std::string driver_name) { mDriverName = driver_name; };
    void setProbeName(std::string probe_name) { mProbeName = probe_name; };
    void setBaseTopic() { mBaseTopic = "pza/" + mMachineName + "/" + mInterfaceTree["driver"].asString(); };
    void setBaseTopicCmds() { mBaseTopicCmd = mBaseTopic + "/cmds"; };
    void setBaseTopicAtts() { mBaseTopicAtts = mBaseTopic + "/atts"; };
    void setBehaviour();
    const Json::Value &getInterfaceTree() const { return mInterfaceTree; };
    const std::string &getMachineName() const { return mMachineName; };
    const std::string &getBrokerName() const { return mBrokerName; };
    const std::string &getBrokerAddr() const { return mBrokerAddr; };
    const std::string &getBrokerPort() const { return mBrokerPort; };
    const std::string &getDriverName() const { return mDriverName; };
    const std::string &getProbeName() const { return mProbeName; };
    const std::string &getBaseTopic() const { return mBaseTopic; };
    const std::string &getBaseTopicCmds() const { return mBaseTopicCmd; };
    const std::string &getBaseTopicAtts() const { return mBaseTopicAtts; };
    const std::string &getBehaviour() const { return mBehaviour; };
    const std::shared_ptr<mqtt::async_client> &getClientMqtt() const { return mClientMqtt; };
    void setBehaviour(std::string behaviour) { mBehaviour = behaviour; };

private:
    int mRetry = 5;

    /// Json needed
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

class MetaDriverFactory
{
public:
    /// constructor
    MetaDriverFactory(){};

    /// virtual createDriver function
    virtual std::shared_ptr<MetaDriver> createDriver(void *arg) = 0;
};

#endif