#pragma once
#include <iostream>
#include <string>
#include "Database.h"
#include "Globals.h"
#include "clsInputValidate.h"

using namespace std;

class clsAddSpeakerScreen {
public:
    static void Show() {
        system("cls");
        cout << "=============================\n";
        cout << "        Add New Speaker      \n";
        cout << "=============================\n\n";

        int event_id;
        string full_name, topic, session_time;
        cin.ignore();

        do {
            cout << "Enter Event ID           : ";
            cin >> event_id;
            cin.ignore(); // clear leftover newline

            if (!DoesEventExist(event_id)) {
                cout << " Event ID does not exist. Please try again.\n";
                event_id = -1;
            }
        } while (event_id == -1);

        cout << "Enter Full Name          : ";
        getline(cin, full_name);

        cout << "Enter Topic              : ";
        getline(cin, topic);

        do {
            cout << "Enter Session Time (HH:MM:SS): ";
            getline(cin, session_time);
            if (!clsInputValidate::IsValidTimeFormat(session_time)) {
                cout << " Invalid format! Use HH:MM:SS.\n";
                session_time.clear();
            }
        } while (session_time.empty());

        string query = "INSERT INTO speakers (event_id, full_name, topic, session_time) VALUES ('" +
            to_string(event_id) + "', '" +
            escapeString(full_name) + "', '" +
            escapeString(topic) + "', '" +
            escapeString(session_time) + "')";

        connectToDatabase();
        executeInstruction(query);
        closeDatabaseConnection();

        cout << "\n Speaker added successfully.\n";
    }

private:
    static string escapeString(const string& str) {
        string escaped = str;
        size_t pos = 0;
        while ((pos = escaped.find('\'', pos)) != string::npos) {
            escaped.insert(pos, 1, '\'');
            pos += 2;
        }
        return escaped;
    }

    static bool DoesEventExist(int event_id) {
        connectToDatabase();

        string query = "SELECT id FROM events WHERE id = " + to_string(event_id) + " and organizer_id = " + to_string(CurrentUser.Id) ;
        string result = executeQuery(query);

        closeDatabaseConnection();

        return !result.empty();
    }
};
