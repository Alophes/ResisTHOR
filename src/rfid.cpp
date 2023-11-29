#include "rfid.h"

void rfidInit()
{
    Serial2.begin(9600);
}

rfid_command rfidCommand(String tag)
{
    // RFID CARD
    if (tag.equals("0F027D734043\r\n"))
        return rfid_command::fOrward;
    if (tag.equals("0E008E920B19\r\n"))
        return rfid_command::left;
    if (tag.equals("0F027D729391\r\n"))
        return rfid_command::rigth;
    if (tag.equals("0E008E9C425E\r\n"))
        return rfid_command::scan;
    if (tag.equals("0E008E614AAB\r\n"))
        return rfid_command::stop;

    // RFID CHIP
    if (tag.equals("48007378FFBC\r\n"))
        return rfid_command::error;
    if (tag.equals("1600142C002E\r\n"))
        return rfid_command::error;
    if (tag.equals("48007479D297\r\n"))
        return rfid_command::error;
    if (tag.equals("48007593EF41\r\n"))
        return rfid_command::error;
    if (tag.equals("000088A399B2\r\n"))
        return rfid_command::error;

    return rfid_command::error;
}

rfid_command rfidRead()
{
    bool read = true;
    char c, incoming = 0;
    String tag;

    while (read)
    {
        if (Serial2.available())
        {
            c = Serial2.read();
            switch (c)
            {
            case 0x02: // START OF TRANSMIT
                incoming = 1;
                break;
            case 0x03: // END OF TRANSMIT
                incoming = 0;
                read = false;
                break;
            default:
                if (incoming)
                    tag += c;
                break;
            }
        }
    }
    return rfidCommand(tag);
}

void rfidPrintCommand(rfid_command command)
{
    switch (command)
    {
    case rfid_command::fOrward:
        Serial.println("FORWARD");
        break;
    case rfid_command::left:
        Serial.println("LEFT");
        break;
    case rfid_command::rigth:
        Serial.println("RIGHT");
        break;
    case rfid_command::scan:
        Serial.println("SCAN");
        break;
    case rfid_command::stop:
        Serial.println("STOP");
        break;
    case rfid_command::error:
        Serial.println("ERROR");
        break;
    default:
        Serial.println("DEFAULT");
        break;
    }
}

int rfidReturnCommand(rfid_command command)
{
    switch (command)
    {
    case rfid_command::fOrward:
        return FORWARD;
        break;
    case rfid_command::left:
        Serial.println("LEFT");
        return TURNLEFT;
        break;
    case rfid_command::rigth:
        Serial.println("RIGHT");
        return TURNRIGHT;
        break;
    case rfid_command::scan:
        Serial.println("STOP/SCAN");
        return SCAN;
        break;
    case rfid_command::stop:
        Serial.println("STOP");
        return STOP;
        break;
    case rfid_command::error:
        Serial.println("ERROR");
        break;
    default:
        Serial.println("DEFAULT");
        break;
    }
}