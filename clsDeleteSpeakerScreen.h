#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include "Database.h"
#include "Globals.h"
#include "clsScreen.h"
#include "clsInputValidate.h"
#include "clsErrorHandler.h"
#include "clsDate.h"

using namespace std;

class clsDeleteSpeakerScreen : protected clsScreen {
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

    static void _DrawScreenHeader() {
        system("cls");
        cout << "\n===========================================\n";
        cout << "\t   Delete Speaker Screen";
        cout << "\n===========================================\n";
    }

public:
    static void ShowDeleteSpeakerScreen() {
        _DrawScreenHeader();

        if (!connectToDatabase()) {
            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, "Failed to connect to database");
            return;
        }

        cout << "\n--- Speakers in Your Upcoming Events ---\n";
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
            cout << "You have no speakers in upcoming events to delete.\n";
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
            cout << "\nEnter Speaker ID to delete (or 'X' to cancel): ";
            getline(cin >> ws, speakerID);

            if (speakerID == "x" || speakerID == "X") {
                cout << "\nOperation cancelled.\n";
                closeDatabaseConnection();
                return;
            }

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


        cout << "\nAre you sure you want to delete this speaker? (Y/N): ";
        string confirm;
        getline(cin >> ws, confirm);

        if (confirm == "Y" || confirm == "y") {
            string deleteQuery = "DELETE FROM Speakers WHERE id = '" + speakerID + "'";
            executeInstruction(deleteQuery);
            cout << "\nSpeaker deleted successfully!\n";
        }
        else {
            cout << "\nDeletion cancelled.\n";
        }

        closeDatabaseConnection();

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
