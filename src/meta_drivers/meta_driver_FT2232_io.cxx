#include "meta_driver_FT2232_io.hxx"
#include "meta_driver_FT2232_boundary_scan.hxx"

std::mutex MetaDriverFT2232Io::mCheckInputMutex;
std::mutex MetaDriverFT2232Io::mMessageMutex;

MetaDriverFT2232Io::~MetaDriverFT2232Io()
{
    mKillThread = true;

    mAlternativeThread->join();
    delete mAlternativeThread;
    delete mPin;

    mKillThread = false;
}

MetaDriverFT2232Io::MetaDriverFT2232Io(std::shared_ptr<JtagFT2232> jc)
{
    mJtagManager = jc;
}

void MetaDriverFT2232Io::setup()
{
    mNeedThread = true;
    mJc = mJtagManager->getJc();
    mProbeId = mJtagManager->getProbeId();

    // Get the name of the pin to use it on the function
    const std::string pin_name = "IO_" + getInterfaceTree()["settings"]["pin"].asString();
    LOG_F(4, "driver instance for pin : %s", pin_name.c_str());

    // Load the pin
    Io *Pin = new Io(pin_name, this);
    mPin = Pin;
    mPinName = pin_name;

    // Subscribe to the different topic needed direction and value separated because of retained not coming in the good order
    subscribe(getBaseTopic() + "/" + pin_name + "/cmds/#", 0);
    subscribe(getBaseTopic() + "/" + pin_name + "/atts/direction", 0);
    subscribe(getBaseTopic() + "/" + pin_name + "/atts/value", 0);

    mAlternativeThread = new std::thread(&MetaDriverFT2232Io::checkInput, this);
}

int MetaDriverFT2232Io::readInputState(Io &Io)
{
    // Return -1 if there is an error
    if (jtagcore_push_and_pop_chain(mJc, JTAG_CORE_WRITE_READ) < 0)
    {
        return -1;
    }

    // Return the actual state
    return jtagcore_get_pin_state(mJc, 0, Io.getId(), JTAG_CORE_INPUT);
}

void MetaDriverFT2232Io::setState(Io &Io, int state)
{
    // Set the state of the Io
    Io.setState(state);
}

void MetaDriverFT2232Io::setSavedState(Io &Io, int save)
{
    // Set the saved state of the Io
    Io.setSavedState(save);
}

void MetaDriverFT2232Io::setOutputOn(Io &Io)
{
    // Set Output to On
    jtagcore_set_pin_state(mJc, 0, Io.getId(), JTAG_CORE_OUTPUT, 1);
    jtagcore_set_pin_state(mJc, 0, Io.getId(), JTAG_CORE_OE, 1);
    jtagcore_push_and_pop_chain(mJc, JTAG_CORE_WRITE_ONLY);
}

void MetaDriverFT2232Io::setOutputOff(Io &Io)
{
    // Set Output to Off
    jtagcore_set_pin_state(mJc, 0, Io.getId(), JTAG_CORE_OUTPUT, 0);
    jtagcore_set_pin_state(mJc, 0, Io.getId(), JTAG_CORE_OE, 0);
    jtagcore_push_and_pop_chain(mJc, JTAG_CORE_WRITE_ONLY);
}

void MetaDriverFT2232Io::setDirection(Io &Io, std::string direction)
{
    // Set the direction of the Io
    Io.setDirection(direction);
}

int MetaDriverFT2232Io::getSavedState(Io &Io)
{
    return Io.getSavedState();
}

Io *MetaDriverFT2232Io::getPin()
{
    return mPin;
}

int MetaDriverFT2232Io::publishState(Io &Io)
{
    mPubMutex.lock();

    std::string PUB_TOPIC_VALUE = getBaseTopic() + "/" + Io.getName() + "/atts/value";
    mState["value"] = Io.getState();
    mqtt::message_ptr PubVal = mqtt::make_message(PUB_TOPIC_VALUE, mState.toStyledString());

    publish(PUB_TOPIC_VALUE, mState, 0, true);

    mPubMutex.unlock();

    return 0;
}

int MetaDriverFT2232Io::publishDirection(Io &Io)
{
    mPubMutex.lock();

    std::string PUB_TOPIC_DIRECTION = getBaseTopic() + "/" + Io.getName() + "/atts/direction";
    mDirection["direction"] = Io.getDirection();

    publish(PUB_TOPIC_DIRECTION, mDirection, 0, true);

    mPubMutex.unlock();

    return 0;
}

void MetaDriverFT2232Io::checkInput()
{
    loguru::set_thread_name("MetaDriverIo");

    while (!mKillThread)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        /// Parse IO vector, search for IO input, keeps track of their actual state and checks for changes to publish them.
        mCheckInputMutex.lock();
        if (mPin->getDirection() == "in")
        {
            // LOG_F(8, "The pin %s have it read value as : %d", mPin->getName().c_str(), mPin->getRead());
            ///	If the pin state hasnt been alrady read, we read it and mark as read
            if (mPin->getRead() == 0)
            {
                setSavedState(*mPin, readInputState(*mPin));
                setState(*mPin, readInputState(*mPin));

                mPin->setReadState(1);
            }

            ///	If the saved state and actual state are different, we publish state and mark as not read yet
            if ((mPin->getState() != readInputState(*mPin)) && (readInputState(*mPin) >= 0))
            {
                setState(*mPin, readInputState(*mPin));
                publishState(*mPin);

                mPin->setReadState(0);
            }
            if (readInputState(*mPin) < 0)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        mCheckInputMutex.unlock();
    }
}

void MetaDriverFT2232Io::message_arrived(mqtt::const_message_ptr msg)
{
    loguru::set_thread_name("Sub callback");
    Json::Value root;
    Json::Reader reader;
    std::string SubDir, val, PinName, driver = getDriverName() + "/";
    std::size_t start;
    int SubVal;

    VLOG_SCOPE_F(2, "Message arrived in Boundary Scan");

    // Log the topic and payload
    LOG_F(3, "Topic: %s", msg->get_topic().c_str());
    LOG_F(3, "Payload: %s", msg->to_string().c_str());

    mMessageMutex.lock();
    // if the topic is pza, send the info
    if ((msg->get_topic().compare("pza") == 0) && (msg->get_payload().compare("*") == 0))
    {
        LOG_F(3, "Sending infos");
        sendInfo();
    }
    else
    {
        /// Parse the message, get the name of the IO from it and the object associated to that IO
        reader.parse(msg->to_string().c_str(), root);

        /// topic is ex. : "pza/machine/driver/LED_RGB_B1/cmds/value/set", extract LED_RGB_B1.
        start = (msg->get_topic()).find(driver) + driver.length();

        if (msg->get_topic().find("/atts") != std::string::npos)
            PinName = msg->get_topic().substr(start, (msg->get_topic().find("/atts") - start));
        else if (msg->get_topic().find("/cmds") != std::string::npos)
            PinName = msg->get_topic().substr(start, (msg->get_topic().find("/cmds") - start));

        Io &pin = getIo(PinName, this);

        /// If message contains "direction", get it
        if (msg->get_topic().find("direction") != std::string::npos)
        {
            SubDir = root.get("direction", "").asString();
            /// If the direction of the pin is different then the direction published in the topic
            if (SubDir != pin.getDirection())
            {
                /// If direction is "input" or "output", edit the object and publish its direction
                if (SubDir == "in")
                {
                    LOG_F(INFO, "Setting %s as input", pin.getName().c_str());

                    setDirection(pin, "in");
                    publishDirection(pin);
                    setState(pin, getSavedState(pin));

                    pin.editPin(pin.getName(), this);
                }
                else if (SubDir == "out")
                {
                    LOG_F(INFO, "Setting %s as output", pin.getName().c_str());

                    setDirection(pin, "out");
                    publishDirection(pin);

                    pin.editPin(pin.getName(), this);
                }
                else
                {
                    LOG_F(WARNING, "Expecting in or out as pin direction");
                }
            }
        }
        /// If the message contains "value" and IO's direction is output, get the value
        if (((msg->get_topic().find("value")) != (std::string::npos)) && (pin.getDirection() == "out"))
        {
            val = root.get("value", "").asString();
            SubVal = stoi(val);
            /// If the state of the pin is different then the state published in the topic
            if (SubVal != pin.getState())
            {
                /// If value = 0/1, edit the object and publish its state
                if (SubVal == 0)
                {
                    LOG_F(INFO, "Setting %s to state %d", pin.getName().c_str(), 0);

                    setOutputOff(pin);
                    setState(pin, 0);
                    publishState(pin);
                }
                else if (SubVal == 1)
                {
                    LOG_F(INFO, "Setting %s to state %d", pin.getName().c_str(), 1);

                    setOutputOn(pin);
                    setState(pin, 1);
                    publishState(pin);
                }
                else
                {
                    LOG_F(WARNING, "Expecting 0 or 1 as pin value");
                }
                pin.editPin(pin.getName(), this);
            }
        }
    }
    mMessageMutex.unlock();
}

void MetaDriverFT2232Io::sendInfo()
{
    // Create the info payload
    Json::Value info;
    info["type"] = "io";
    info["version"] = "1.0";

    LOG_F(4, "Info sent is : %s", info.toStyledString().c_str());

    // publish the message info to the mqtt server for the pin
    publish(getBaseTopic() + "/" + mPinName + "/info", info, 0, false);
}