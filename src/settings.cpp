#include "settings.h"

#include <fstream>
#include <limits>

Settings::Settings(std::string_view filename, const std::map<std::string, uint32_t>& text_to_console_map) {
    // Open the file
    std::ifstream input_file(filename);

    // Ignore comments
    while(input_file.peek() == '/') {
        input_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    //* Read the port by ignoring the first word 'PORT'
    input_file >> port;

    //* Read the keys
    while(input_file.peek() != EOF) {
        std::string key;
        size_t virtual_code;
        bool extended = false;

        input_file >> key >> std::hex >> virtual_code;

        if(key.at(0) == '!') {
            key.erase(0, 1);
            extended = true;
        }

        uint32_t button = text_to_console_map.find(key)->second;

        console_to_virtual_map.insert({button, {virtual_code, extended}});
    }
}
