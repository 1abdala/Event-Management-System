#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include "Database.h"
#include "Globals.h"
#include "clsInputValidate.h"

using namespace std;

class clsUpdateSpeakerScreen {
public:
    static void Show() {
        system("cls");
        cout << "========================================\n";
        cout << "           Update My Speaker            \n";
        cout << "========================================\n\n";

        string speakerID;
        cout << "Enter Speaker ID to update: ";
        cin.ignore();
        getline(cin, speakerID);

        // Verify speaker ownership via event's organizer
        string checkQuery =
            "SELECT s.* FROM Speakers s "
            "JOIN Events e ON s.event_id = e.id "
            "WHERE s.id = '" + escapeString(speakerID) + "' "
            "AND e.organizer_id = '" + to_string(CurrentUser.Id) + "'";

        connectToDatabase();
        string result = executeQuerySilent(checkQuery);
        closeDatabaseConnection();

        if (result.empty() || result.find('\n') == string::npos) {
            cout << "\nSpeaker not found or you don't have access to update it.\n";
            cout << "\nPress any key to return...";
            system("pause>0");
            return;
        }

        string fullName, topic, sessionTime;

        // Input with validation
        do {
            cout << "New Full Name       : ";
            getline(cin, fullName);
            if (fullName.empty()) cout << "Full name cannot be empty.\n";
        } while (fullName.empty());

        do {
            cout << "New Topic           : ";
            getline(cin, topic);
            if (topic.empty()) cout << "Topic cannot be empty.\n";
        } while (topic.empty());

        do {
            cout << "New Session Time (HH:MM:SS): ";
            getline(cin, sessionTime);
            if (!clsInputValidate::IsValidTimeFormat(sessionTime)) {
                cout << "Invalid format! Use HH:MM:SS.\n";
                sessionTime.clear();
            }
        } while (sessionTime.empty());

        // Update query
        string updateQuery =
            "UPDATE Speakers SET "
            "full_name = '" + escapeString(fullName) + "', "
            "topic = '" + escapeString(topic) + "', "
            "session_time = '" + escapeString(sessionTime) + "' "
            "WHERE id = '" + escapeString(speakerID) + "'";

        connectToDatabase();
        executeInstruction(updateQuery);
        closeDatabaseConnection();

        cout << "\nSpeaker updated successfully.\n";
        cout << "Press any key to return...";
        system("pause>0");
    }

private:
    static string escapeString(const string& input) {
        string result = input;
        size_t pos = 0;
        while ((pos = result.find('\'')) != string::npos) {
            result.insert(pos, 1, '\'');
            pos += 2;
        }
        return result;
    }
};
