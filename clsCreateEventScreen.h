#pragma once
#include <iostream>
#include <string>
#include "Database.h"
#include "Globals.h"
#include "clsInputValidate.h"

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
            }
        } while (date.empty());

        do {
            cout << "Enter Time (HH:MM:SS)  : ";
            getline(cin, time);
            if (!clsInputValidate::IsValidTimeFormat(time)) {
                cout << "Invalid format! Use HH:MM:SS.\n";
                time.clear();
            }
        } while (time.empty());

        cout << "Enter Venue            : ";
        getline(cin, venue);

       
            cout << "Enter Available Tickets: ";
            availableTickets=clsInputValidate::ReadIntNumber();
           
        

        string query = "INSERT INTO events (organizer_id, name, description, date, time, venue, available_tickets) VALUES ('" +
            to_string(CurrentUser.Id) + "', '" +
            escapeString(name) + "', '" +
            escapeString(description) + "', '" +
            escapeString(date) + "', '" +
            escapeString(time) + "', '" +
            escapeString(venue) + "', '" +
            to_string(availableTickets) + "')";

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
