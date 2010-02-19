#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>

#include <dynamixel/dynamixel.hpp>

using namespace dynamixel;

static bool verboseFlag;

void dump(Usb2Dynamixel controller, byte_t id_to_dump);

void usage(char * prog);

int main(int argc, char **argv) {

    // Getopt ///////////////////////////////////
    verboseFlag = false;
    byte_t idToDump = 0xFF;       // Dump all actuators by default
    char controllerDevice[256] = "/dev/ttyUSB0";
    int c;
     
    opterr = 0;

    while ((c = getopt (argc, argv, "vi:d:")) != -1) {
        switch (c) {
            case 'v':
                verboseFlag = true;
                break;
            case 'i':
                idToDump = atoi(optarg);
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

        // Print header
        if(!verboseFlag) {
            fprintf(stdout, "\n          | ");
            for(int i=0 ; i<50 ; i++) fprintf(stdout, "%02x ", i);
            fprintf(stdout, "\n");

            fprintf(stdout, "----------+");
            for(int i=0 ; i<50 ; i++) fprintf(stdout, "---");
            fprintf(stdout, "\n");
        }

        // Dump AX-12
        if(idToDump == 0xFF) { // Dump all actuators
            for (size_t i = 0 ; i < ax12_ids.size() ; ++i) {
                dump(controller, ax12_ids[i]);

                fprintf(stdout, "\n");
            }
        } else {
            std::vector<byte_t>::iterator it;
            it = std::find(ax12_ids.begin(), ax12_ids.end(), (byte_t) idToDump);
            
            dump(controller, *it);
            fprintf(stdout, "\n");
        }
    } catch (Error& e) {
        std::cerr << "error : " << e.msg() << std::endl;
    }

    fprintf(stdout, "\n");

    return 0;

}

void dump(Usb2Dynamixel controller, byte_t id_to_dump) {

    if(verboseFlag) std::cout << "Dump AX-12 #" << (int) id_to_dump << std::endl;
    else fprintf(stdout, "AX-12 #%02d | ", (int) id_to_dump);

    for(byte_t address_to_read = 0x00 ; (int) address_to_read < 0x32 ; address_to_read++) {
        byte_t param1 = address_to_read;
        byte_t param2 = 0x01;
        controller.send(ax12::ReadData(id_to_dump, param1, param2));

        Status status;
        controller.recv(0.5f, status);

        if(verboseFlag) fprintf(stdout, "%#04x %#04x\n", (int) address_to_read, (int) status.get_params()[0]);
        else fprintf(stdout, "%02x ", (int) status.get_params()[0]);
        
        usleep(1000); // Suspend execution for n microseconds
    }

}


void usage(char * prog) {
    fprintf(stderr, "usage: %s [-v] [-i actuator_id] [-d controller_device]\n", prog);
}

