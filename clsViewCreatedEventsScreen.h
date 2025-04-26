#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include "Database.h"
#include "Globals.h"

using namespace std;

class clsViewCreatedEventsScreen {
private:

    static void _PrintEventRecordLine(vector<string> row) {
        cout << setw(8) << left << "" << "| " << setw(4) << left << row[0];  // ID
        cout << "| " << setw(18) << left << row[1];  // Name
        cout << "| " << setw(30) << left << row[2].substr(0, 29);  // Description
        cout << "| " << setw(10) << left << row[3];  // Date
        cout << "| " << setw(8) << left << row[4];  // Time
        cout << "| " << setw(15) << left << row[5].substr(0, 14);  // Venue
    }

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

public:

    static void ShowCreatedEvents() {
        system("cls");

        cout << "========================================\n";
        cout << "         My Created Events              \n";
        cout << "========================================\n\n";

        string query = "SELECT id, name, description, date, time, venue FROM Events WHERE organizer_id = " + to_string(CurrentUser.Id);
        connectToDatabase();
        string result = executeQuerySilent(query);
        closeDatabaseConnection();
        vector<vector<string>> table = _ParseResult(result);

        if (table.size() <= 1) {
            cout << "\n\n\t\t\t  No events found for this user.\n";
            return;
        }

        // Table Header
        cout << setw(8) << left << "" << "\n\t________________________________________________________________________________________\n";
        cout << setw(8) << left << "" << "| " << setw(4) << left << "ID";
        cout << "| " << setw(18) << left << "Name";
        cout << "| " << setw(30) << left << "Description";
        cout << "| " << setw(10) << left << "Date";
        cout << "| " << setw(8) << left << "Time";
        cout << "| " << setw(15) << left << "Venue";
        cout << "\n" << setw(5) << left << "" << "\t________________________________________________________________________________________\n";

        // Table Body
        for (size_t i = 1; i < table.size(); ++i) {
            _PrintEventRecordLine(table[i]);
            cout << endl;
        }

        // Table Footer
        cout << setw(5) << left << "" << "\t________________________________________________________________________________________\n";
    }
};
