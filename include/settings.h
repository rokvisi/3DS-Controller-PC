#pragma once

#include <string>
#include <map>

#include "types.h"

class Settings {
public:
    Settings(std::string_view filename, const std::map<std::string, uint32_t>& text_to_console_map);
    
    std::map<uint32_t, key_type> console_to_virtual_map;
    std::string port;
};
