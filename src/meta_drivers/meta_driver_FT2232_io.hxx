#ifndef _METADRIVERFT2232IO_
#define _METADRIVERFT2232IO_
#pragma once

#include "../meta_driver.hxx"
#include "ft2232_boundary_scan/io.hxx"
#include "ft2232_boundary_scan/jtag_core/jtag_core.hxx"
#include "ft2232_boundary_scan/jtag_manager.hxx"

class Io;
class JtagFT2232;

class MetaDriverFT2232Io : public MetaDriver
{
public:
    /// Destructor
    ~MetaDriverFT2232Io();

    /// Constructor
    MetaDriverFT2232Io(std::shared_ptr<JtagFT2232> jc);

    /// Setup the instance
    void setup();

    /// Sets a pin direction to input and reads its value
    int readInputState(Io &Io);

    /// Sets the state of an Io
    void setState(Io &Io, int state);

    /// Sets the saved state of an Io
    void setSavedState(Io &Io, int save);

    /// Sets an Io direction to output and sets its value to 1 (to the board)
    void setOutputOn(Io &Io);

    /// Sets an Io direction to output and sets its value to 0  (to the board)
    void setOutputOff(Io &Io);

    void setDirection(Io &Io, std::string direction);

    /// Gets the saved state of an Io
    int getSavedState(Io &Io);

    /// Getter of Jc object
    jtag_core *getJc() const { return mJc; };

    /// Pin getter
    /// @return Reference of the Pin
    Io *getPin();

    /// Publishes state of an Io
    /// @param Io : State of this Io is published
    int publishState(Io &Io);

    /// Publishes direction of an Io
    /// @param Io : State of this Io is published
    int publishDirection(Io &Io);

    /// Check the input of the board, this is for the thread
    void checkInput();

    /// Launched when a message arrive on the subscribed topic
    void message_arrived(mqtt::const_message_ptr msg);

    /// Send info when asked
    void sendInfo();

    /// Create an alternative thread
    std::shared_ptr<std::thread> createAlternativeThread() { return std::make_shared<std::thread>(&MetaDriverFT2232Io::checkInput, this); }

private:

    std::shared_ptr<JtagFT2232> mJtagManager;
    jtag_core *mJc;
    std::string mProbeId;

    Io *mPin;
    std::string mPinName;

    Json::Value mState;
    Json::Value mDirection;

    /// A Mutex to synchronize the publishes
    std::mutex mPubMutex;
    static std::mutex mCheckInputMutex;
    static std::mutex mMessageMutex;

    std::thread *mAlternativeThread;
    bool mKillThread = false;
};

#endif