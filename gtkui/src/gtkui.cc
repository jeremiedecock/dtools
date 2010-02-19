#include <gtk/gtk.h>
#include <stdlib.h>
#include <iostream>
#include <dynamixel/dynamixel.hpp>
 
using namespace dynamixel;

Usb2Dynamixel controller("/dev/ttyUSB0"); 

extern "C"
{
    void on_window_destroy(GtkObject *object, gpointer data)
    {
        gtk_main_quit();
    }
    
    void on_hscale_puppet_speed_value_changed(GtkObject *object, gpointer data)
    {
        controller.scan_ax12s();
        const std::vector<byte_t>& ax12_ids = controller.ax12_ids();

        int moving_speed = (int) gtk_range_get_value(GTK_RANGE(object));
        byte_t moving_speed_L = moving_speed & 0x00FF;
        byte_t moving_speed_H = (moving_speed & 0xFF00) >> 8;

        //g_print("H : %#04x   -   L : %#04x\n", moving_speed_H, moving_speed_L);

        controller.send(ax12::WriteData(0xfe, 0x20, moving_speed_L, moving_speed_H));
        /*for (size_t i = 0 ; i < controller.ax12_ids().size() ; ++i) {
            try {
                controller.send(ax12::WriteData(controller.ax12_ids()[i], 0x20, moving_speed_L, moving_speed_H));

                Status status;
                controller.recv(0.5f, status);
                usleep(1000);
            } catch (Error& e) {
                std::cerr << "error : " << e.msg() << std::endl;
            }
        }*/
    }
    
    void on_hscale_puppet_position_value_changed(GtkObject *object, gpointer data)
    {
        //g_print("%s : %f\n", gtk_widget_get_name(GTK_WIDGET(object)), gtk_range_get_value(GTK_RANGE(object)));

        byte_t *pid = (byte_t*) g_object_get_data(G_OBJECT(object), "ax12_id");
        int goal_position = (int) gtk_range_get_value(GTK_RANGE(object));
        byte_t goal_position_L = goal_position & 0x00FF;
        byte_t goal_position_H = (goal_position & 0xFF00) >> 8;

        //g_print("%d : %d\n", *pid, value);
        //g_print("H : %#04x   -   L : %#04x\n", goal_position_H, goal_position_L);

        try {
            controller.send(ax12::WriteData(*pid, 0x1e, goal_position_L, goal_position_H));

            Status status;
            controller.recv(0.5f, status);
            usleep(1000);
        } catch (Error& e) {
            std::cerr << "error : " << e.msg() << std::endl;
        }
    }

    void on_togglebutton_puppet_toggled(GtkObject *object, gpointer data)
    {
        g_print("%s : %s\n", gtk_widget_get_name(GTK_WIDGET(object)), gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(object)) ? "Torque enabled" : "Torque disabled");
    }
}

int main(int argc, char ** argv)
{

    // Init dynamixel /////////////////////////////////////
    //Usb2Dynamixel controller("/dev/ttyUSB0"); 
    g_print("dynamixel init ok\n");

    // Scan actuators IDs
    controller.scan_ax12s();
    const std::vector<byte_t>& ax12_ids = controller.ax12_ids();
    if (!ax12_ids.size()) {
        g_print("no ax12 detected, exit\n");
        return 0;
    } else {
        g_print("%d ax12 are connected\n", ax12_ids.size());
    }

    // GTK ////////////////////////////////////////////////
    GtkBuilder  *builder;
    GtkWidget   *window;

    /* Initialisation de GTK+ */
    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/gui.xml", NULL);

    // Setup widgets
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    byte_t hscale_datas[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    char * hscale_names[] = { "hscale_tab1_id1",
                              "hscale_tab1_id2",
                              "hscale_tab1_id3",
                              "hscale_tab1_id4",
                              "hscale_tab1_id5",
                              "hscale_tab1_id6",
                              "hscale_tab1_id7",
                              "hscale_tab1_id8",
                              "hscale_tab1_id9",
                              "hscale_tab1_id10",
                              "hscale_tab1_id11",
                              "hscale_tab1_id12",
                              "hscale_tab1_id13",
                              "hscale_tab1_id14",
                              "hscale_tab1_id15",
                              "hscale_tab1_id16" };

    for(int i=0 ; i < 16; i++) {
        g_object_set_data(G_OBJECT(gtk_builder_get_object(builder, hscale_names[i])), "ax12_id", &hscale_datas[i]);
    }

    // Apply initial position
    for(int i=0 ; i < ax12_ids.size() ; i++) {
        byte_t id = hscale_datas[i];
        int goal_position = (int) gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder, hscale_names[i])));
        byte_t goal_position_L = goal_position & 0x00FF;
        byte_t goal_position_H = (goal_position & 0xFF00) >> 8;

        try {
            controller.send(ax12::WriteData(id, 0x1e, goal_position_L, goal_position_H));

            Status status;
            controller.recv(0.01f, status);
        } catch (Error& e) {
            std::cerr << "error : " << e.msg() << std::endl;
        }
    }

    // Launch GTK UI
    gtk_builder_connect_signals(builder, NULL); 

    g_object_unref(G_OBJECT(builder));

    gtk_widget_show(window);
    gtk_main();

    return EXIT_SUCCESS;
}

