#include "input.h"

#ifdef _WIN32

INPUT Input::to_input_data(const size_t key, const bool extended) {
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

#elif defined __APPLE__

CGEventSourceRef Input::event_source = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);

#endif

void Input::simulate_key_release(const key_type& key) {
#ifdef _WIN32
    INPUT input_data = to_input_data(key.first, key.second);
    input_data.ki.dwFlags |= KEYEVENTF_KEYUP;

    SendInput(1, &input_data, sizeof(INPUT));
    
#elif defined __APPLE__
    // Create a new keyboard key release event
    CGEventRef event = CGEventCreateKeyboardEvent(Input::event_source, static_cast<CGKeyCode>(key.first), false);

    // Post keyboard event
    CGEventPost(kCGHIDEventTap, event);
    
    // Release key release event
    CFRelease(event);
    
#endif
}

void Input::simulate_key_press(const key_type& key) {
#ifdef _WIN32
    INPUT input_data = to_input_data(key.first, key.second);

    SendInput(1, &input_data, sizeof(INPUT));
    
#elif defined __APPLE__
    // Create a new keyboard key press event
    CGEventRef event = CGEventCreateKeyboardEvent(Input::event_source, static_cast<CGKeyCode>(key.first), true);

    // Post keyboard event
    CGEventPost(kCGHIDEventTap, event);
    
    // Release key press event
    CFRelease(event);
    
#endif
}
