#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <iostream>
#include <iomanip>

struct Event {
    int id = 0;
    int organizer_id = 0;
    std::string name = "";
    std::string description = "";
    std::string date = "";
    std::string time = "";
    std::string venue = "";
    std::string created_at = "";
    std::string updated_at = "";

    void display() const {
        std::cout << std::left
            << std::setw(5) << id
            << std::setw(12) << organizer_id
            << std::setw(20) << name
            << std::setw(25) << description
            << std::setw(12) << date
            << std::setw(12) << time
            << std::setw(10) << venue
            << std::setw(22) << created_at
            << std::setw(22) << updated_at
            << "\n";
    }
};

#endif
