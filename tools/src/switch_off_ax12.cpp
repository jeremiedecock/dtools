#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>

#include <dynamixel/dynamixel.hpp>

using namespace dynamixel;

void switch_off(Usb2Dynamixel controller, byte_t id_to_switch_off);

void usage(char * prog);

int main(int argc, char **argv) {

    // Getopt ///////////////////////////////////
    byte_t idToSwitch = 0xFF;       // Reset all actuators by default
    char controllerDevice[256] = "/dev/ttyUSB0";
    int c;
     
    opterr = 0;

    while ((c = getopt (argc, argv, "i:d:")) != -1) {
        switch (c) {
            case 'i':
                idToSwitch = atoi(optarg);
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
        if(idToSwitch == 0xFF) { // Reset all actuators
            for(size_t i = 0 ; i < ax12_ids.size() ; ++i) {
                switch_off(controller, ax12_ids[i]);
            }
        } else {
            std::vector<byte_t>::iterator it;
            it = std::find(ax12_ids.begin(), ax12_ids.end(), (byte_t) idToSwitch);
            switch_off(controller, *it);
        }
    } catch (Error& e) {
        std::cerr << "error : " << e.msg() << std::endl;
    }

    fprintf(stdout, "\n");

    return 0;

}

void switch_off(Usb2Dynamixel controller, byte_t id_to_switch_off) {

    Status status;

    controller.send(ax12::WriteData(id_to_switch_off, 0x18, 0x00));
    controller.recv(0.5f, status); usleep(1000);
    std::cout << "AX-12 #" << (int) id_to_switch_off << " : torque disabled" << std::endl;

}

void usage(char * prog) {
    fprintf(stderr, "usage: %s [-i actuator_id] [-d controller_device]\n", prog);
}

