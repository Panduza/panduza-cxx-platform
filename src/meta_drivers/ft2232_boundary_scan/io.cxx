/**
 * @file   io.cpp
 * @brief  Manages and keeps informations about each IO
 * @author Adel
 */

#include "io.hxx"

// Io::Io(int pin_id, MetaDriverFT2232Io* Jc){
//     char tmp_name[128];
//     /// Gets name of IO from its ID
//     jtagcore_get_pin_properties(Jc->getJc(), 0, pin_id, tmp_name, sizeof(tmp_name), 0);		
//     std::string str(tmp_name);

//     /// Gives default values to the Io
//     mName = str;
//     mId = pin_id;
//     mDirection = "unknown";
//     mState = -1;
//     mSavedState = -1;
//     mReadState = 1;
// }

Io::Io(std::string pin_name, MetaDriverFT2232Io* Jc){
    /// Gets ID of IO from its name
    mId = jtagcore_get_pin_id(Jc->getJc(), 0, pin_name.data());

    /// Gives default values to the Io
    mName = pin_name;
    mDirection = "unknown";
    mState = -1;
    mSavedState = -1;
    mReadState = 1;
}

Io::Io(std::string name, int val){
    mName = name;
    mId = val;
}

Io& getIo(std::string name, MetaDriverFT2232Io* jc){
    loguru::set_thread_name("getIo");	

    Io* Pin = jc->getPin();

    if( Pin->getName() == name ){
        return *Pin;
    }
    
    LOG_F(ERROR, "Io %s retained but not declared",name.c_str());
    exit(0);
    // LOG_F(WARNING, "Io %s retained but not declared",name.c_str());
    // Io* err = new Io(name,-2);
    // return *err;
}

void Io::editPin(std::string name, MetaDriverFT2232Io* jc){

    Io* Pin = jc->getPin();

    if( Pin->getName() == name ){
        Pin = this;
    }
}

void Io::printPin(){
    loguru::set_thread_name("printPin()");
    LOG_F(INFO, "Name of the pin : %s",(this->mName).c_str());
    LOG_F(INFO, "Id of the pin : %d",(this->mId));
    LOG_F(INFO, "Direction of the pin : %s",(this->mDirection).c_str());
    LOG_F(INFO, "State of the pin : %d",(this->mState));
}

std::string Io::getName(){
    return mName;
}

int Io::getId(){
    return mId;
}

std::string Io::getDirection(){
    return mDirection;
}

void Io::setDirection(std::string direction){
    mDirection = direction;;
}

void Io::setState(int state){
    mState = state;
}

int Io::getState(){
    return mState;
}

int Io::getSavedState(){
    return mSavedState;
}

int Io::getRead(){
    return mReadState;
}

void Io::setReadState(int val){
    mReadState = val;
}

void Io::setSavedState(int savedState){
    mSavedState = savedState;
}