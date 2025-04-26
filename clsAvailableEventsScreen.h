#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include "Database.h"
#include "Globals.h"

using namespace std;

class clsAvailableEventsScreen {
private:

    static void _PrintEventRow(vector<string> row) {
        cout << setw(8) << left << "" << "| " << setw(4) << left << row[0];  // ID
        cout << "| " << setw(20) << left << row[1];                           // Name
        cout << "| " << setw(30) << left << row[2].substr(0, 29);             // Description
        cout << "| " << setw(10) << left << row[3];                           // Date
        cout << "| " << setw(8) << left << row[4];                           // Time
        cout << "| " << setw(15) << left << row[5].substr(0, 14);             // Venue
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

    static void Show() {
        system("cls");
        cout << "========================================\n";
        cout << "         Available Events               \n";
        cout << "========================================\n\n";

        string query =
            "SELECT id, name, description, date, time, venue "
            "FROM Events "
            "WHERE date > CURDATE() OR (date = CURDATE() AND time > CURTIME())";

        connectToDatabase();
        string result = executeQuerySilent(query);
        closeDatabaseConnection();

        vector<vector<string>> table = _ParseResult(result);

        if (table.size() < 1) {
            cout << "\nNo available events at the moment.\n";
            cout << "\nPress any key to return...";
            system("pause>0");
            return;
        }

        // Header
        cout << setw(8) << left << "" << "\n\t_____________________________________________________________________________________________________\n";
        cout << setw(8) << left << "" << "| " << setw(4) << left << "ID";
        cout << "| " << setw(20) << left << "Name";
        cout << "| " << setw(30) << left << "Description";
        cout << "| " << setw(10) << left << "Date";
        cout << "| " << setw(8) << left << "Time";
        cout << "| " << setw(15) << left << "Venue";
        cout << "\n" << setw(8) << left << "" << "_____________________________________________________________________________________________________\n";

        for (size_t i = 1; i < table.size(); ++i) {
            _PrintEventRow(table[i]);
            cout << endl;
        }

        cout << setw(8) << left << "" << "\n\t_____________________________________________________________________________________________________\n";
        
    }
};
