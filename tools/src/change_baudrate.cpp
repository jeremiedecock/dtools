#include <iostream>
#include <dynamixel/dynamixel.hpp>

int main(int argc, char **argv)
{
    using namespace dynamixel;

    try
    {
        // Init AX-12
        Usb2Dynamixel controller(argv[1], B57600);
        std::cout << "dynamixel init ok" << std::endl;

        // Get IDs
        controller.scan_ax12s();
        std::vector<byte_t> ax12_ids = controller.ax12_ids();
        if (!ax12_ids.size())
        {
            std::cerr << "no ax12 detected, exit" << std::endl;
            return 0;
        }

        // Change baudrates for all AX-12
        for (size_t i = 0 ; i < ax12_ids.size() ; ++i)
        {
            std::cout << "changing baud rate of :" << (int) ax12_ids[i] << std::endl;
            std::vector<byte_t> params;
            params.push_back(ax12::ctrl::baudrate);
            params.push_back(0x10);
            controller.send(ax12::WriteData(broadcast, params));
            std::cout << "done" << std::endl;
            sleep(1);
        }
    }
    catch (Error& e)
    {
        std::cerr << "error : " << e.msg() << std::endl;
    }
    return 0;
}
