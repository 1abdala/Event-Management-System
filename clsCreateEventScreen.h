#pragma once
#include <iostream>
#include <string>
#include "Database.h"
#include "Globals.h"
#include "clsInputValidate.h"
#include "clsDate.h"

using namespace std;

class clsCreateEventScreen {
public:
    static void Show() {
        system("cls");
        cout << "==============================\n";
        cout << "      Create New Event        \n";
        cout << "==============================\n\n";

        cin.ignore();

        string name, description, date, time, venue;
        int availableTickets;
        double ticketPrice;

        cout << "Enter Event Name       : ";
        getline(cin, name);

        cout << "Enter Description      : ";
        getline(cin, description);

        do {
            cout << "Enter Date (YYYY-MM-DD): ";
            getline(cin, date);
            if (!clsInputValidate::IsValidDateFormat(date)) {
                cout << "Invalid format! Use YYYY-MM-DD.\n";
                date.clear();
            } else {
                // Convert entered date string (YYYY-MM-DD) to clsDate object
                short year = stoi(date.substr(0, 4));
                short month = stoi(date.substr(5, 2));
                short day = stoi(date.substr(8, 2));
                clsDate EventDate(day, month, year);
                clsDate CurrentSystemDate = clsDate::GetSystemDate();

                if (clsDate::IsDate1BeforeDate2(EventDate, CurrentSystemDate)) {
                    cout << "Event date cannot be in the past. Please enter a future or current date.\n";
                    date.clear();
                }
            }
        } while (date.empty());

        do {
            cout << "Enter Time (HH:MM:SS)  : ";
            getline(cin, time);
            if (!clsInputValidate::IsValidTimeFormat(time)) {
                cout << "Invalid format! Use HH:MM:SS.\n";
                time.clear();
            } else {
                // Additional validation for time components
                try {
                    short hour = stoi(time.substr(0, 2));
                    short minute = stoi(time.substr(3, 2));
                    short second = stoi(time.substr(6, 2));

                    if (hour < 0 || hour > 23) {
                        cout << "Invalid hour. Hour must be between 00 and 23.\n";
                        time.clear();
                    } else if (minute < 0 || minute > 59) {
                        cout << "Invalid minute. Minute must be between 00 and 59.\n";
                        time.clear();
                    } else if (second < 0 || second > 59) {
                        cout << "Invalid second. Second must be between 00 and 59.\n";
                        time.clear();
                    }
                } catch (const std::out_of_range& oor) {
                    // This can happen if stoi tries to parse a very large number not fitting in short
                    cout << "Time values are out of range. Please enter valid numbers.\n";
                    time.clear();
                } catch (const std::invalid_argument& ia) {
                    // This can happen if substrings are not numbers (though IsValidTimeFormat should catch some)
                    cout << "Time values are not valid numbers. Please check your input.\n";
                    time.clear();
                }
            }
        } while (time.empty());

        cout << "Enter Venue            : ";
        getline(cin, venue);

        cout << "Enter Available Tickets: ";
        availableTickets = clsInputValidate::ReadIntNumber();
        cin.ignore();
        // Ticket Price input with validation
        string priceInput;
        do {
            cout << "Enter Ticket Price     : ";
            getline(cin, priceInput);
            try {
                ticketPrice = stod(priceInput);
                if (ticketPrice < 0) {
                    cout << "Ticket price cannot be negative.\n";
                    priceInput.clear();
                }
            }
            catch (...) {
                cout << "Invalid price. Please enter a valid number.\n";
                priceInput.clear();
            }
        } while (priceInput.empty());

        string query = "INSERT INTO events (organizer_id, name, description, date, time, venue, available_tickets, ticket_price) VALUES ('" +
            to_string(CurrentUser.Id) + "', '" +
            escapeString(name) + "', '" +
            escapeString(description) + "', '" +
            escapeString(date) + "', '" +
            escapeString(time) + "', '" +
            escapeString(venue) + "', '" +
            to_string(availableTickets) + "', '" +
            to_string(ticketPrice) + "')";

        connectToDatabase();
        executeInstruction(query);
        closeDatabaseConnection();

        cout << "\nEvent created successfully.\n";
        cout << "Press any key to return...";
        system("pause>0");
    }

private:
    // Escape single quotes for SQL strings
    static string escapeString(const string& str) {
        string escaped = str;
        size_t pos = 0;
        while ((pos = escaped.find('\'', pos)) != string::npos) {
            escaped.insert(pos, 1, '\'');
            pos += 2;
        }
        return escaped;
    }
};
