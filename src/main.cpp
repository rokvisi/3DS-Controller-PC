#include <iostream>
#include <array>

#include "internet.h"
#include "input.h"
#include "settings.h"
#include "constants.h"

int main() {
    //* Read the settings
    const Settings settings("settings.ini", text_to_console_map);
    
    //* Start the server
    socket_init();
    UDP_Server server(settings.port);
    std::cout << "Listenting for data on port: " << settings.port << '\n';

    //* Array for holding the key states
    //* Each array element is a uint32 that represents all of the keys with respective bits.
    //? [0] - pressed, [1] - held, [2] - released
    std::array<uint32_t, 3> states;
    enum { PRESSED = 0, HELD = 1, RELEASED = 2 };

    //* Recieve data from the client and simulate key actions
    int rec;
    while((rec = server.recv(reinterpret_cast<char*>(states.data()),  sizeof(states))) != 0) {
        std::cout << rec << "\n";

        //? Loop over the mapped keys, check their states and respond accordingly
        for(const auto& entry : settings.console_to_virtual_map) {
            //* Simulate key press if the 'pressed' state is true
            if(states[PRESSED] & entry.first) Input::simulate_key_press(entry.second);

            //* Simulate key release if the 'released' state is true
            if(states[RELEASED] & entry.first) Input::simulate_key_release(entry.second);
        }
    }
}