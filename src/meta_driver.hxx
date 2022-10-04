#ifndef _METADRIVER_
#define _METADRIVER_
#pragma once

#include <iostream>
#include <jsoncpp/json/json.h>
#include <algorithm>
#include <fstream>

#include "loguru/loguru.hxx"
#include "mqtt/async_client.h"

/// Meta Drivers template
class MetaDriver : public virtual mqtt::callback, public virtual mqtt::iaction_listener
{
public:
    /// Retry count on reconnect, resetted when successful
    int mRetryCnt = 0;

    /// Callback functions on failure @param tok Token of the MQTT connection
    void on_failure(const mqtt::token &tok) override;
    /// Callback function on success @param tok Token of the MQTT connection
    void on_success(const mqtt::token &tok) override;
    /// Callback function when mqtt need to reconnect
    void reconnect();
    /// (Re)connection success @param cause Success connection message
    void connected(const std::string &cause) override;
    /// Callback for when the connection is lost. This will initiate the attempt to manually reconnect. @param cause Reason of the connection lost
    void connection_lost(const std::string &cause) override;
    /// Callback function when a message arrives. @param msg Received message
    virtual void message_arrived(mqtt::const_message_ptr msg) override;
    /// Called when delivery for a message has been completed, and all acknowledgments have been received. @param token Token of the MQTT connection
    void delivery_complete(mqtt::delivery_token_ptr token) override;

    /// Constructor
    MetaDriver() { LOG_F(9, "Meta Driver Constructor"); };

    /// creating mqtt connection, connects it and launch setup function of the Meta driver
    virtual void run();

    /// add datas to variables of the meta driver @param machine_name Name of the Machine @param broker_name Name of the Broker @param broker_addr address of the broker @param broker_port Port of the broker @param config_json Data of the interface in Json format
    void initialize(std::string machine_name, std::string broker_name, std::string borker_addr, std::string broker_port, Json::Value config_json);

    /// create an virtual function that will be empty or not depending of the meta driver loading
    virtual void setup();

    /// send info json when needed with type and version
    virtual void sendInfo();

    /// publish function : send the payload into the mqtt broker @param topic Topic to publish the payload @param payload Information to send @param qoS Quality of Service @param retained Enable message retained by the broker
    void publish(std::string topic, Json::Value payload, int qoS, bool retained);

    /// parse the received payload @param payload Information to parse @return Parsed payload
    Json::Value parseMsg(std::string payload);

    /// create an alternative thread to be call in meta platform to be launched @return thread to run in parallel
    virtual std::shared_ptr<std::thread> createAlternativeThread(){}

    /// subscribe to topics @param topic Topic to subscribe @param qoS Quality of Service
    virtual void subscribe(std::string topic, int qoS);

    /// Setter of the interface tree @param interface_tree tree of the interface in Json format
    void setInterfaceTree(Json::Value interface_tree) { mInterfaceTree = interface_tree; };
    /// Setter of the machine name @param machine_name Name of the machine
    void setMachineName(std::string machine_name) { mMachineName = machine_name; };
    /// Setter of the broker name @param broker_name Name of the broker
    void setBrokerName(std::string broker_name) { mBrokerName = broker_name; };
    /// Setter of the broker address @param broker_addr Address of the broker
    void setBrokerAddr(std::string broker_addr) { mBrokerAddr = broker_addr; };
    /// Setter of the broker port @param broker_port Port of the broker
    void setBrokerPort(std::string broker_port) { mBrokerPort = broker_port; };
    /// Setter of the cliend id @param client_id Client ID for the MQTT connection
    void setClientID(std::string client_id) { mClientID = client_id; };
    /// Setter of the interface name @param interface_name Name of the interface
    void setInterfaceName(std::string interface_name) { mInterfaceName = interface_name; };
    /// Setter of the driver name @param driver_name Name of the driver
    void setDriverName(std::string driver_name) { mDriverName = driver_name; };
    /// Setter of the probe name @param probe_name Name of the probe
    void setProbeName(std::string probe_name) { mProbeName = probe_name; };
    /// Setter of the base topic
    void setBaseTopic();
    /// Setter of the base topic with argument @param topic topic to set in the base topic
    void setBaseTopic(std::string topic) { mBaseTopic = topic; };
    /// Setter of the base commands topic
    void setBaseTopicCmds() { mBaseTopicCmd = mBaseTopic + "/cmds"; };
    /// Setter of the base attributs topic
    void setBaseTopicAtts() { mBaseTopicAtts = mBaseTopic + "/atts"; };
    /// Setter of the behaviour parameter
    void setBehaviour();
    /// Setter of the behaviour parameter by defining the parameter @param behaviour Behaviour of the meta driver
    void setBehaviour(std::string behaviour) { mBehaviour = behaviour; };
    /// Getter of the interface tree @return Interface tree in Json format
    const Json::Value &getInterfaceTree() const { return mInterfaceTree; };
    /// Getter of the machine name @return Name of the machine
    const std::string &getMachineName() const { return mMachineName; };
    /// Getter of the broker name @return Name of the Broker
    const std::string &getBrokerName() const { return mBrokerName; };
    /// Getter of the broker addr @return Address of the broker
    const std::string &getBrokerAddr() const { return mBrokerAddr; };
    /// Getter of the broker port @return Port of the broker
    const std::string &getBrokerPort() const { return mBrokerPort; };
    /// Getter of the driver name @return Name of the driver
    const std::string &getDriverName() const { return mDriverName; };
    /// Getter of the probe name @return Name of the probe
    const std::string &getProbeName() const { return mProbeName; };
    /// Getter of the base topic @return Base topic of the interface
    const std::string &getBaseTopic() const { return mBaseTopic; };
    /// Getter of the base commands topic @return Base command topic of the interface
    const std::string &getBaseTopicCmds() const { return mBaseTopicCmd; };
    /// Getter of the base attributs topic @return Base attributs topic of the interface
    const std::string &getBaseTopicAtts() const { return mBaseTopicAtts; };
    /// Getter of the behaviour parameter @return behaviour configured
    const std::string &getBehaviour() const { return mBehaviour; };
    /// Getter of the Mqtt Client @return Shared ptr of the MQTT Client
    const std::shared_ptr<mqtt::async_client> &getClientMqtt() const { return mClientMqtt; };

    virtual Json::Value generateAutodetectInfo();

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