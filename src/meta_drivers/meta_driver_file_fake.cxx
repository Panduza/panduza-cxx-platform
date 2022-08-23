#include "meta_driver_file_fake.hxx"
#include "../base64/base64.hxx"
#include <fstream>


MetaDriverFileFake::MetaDriverFileFake()
{
    // Pass the thread flag to True
    mNeedThread = true;
}

// ============================================================================
//
void MetaDriverFileFake::setup()
{
    // Subscribe to the different topic needed direction and value separated because of retained not coming in the good order
    subscribe(getBaseTopic() + "/Bsdl_File/cmds/#", 0);
    subscribe(getBaseTopic() + "/Bsdl_File/atts/content", 0);
    subscribe(getBaseTopic() + "/Bsdl_File/atts/data", 0);

    LOG_F(ERROR, "%s.bsdl",getDriverName().c_str());
    // Try to open the BSDL
    std::ifstream bsdl_file(getDriverName() + ".bsdl", std::ifstream::binary);

    // If it does open, then start the launch of Ios
    if (bsdl_file.is_open())
    {
        LOG_F(INFO, "BSDL File found, loading the BSDL...");
        
        bsdl_file.close();
    }
}

// ============================================================================
//
void MetaDriverFileFake::sendInfo()
{
    // Create the info payload
    Json::Value info;
    info["type"] = "file";
    info["version"] = "1.0";

    LOG_F(4, "Info sent is : %s", info.toStyledString().c_str());

    // publish the message info to the mqtt server for the pin
    publish(getBaseTopic() + "/Bsdl_File/info", info, 0, false);
}

// ============================================================================
//
void MetaDriverFileFake::message_arrived(mqtt::const_message_ptr msg)
{

    loguru::set_thread_name("Sub callback");
    Json::Value root;
    Json::Reader reader;
    std::string SubDir, val, driver = getDriverName() + "/";
    std::size_t start;

    // If the message topid is "pza" and the payload is "*"
    if ((msg->get_topic().compare("pza") == 0) && (msg->get_payload().compare("*") == 0))
    {
        LOG_F(3, "Sending infos");
        sendInfo();
    }
    else
    {
        Json::Value parsedMsg = parseMsg(msg->get_payload());

        if (msg->get_topic().find("/cmds/content/set") != std::string::npos)
        {
            int content_size = parsedMsg["data"].asString().size();
            parsedMsg["mime"] = "text/plain";
            
            // Get CRC32 encoded message and gets its checksum
            boost::crc_32_type crc32;
            crc32.process_bytes(parsedMsg["data"].asString().data(),content_size);
            std::stringstream crc32hex;
            crc32hex << std::hex << crc32.checksum();

            // Create one of the payload for atts
            Json::Value payload;
            payload["size"] = content_size;
            payload["crc"] = crc32hex.str();
            
            // Send payloads to atts
            publish(getBaseTopic() + "/Bsdl_File/atts/metadata", payload, 0, true);
            publish(getBaseTopic() + "/Bsdl_File/atts/content", parsedMsg, 0, true);

            // Get encoded message
            std::string encoded_message = parsedMsg["data"].asString();

            // Decode the payload
            mDecodedBSDL = base64_decode(encoded_message);

            // Create BSDL file and write the decoded content
            mBSDLName = getDriverName() +".bsdl";
            std::ofstream BSDLFile(mBSDLName);
            BSDLFile << mDecodedBSDL;
            BSDLFile.close();
        }
    }
}

// ============================================================================
//

std::shared_ptr<MetaDriver> MetaDriverFactoryFileFake::createDriver(void *arg)
{
    std::shared_ptr<MetaDriver> MetaDriverFactoryFileFakeInstance = std::make_shared<MetaDriverFileFake>();

    return MetaDriverFactoryFileFakeInstance;
}