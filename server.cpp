#include "internet.h"
#include "hid.h"

#include <iostream>
#include <array>
#include <map>
#include <fstream>
#include <limits>
#include <algorithm>

#include <windows.h>

using namespace std;

//? size_t - virtual key code
//? bool - is_extended flag
typedef std::pair<size_t, bool> key_t;

const std::map<std::string, uint32_t> text_to_console_map = {
    {"KEY_A",      KEY_A},
    {"KEY_B",      KEY_B},
    {"KEY_X",      KEY_X},
    {"KEY_Y",      KEY_Y},
    {"KEY_L",      KEY_L},
    {"KEY_R",      KEY_R},
    {"KEY_LEFT",   KEY_LEFT},
    {"KEY_RIGHT",  KEY_RIGHT},
    {"KEY_UP",     KEY_UP},
    {"KEY_DOWN",   KEY_DOWN},
    {"KEY_START",  KEY_START},
    {"KEY_SELECT", KEY_SELECT}
};

INPUT to_input_data(const size_t key, const bool extended) {
	INPUT input_data = {0};

	if(key == VK_LBUTTON || key == VK_RBUTTON) {
		input_data.type = INPUT_MOUSE;
		input_data.mi.dwFlags = key == VK_LBUTTON ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
	}
	else {
		input_data.type = INPUT_KEYBOARD;
		input_data.ki.dwFlags = KEYEVENTF_SCANCODE | (extended ? KEYEVENTF_EXTENDEDKEY : 0);
		
		//* Turns the virtual key code into a scan code
		input_data.ki.wScan = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
	}

	return input_data;
}
void simulate_key_release(const key_t key) {
	INPUT input_data = to_input_data(key.first, key.second);
	input_data.ki.dwFlags |= KEYEVENTF_KEYUP;

	SendInput(1, &input_data, sizeof(INPUT));
}
void simulate_key_press(const key_t key) {
	INPUT input_data = to_input_data(key.first, key.second);

	SendInput(1, &input_data, sizeof(INPUT));
}

std::map<uint32_t, key_t> read_settings(std::string& port) {
    ifstream input_file("settings.ini");

	// Ignore comments
	while(input_file.peek() == '/') {
		input_file.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	//* Read the port by ignoring the first word 'PORT'
	input_file >> port >> port;

    std::map<uint32_t, key_t> result;
    while(input_file.peek() != EOF) {
		std::string key;
		size_t virtual_code;
		bool extended = false;

        input_file >> key >> hex >> virtual_code;

		if(key.at(0) == '!') {
			key.erase(0, 1);
			extended = true;
		}

        uint32_t button = text_to_console_map.find(key)->second;

        result.insert({button, {virtual_code, extended}});
    }

    return result;
}

int main() {
	//* Read the settings
	std::string port;
    const std::map<uint32_t, key_t> console_to_virtual_map = read_settings(port);
    
	//* Start the server
	socket_init();
    UDP_Server server(port);

	std::cout << "Listenting for data on port: " << port << '\n';

	//* Array for holding the key states
	//? [0] - pressed, [1] - held, [2] - released
    std::array<uint32_t, 3> states;

	//* Recieve data from the client and simulate key actions
    while(server.recv(reinterpret_cast<char*>(states.data()),  sizeof(states)) != 0) {

		//? Loop over the mapped keys, check their states and respond accordingly
		std::for_each(std::begin(console_to_virtual_map), std::end(console_to_virtual_map), [&states](const auto& map_entry){
			//* Simulate key press if the 'pressed' state is true
            if(states[0] & map_entry.first){
				simulate_key_press(map_entry.second);
			}

			//* Simulate key release if the 'released' state is true
			if(states[2] & map_entry.first) {
				simulate_key_release(map_entry.second);
			}
		});
    }
}