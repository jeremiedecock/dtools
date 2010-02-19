#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>

#include <dynamixel/dynamixel.hpp>

using namespace dynamixel;

static bool fullResetFlag;

void reset(Usb2Dynamixel controller, byte_t id_to_reset);

void usage(char * prog);

int main(int argc, char **argv) {

    // Getopt ///////////////////////////////////
    fullResetFlag = false;
    byte_t idToReset = 0xFF;       // Reset all actuators by default
    char controllerDevice[256] = "/dev/ttyUSB0";
    int c;
     
    opterr = 0;

    while ((c = getopt (argc, argv, "fi:d:")) != -1) {
        switch (c) {
            case 'f':
                fullResetFlag = true;
                break;
            case 'i':
                idToReset = atoi(optarg);
                break;
            case 'd':
                strcpy(controllerDevice, optarg);
                break;
            case '?':
                usage(argv[0]);
                return 1;
        }
    }

    // Dynamixel ////////////////////////////////
    try {
        // Init AX-12
        Usb2Dynamixel controller(controllerDevice);

        // Get IDs
        controller.scan_ax12s();
        std::vector<byte_t> ax12_ids = controller.ax12_ids();
        if (!ax12_ids.size()) {
            std::cerr << "no ax12 detected, exit" << std::endl;
            return 0;
        }

        // Reset AX-12
        if(idToReset == 0xFF) { // Reset all actuators
            for(size_t i = 0 ; i < ax12_ids.size() ; ++i) {
                reset(controller, ax12_ids[i]);
            }
        } else {
            std::vector<byte_t>::iterator it;
            it = std::find(ax12_ids.begin(), ax12_ids.end(), (byte_t) idToReset);
            reset(controller, *it);
        }
    } catch (Error& e) {
        std::cerr << "error : " << e.msg() << std::endl;
    }

    fprintf(stdout, "\n");

    return 0;

}

void reset(Usb2Dynamixel controller, byte_t id_to_reset) {

    Status status;

    if(fullResetFlag) {
        controller.send(ax12::Reset(id_to_reset));
        controller.recv(0.5f, status);
    } else {
        //controller.send(ax12::WriteData(id_to_reset, 0x04, 0x01));
        //controller.recv(0.5f, status); usleep(1000);
        //controller.send(ax12::WriteData(id_to_reset, 0x05, 0xfa));
        //controller.recv(0.5f, status); usleep(1000);
/*        
        ax12::WriteData packet_raw(id_to_reset, 0x19, 0x00);
        const byte_t* pw = packet_raw.packet();
        fprintf(stdout, "%02x %02x %02x %02x %02x %02x %02x %02x\n", pw[0], pw[1], pw[2], pw[3], pw[4], pw[5], pw[6], pw[7]);

        std::cout << "0x19 -> 0x00" << std::endl;
        controller.send(ax12::WriteData(id_to_reset, 0x1e, 0xff ,0x00));
        controller.recv(0.5f, status); usleep(1000);
*/


/*
        ax12::WriteData packet_raw(id_to_reset, 0x06, 0x00);
        const byte_t* pw = packet_raw.packet();
        fprintf(stdout, "%02x %02x %02x %02x %02x %02x %02x %02x\n", pw[0], pw[1], pw[2], pw[3], pw[4], pw[5], pw[6], pw[7]);
*/
        // Return Delay Time -> 0xa0 (320µs)
        std::cout << "0x05 -> 0xa0" << std::endl;
        controller.send(ax12::WriteData(id_to_reset, 0x05, 0xa0));
        controller.recv(0.5f, status); usleep(1000);

        // CW Angle Limit -> 0x00
        std::cout << "0x06 0x07 -> 0x00 0x00" << std::endl;
        controller.send(ax12::WriteData(id_to_reset, 0x06, 0x00, 0x00));
        controller.recv(0.5f, status); usleep(1000);

        // CCW Angle Limit -> 0x03ff (1023 dec, the maximum value)
        std::cout << "0x08 0x09 -> 0xff 0x03 (1023)" << std::endl;
        controller.send(ax12::WriteData(id_to_reset, 0x08, 0xff, 0x03));
        controller.recv(0.5f, status); usleep(1000);

        // The highest limit temperature -> 0x41 (61°C)
        std::cout << "0x0b -> 0x41" << std::endl;
        controller.send(ax12::WriteData(id_to_reset, 0x0b, 0x41));
        controller.recv(0.5f, status); usleep(1000);

        // The lowest limit voltage -> 0x3c (6V)
        std::cout << "0x0c -> 0x3c" << std::endl;
        controller.send(ax12::WriteData(id_to_reset, 0x0c, 0x3c));
        controller.recv(0.5f, status); usleep(1000);

        // The highest limit voltage -> 0xbe (19V)
        std::cout << "0x0d -> 0xbe" << std::endl;
        controller.send(ax12::WriteData(id_to_reset, 0x0d, 0xbe));
        controller.recv(0.5f, status); usleep(1000);

        // Max torque -> 0x038e (910 dec)
        std::cout << "0x0e 0x0f -> 0xe8 0x03      (1000)" << std::endl;
        controller.send(ax12::WriteData(id_to_reset, 0x0e, 0xe8, 0x03));
        controller.recv(0.5f, status); usleep(1000);

        // Status return level -> Respond to all instructions
        std::cout << "0x10 -> 0x02" << std::endl;
        controller.send(ax12::WriteData(id_to_reset, 0x10, 0x02));
        controller.recv(0.5f, status); usleep(1000);

        // Alarm LED -> Overheating Error
        std::cout << "0x11 -> 0x04" << std::endl;
        controller.send(ax12::WriteData(id_to_reset, 0x11, 0x04));
        controller.recv(0.5f, status); usleep(1000);

        // Alarm shutdown -> Input Voltage Error + Angle Limit Error + Overheating Error + Overload Error (the most important)
        std::cout << "0x12 -> 0x27" << std::endl;
        controller.send(ax12::WriteData(id_to_reset, 0x12, 0x27));
        controller.recv(0.5f, status); usleep(1000);
    }

}

void usage(char * prog) {
    fprintf(stderr, "usage: %s [-i actuator_id] [-d controller_device]\n", prog);
}

