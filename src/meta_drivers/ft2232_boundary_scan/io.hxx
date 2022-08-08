/**
 * @file   io.hpp
 * @brief  Manages and keeps informations about each IO
 * @author Adel
 */

#ifndef _IO_
#define _IO_

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

// #include "jtag_manager.hxx"
#include "../meta_driver_FT2232_io.hxx"

class MetaDriverFT2232Io;

///Io class that represents a physical Io
class Io{
    public:
        ///Constructor of Io object from its pin ID
        // Io(int pin_id, MetaDriverFT2232Io* Jc);

        ///Constructor of Io object from its pin name
        Io(std::string pin_name, MetaDriverFT2232Io* Jc);

        ///Constructor of Io object to contain name and value
        Io(std::string name, int val);
        
        ///Getter of Io's ID
        /// @return ID of the Io
        int getId();

        ///Getter of Io's state
        /// @return State of the Io
        int getState();

        ///Getter of Io's state
        /// @return State of the Io
        void setState(int state);

        ///Getter of Io's saved state
        /// @return Saved State of the Io
        int getSavedState();

        ///Setter of Io's saved state
        /// @return State of the Io
        void setSavedState(int savedState);

        ///Getter of Io's name (ex. IO_M5)
        /// @return Name of the Io
        std::string getName();

        ///Getter of Io's direction 
        /// @return Direction of the Io
        std::string getDirection();

        ///Setter of Io's direction 
        /// @return Direction of the Io
        void setDirection(std::string direction);

        ///Getter of read state of input Io
        /// @return Read state of the Io
        int getRead();

        ///Prints the members of an Io
        void printPin();

        ///Setter of read state of input Io
        /// @param val : Read state of the Io
        void setReadState(int val);
        
        ///Edits an Io in the vector	
        /// @param name : Name of the Io to edit
        void editPin(std::string name, MetaDriverFT2232Io* jc);

    private:
        int mId;
        int mState;
        std::string mName;
        std::string mDirection;
        int mSavedState;
        int mReadState;
        
};

///Getter of reference to an IO object
/// @param name : Name of the Io
/// @return Reference to IO object
Io& getIo(std::string name, MetaDriverFT2232Io* jc);

#endif