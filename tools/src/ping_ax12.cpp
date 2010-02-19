#include <iostream>
#include <string.h>
#include <dynamixel/dynamixel.hpp>

void usage(char * prog);

int main(int argc, char **argv) {

    // Getopt ///////////////////////////////////
    int baudrate = B115200;       // Reset all actuators by default
    char controllerDevice[256] = "/dev/ttyUSB0";
    int c;
     
    opterr = 0;

    while ((c = getopt (argc, argv, "d:b:")) != -1) {
        switch (c) {
            case 'b':
                baudrate = atoi(optarg);
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
    using namespace dynamixel;

    try {

        // Set device file
        Usb2Dynamixel controller(controllerDevice, baudrate);
        
        // Set device file
        controller.scan_ax12s();
        if (!controller.ax12_ids().size()) {
            std::cerr << "no ax12 detected, exit" << std::endl;
            return 0;
        } else {
            for (size_t i = 0 ; i < controller.ax12_ids().size() ; ++i) {
                std::cout << (int) controller.ax12_ids()[i] << " -> ok" << std::endl;
            }
        }
        
    } catch (Error& e) {
        std::cerr << "error : " << e.msg() << std::endl;
    }

    return 0;
}

void usage(char * prog) {
    fprintf(stderr, "usage: %s [-b baudrate] [-d controller_device]\n", prog);
}
