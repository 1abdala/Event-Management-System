#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include "Database.h"
#include "Globals.h"
#include "clsInputValidate.h"
#include "clsDate.h"
#include "clsErrorHandler.h"

using namespace std;

class clsAddSpeakerScreen {
private:
    static vector<string> _SplitLine(const string& line, char delimiter = '\t') {
        vector<string> tokens;
        string token;
        stringstream ss(line);
        while (getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    static vector<vector<string>> _ParseResult(const string& data) {
        vector<vector<string>> table;
        stringstream ss(data);
        string line;
        while (getline(ss, line)) {
            if (!line.empty()) {
                table.push_back(_SplitLine(line));
            }
        }
        return table;
    }

    static void _PrintEventRow(const vector<string>& row) {
        cout << "| " << setw(4) << left << row[0]
            << "| " << setw(30) << left << row[1]
            << "| " << setw(15) << left << row[2] << " |" << endl;
    }

public:
    static void Show() {
        system("cls");
        cout << "=============================\n";
        cout << "        Add New Speaker      \n";
        cout << "=============================\n\n";

        if (!connectToDatabase()) {
            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, "Failed to connect to database");
            return;
        }

        cout << "--- Your Upcoming Events (Without a Speaker) ---\n";
        string currentDateStr = clsDate::DateToYYYYMMDDString(clsDate::GetSystemDate());
        string query = "SELECT id, name, date FROM events WHERE organizer_id = " + to_string(CurrentUser.Id) +
            " AND date >= '" + currentDateStr + "'" +
            " AND id NOT IN (SELECT event_id FROM speakers WHERE event_id IS NOT NULL) " +
            "ORDER BY date ASC";
        string result = executeQuerySilent(query);

        size_t first_newline = result.find('\n');
        if (first_newline != string::npos) {
            result = result.substr(first_newline + 1);
        }

        vector<vector<string>> table = _ParseResult(result);
        vector<string> validEventIds;

        if (table.empty()) {
            cout << "You have no upcoming events that need a speaker assigned.\n";
            closeDatabaseConnection();
            cout << "\nPress any key to return...";
            system("pause>0");
            return;
        }

        cout << "-----------------------------------------------------\n";
        cout << "| " << setw(4) << left << "ID"
            << "| " << setw(30) << left << "Event Name"
            << "| " << setw(15) << left << "Event Date" << " |" << endl;
        cout << "-----------------------------------------------------\n";

        for (const auto& row : table) {
            if (row.size() >= 3) {
                _PrintEventRow(row);
                validEventIds.push_back(row[0]);
            }
        }
        cout << "-----------------------------------------------------\n\n";

        int event_id;
        string full_name, topic, session_time;

        do {
            cout << "Enter Event ID to add speaker to: ";
            event_id = clsInputValidate::ReadIntNumber();

            bool isValidId = false;
            for (const auto& id : validEventIds) {
                if (id == to_string(event_id)) {
                    isValidId = true;
                    break;
                }
            }

            if (!isValidId) {
                cout << " Invalid Event ID. Please choose from the list above.\n";
                event_id = -1;
            }

        } while (event_id == -1);

        cin.ignore();
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

        string insertQuery = "INSERT INTO speakers (event_id, full_name, topic, session_time) VALUES ('" +
            to_string(event_id) + "', '" +
            escapeString(full_name) + "', '" +
            escapeString(topic) + "', '" +
            escapeString(session_time) + "')";

        executeInstruction(insertQuery);
        closeDatabaseConnection();

        cout << "\n Speaker added successfully.\n";
        cout << "\nPress any key to return...";
        system("pause>0");
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
};
