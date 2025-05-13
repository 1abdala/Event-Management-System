#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include "Database.h"
#include "Globals.h"

using namespace std;

class clsDeleteEventScreen {
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

    static void _PrintEventRecordLine(const vector<string>& row) {
        cout << setw(8) << left << "" << "| " << setw(4) << left << row[0]; // ID
        cout << "| " << setw(18) << left << row[1]; // Name
        cout << "| " << setw(30) << left << row[2].substr(0, 29); // Description
        cout << "| " << setw(10) << left << row[3]; // Date
        cout << "| " << setw(8) << left << row[4]; // Time
        cout << "| " << setw(15) << left << row[5].substr(0, 14); // Venue
    }

    static string escapeString(const string& input) {
        string result = input;
        size_t pos = 0;
        while ((pos = result.find('\'')) != string::npos) {
            result.insert(pos, 1, '\'');
            pos += 2;
        }
        return result;
    }

public:
    static void Show() {
        system("cls");
        cout << "========================================\n";
        cout << "           Delete My Event              \n";
        cout << "========================================\n\n";

        // Step 0: Display existing events
        string query = "SELECT id, name, description, date, time, venue FROM Events WHERE organizer_id = " + to_string(CurrentUser.Id);
        connectToDatabase();
        string result = executeQuerySilent(query);
        closeDatabaseConnection();
        vector<vector<string>> table = _ParseResult(result);

        if (table.size() <= 1) {
            cout << "\nNo events found for this user.\n";
            return;
        }

        // Print Header
        cout << setw(8) << left << "" << "\n\t________________________________________________________________________________________\n";
        cout << setw(8) << left << "" << "| " << setw(4) << left << "ID";
        cout << "| " << setw(18) << left << "Name";
        cout << "| " << setw(30) << left << "Description";
        cout << "| " << setw(10) << left << "Date";
        cout << "| " << setw(8) << left << "Time";
        cout << "| " << setw(15) << left << "Venue";
        cout << "\n" << setw(5) << left << "" << "\t________________________________________________________________________________________\n";

        for (size_t i = 1; i < table.size(); ++i) {
            _PrintEventRecordLine(table[i]);
            cout << endl;
        }

        cout << setw(5) << left << "" << "\t________________________________________________________________________________________\n";

        // Step 1: Ask for ID to delete
        string eventID;
        cout << "\nEnter Event ID to delete: ";
        cin.ignore();
        getline(cin, eventID);

        // Step 2: Verify ownership
        string checkQuery = "SELECT * FROM Events WHERE id = '" + escapeString(eventID) + "' AND organizer_id = '" + to_string(CurrentUser.Id) + "'";
        connectToDatabase();
        string checkResult = executeQuerySilent(checkQuery);
        closeDatabaseConnection();

        if (checkResult.empty() || checkResult.find('\n') == string::npos) {
            cout << "\nEvent not found or you don't have permission to delete it.\n";
            return;
        }

        // Step 3: Confirm deletion
        string confirm;
        cout << "\nAre you sure you want to delete this event? (Y/N): ";
        getline(cin, confirm);

        if (confirm != "Y" && confirm != "y") {
            cout << "\nDeletion cancelled.\n";
            return;
        }

        // Step 4: Delete
        string deleteQuery = "DELETE FROM Events WHERE id = '" + escapeString(eventID) + "' AND organizer_id = '" + to_string(CurrentUser.Id) + "'";
        connectToDatabase();
        executeInstruction(deleteQuery);
        closeDatabaseConnection();

        cout << "\nEvent deleted successfully.\n";
    }
};
