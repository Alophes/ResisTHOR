#include "rfid.h"

void rfidInit() {
    Serial2.begin(9600);
}

rfid_command rfidCommand(String tag) {
    //RFID CARD
    if (tag.equals("0F027D734043\r\n")) return rfid_command::forward;
    if (tag.equals("0E008E920B19\r\n")) return rfid_command::left;
    if (tag.equals("0F027D729391\r\n")) return rfid_command::rigth;
    if (tag.equals("0E008E9C425E\r\n")) return rfid_command::start;
    if (tag.equals("0E008E614AAB\r\n")) return rfid_command::stop;
    return rfid_command::error;
}

rfid_command rfidRead() {
    bool read = true;
    char c, incoming = 0;
    String tag;

    while (read) {
        if (Serial2.available()) {
			c = Serial2.read();
            switch (c)
            {
                case 0x02 : // START OF TRANSMIT
                    incoming=1;
                    break;
                case 0x03 : // END OF TRANSMIT
                    incoming=0;
                    read = false;
                    break;
                default:
                    if(incoming) tag += c;
                    break;
            }
		}
    }
    return rfidCommand(tag);
}