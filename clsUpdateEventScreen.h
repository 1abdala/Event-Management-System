#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include "Database.h"
#include "Globals.h"
#include "clsInputValidate.h"

using namespace std;

class clsUpdateEventScreen {
public:
    static void Show() {
        system("cls");
        cout << "========================================\n";
        cout << "           Update My Event              \n";
        cout << "========================================\n\n";

        string eventID;
        cout << "Enter Event ID to update: ";
        cin.ignore();
        getline(cin, eventID);

        // Verify ownership
        string checkQuery = "SELECT * FROM Events WHERE id = " + eventID +
            " AND organizer_id = " + to_string(CurrentUser.Id);

        connectToDatabase();
        string result = executeQuerySilent(checkQuery);
        closeDatabaseConnection();

        if (result.empty() || result.find('\n') == string::npos) {
            cout << "\nEvent not found or you don't have access to it.\n";
            cout << "\nPress any key to return...";
            system("pause>0");
            return;
        }

        string name, description, date, time, venue;

        // Input with validation
        do {
            cout << "New Event Name       : ";
            getline(cin, name);
            if (name.empty()) cout << "Event name cannot be empty.\n";
        } while (name.empty());

        do {
            cout << "New Description      : ";
            getline(cin, description);
            if (description.empty()) cout << "Description cannot be empty.\n";
        } while (description.empty());

        do {
            cout << "New Date (YYYY-MM-DD): ";
            getline(cin, date);
            if (!clsInputValidate::IsValidDateFormat(date)) {
                cout << "Invalid format! Use YYYY-MM-DD.\n";
                date.clear();
            }
        } while (date.empty());

        do {
            cout << "New Time (HH:MM:SS)  : ";
            getline(cin, time);
            if (!clsInputValidate::IsValidTimeFormat(time)) {
                cout << "Invalid format! Use HH:MM:SS.\n";
                time.clear();
            }
        } while (time.empty());

        do {
            cout << "New Venue            : ";
            getline(cin, venue);
            if (venue.empty()) cout << "Venue cannot be empty.\n";
        } while (venue.empty());

        string query = "UPDATE Events SET "
            "name = '" + escapeString(name) + "', "
            "description = '" + escapeString(description) + "', "
            "date = '" + escapeString(date) + "', "
            "time = '" + escapeString(time) + "', "
            "venue = '" + escapeString(venue) + "' "
            "WHERE id = '" + escapeString(eventID) + "' AND organizer_id = '" + to_string(CurrentUser.Id) + "'";

        connectToDatabase();
        executeInstruction(query);
        closeDatabaseConnection();

        cout << "\nEvent updated successfully.\n";
        cout << "Press any key to return...";
        system("pause>0");
    }

private:
    static string escapeString(const string& input) {
        string result = input;
        size_t pos = 0;
        while ((pos = result.find('\'', pos)) != string::npos) {
            result.insert(pos, 1, '\'');
            pos += 2;
        }
        return result;
    }
};
