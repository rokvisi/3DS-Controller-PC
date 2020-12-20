#pragma once

#ifdef _WIN32
    #inlcude <windows.h>
#elif defined __APPLE__
    #include <ApplicationServices/ApplicationServices.h>
#endif

#include "types.h"

class Input {
public:
#ifdef _WIN32
    static INPUT to_input_data(const size_t key, const bool extended);
#endif
    
    static void simulate_key_release(const key_type& key);
    static void simulate_key_press(const key_type& key);
    
private:
#ifdef __APPLE__
    static CGEventSourceRef event_source;
#endif
};
