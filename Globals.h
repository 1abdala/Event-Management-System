#pragma once
#include <string>
#include "clsUser.h"

// Global Constants
const std::string PROGRAM_NAME = "Event Management System";
const std::string PROGRAM_VERSION = "1.0";
const short SCREEN_WIDTH = 120;

// Global User object
extern User CurrentUser;

// Global enums
enum enUserType { Organizer = 1, Attendee = 2 };

// Global Functions
namespace Global {
    inline void ShowMessage(const std::string& message, bool isError = false) {
        if (isError)
            std::cout << "\n\tError: " << message << std::endl;
        else
            std::cout << "\n\t" << message << std::endl;
    }
}
