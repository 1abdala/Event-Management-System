#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include "Database.h"
#include "Globals.h"
#include "clsInputValidate.h"
#include "clsDate.h"
#include "clsErrorHandler.h"

using namespace std;

class clsUpdateSpeakerScreen {
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

    static void _PrintSpeakerRow(const vector<string>& row) {
        // Expected row: speaker_id, speaker_name, topic, event_name
        cout << "| " << setw(4) << left << row[0]
            << "| " << setw(25) << left << row[1]
            << "| " << setw(30) << left << row[2]
            << "| " << setw(25) << left << row[3] << " |" << endl;
    }

public:
    static void Show() {
        system("cls");
        cout << "========================================\n";
        cout << "           Update My Speaker            \n";
        cout << "========================================\n\n";

        if (!connectToDatabase()) {
            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, "Failed to connect to database");
            return;
        }

        cout << "--- Speakers in Your Upcoming Events ---\n";
        string currentDateStr = clsDate::DateToYYYYMMDDString(clsDate::GetSystemDate());
        string query = "SELECT s.id, s.full_name, s.topic, e.name FROM speakers s "
            "JOIN events e ON s.event_id = e.id "
            "WHERE e.organizer_id = " + to_string(CurrentUser.Id) + " AND e.date >= '" + currentDateStr + "' "
            "ORDER BY e.date ASC, s.full_name ASC";
        string result = executeQuerySilent(query);

        size_t first_newline = result.find('\n');
        if (first_newline != string::npos) {
            result = result.substr(first_newline + 1);
        }

        vector<vector<string>> table = _ParseResult(result);
        vector<string> validSpeakerIds;

        if (table.empty()) {
            cout << "You have no speakers in upcoming events to update.\n";
            closeDatabaseConnection();
            cout << "\nPress any key to return...";
            system("pause>0");
            return;
        }

        cout << "-----------------------------------------------------------------------------------------\n";
        cout << "| " << setw(4) << left << "ID"
            << "| " << setw(25) << left << "Speaker Name"
            << "| " << setw(30) << left << "Topic"
            << "| " << setw(25) << left << "Event" << " |" << endl;
        cout << "-----------------------------------------------------------------------------------------\n";

        for (const auto& row : table) {
            if (row.size() >= 4) {
                _PrintSpeakerRow(row);
                validSpeakerIds.push_back(row[0]);
            }
        }
        cout << "-----------------------------------------------------------------------------------------\n\n";

        string speakerID;
        do {
            cout << "Enter Speaker ID to update: ";
            getline(cin >> ws, speakerID);

            bool isValidId = false;
            for (const auto& id : validSpeakerIds) {
                if (id == speakerID) {
                    isValidId = true;
                    break;
                }
            }

            if (!isValidId) {
                cout << " Invalid Speaker ID. Please choose from the list above.\n";
                speakerID.clear();
            }

        } while (speakerID.empty());

        string fetchQuery = "SELECT full_name, topic, session_time FROM speakers WHERE id = " + speakerID;
        string speakerDataResult = executeQuerySilent(fetchQuery);
        first_newline = speakerDataResult.find('\n');
        if (first_newline != string::npos) {
            speakerDataResult = speakerDataResult.substr(first_newline + 1);
        }
        vector<string> originalData = _SplitLine(speakerDataResult);

        string fullName = originalData[0], topic = originalData[1], sessionTime = originalData[2];

        cout << "\n--- Updating Speaker #" << speakerID << " (Press Enter to keep original value) ---\n";

        cout << "Full Name (" << fullName << "): ";
        string newFullName; getline(cin, newFullName); if (newFullName.empty()) newFullName = fullName;

        cout << "Topic (" << topic << "): ";
        string newTopic; getline(cin, newTopic); if (newTopic.empty()) newTopic = topic;
        
        string newSessionTime;
        do {
            cout << "Session Time (HH:MM:SS) (" << sessionTime << "): ";
            getline(cin, newSessionTime); if (newSessionTime.empty()) newSessionTime = sessionTime;
            if (!clsInputValidate::IsValidTimeFormat(newSessionTime)) {
                cout << "Invalid format! Use HH:MM:SS.\n";
                newSessionTime = "";
            }
        } while (newSessionTime.empty());


        // Update query
        string updateQuery =
            "UPDATE Speakers SET "
            "full_name = '" + escapeString(newFullName) + "', "
            "topic = '" + escapeString(newTopic) + "', "
            "session_time = '" + escapeString(newSessionTime) + "' "
            "WHERE id = '" + escapeString(speakerID) + "'";

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
        while ((pos = result.find('\'', pos)) != string::npos) {
            result.insert(pos, 1, '\'');
            pos += 2;
        }
        return result;
    }
};
