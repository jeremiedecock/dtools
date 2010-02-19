#include <iostream>
#include <algorithm>

#include <math.h>
#include <string.h>
#include <sys/time.h>

#include <dynamixel/dynamixel.hpp>

#define TICK_INTERVAL 500000      // Nombre de us séparant deux appels
#define READ_DURATION 0.05f

using namespace dynamixel;
    
void usage(char * prog);

int main(int argc, char **argv) {

    // Getopt ///////////////////////////////////
    char controllerDevice[256] = "/dev/ttyUSB0";
    int c;
     
    opterr = 0;

    while ((c = getopt (argc, argv, "d:")) != -1) {
        switch (c) {
            case 'd':
                strcpy(controllerDevice, optarg);
                break;
            case '?':
                usage(argv[0]);
                return 1;
        }
    }

    // Dynamixel //////////////////////////////////////////////////////////////////////////////////
        
    // Init ///////////////////////////////////////////////////////////////
   
    // Set device file
    Usb2Dynamixel controller(controllerDevice);

    // Scan actuators IDs
    controller.scan_ax12s();
    const std::vector<byte_t>& actuators_IDs = controller.ax12_ids();
    if (!actuators_IDs.size()) {
        std::cerr << "no ax12 detected, exit" << std::endl;
        return 0;
    } else {
        std::cout << actuators_IDs.size() << " ax12 are connected" << std::endl;
    }
 
    // Init timers //////////////////////////
    struct timeval timev_init;  // Initial absolute time (static)
    struct timeval timev_prev;  // Previous tick absolute time
    struct timeval timev_cur;   // Current absolute time
    struct timeval timev_rel;   // Current relative time (curent absolute time - initial time)
    struct timeval timev_diff;  // Current tick position (curent absolute time - previous tick time)
   
    timerclear(&timev_init);
    gettimeofday(&timev_init, NULL);
    timev_prev = timev_init;

    // Ticks loop ///////////////////////////
    while(true) {
        gettimeofday(&timev_cur, NULL);
        timersub(&timev_cur, &timev_prev, &timev_diff);

        // On fait un step de TICK_INTERVAL ms
        if(timev_diff.tv_usec > TICK_INTERVAL || timev_diff.tv_sec > 0) {

            // Calculate relative time //////////////////////////
            timersub(&timev_cur, &timev_init, &timev_rel);

            // Move robot ///////////////////////////////////////
            for (size_t id_index = 0 ; id_index < actuators_IDs.size() ; ++id_index)
            {
                Status status;

                byte_t id = actuators_IDs[id_index];
                byte_t param1 = 0x2b;
                byte_t param2 = 0x01;

                try {
                    controller.send(ax12::ReadData(id, param1, param2));
                    controller.recv(READ_DURATION, status);
                    fprintf(stdout, "%02d ", (int) status.get_params()[0]);
                } catch (Error& e) {
                    std::cerr << "error : " << e.msg() << std::endl;
                }
            }

            fprintf(stdout, "\n");

            // Update vars //////////////////////////////////////
            timev_prev = timev_cur;
        }
    }

    return 0;
}

void usage(char * prog) {
    fprintf(stderr, "usage: %s [-d controller_device]\n", prog);
}

