#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include "Database.h"

using namespace std;

class clsViewAddedSpeakersScreen {
private:

    static void _PrintSpeakerRecordLine(vector<string> row) {
        cout << setw(8) << left << "" << "| " << setw(4) << left << row[0];  // ID
        cout << "| " << setw(18) << left << row[1];  // Full Name
        cout << "| " << setw(25) << left << row[2].substr(0, 24);  // Topic
        cout << "| " << setw(12) << left << row[3];  // Session Time
        cout << "| " << setw(20) << left << row[4].substr(0, 19);  // Event Name
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

    static void ShowAddedSpeakers() {
        system("cls");

        cout << "========================================\n";
        cout << "             All Speakers               \n";
        cout << "========================================\n\n";

        string query =
            "SELECT s.id, s.full_name, s.topic, s.session_time, e.name "
            "FROM speakers s JOIN events e ON s.event_id = e.id";

        connectToDatabase();
        string result = executeQuerySilent(query);
        closeDatabaseConnection();

        vector<vector<string>> table = _ParseResult(result);

        if (table.size() <= 1) {
            cout << "\n\n\t\t\t  No speakers found.\n";
            return;
        }

        // Table Header
        cout << setw(8) << left << "" << "\n\t________________________________________________________________________________________\n";
        cout << setw(8) << left << "" << "| " << setw(4) << left << "ID";
        cout << "| " << setw(18) << left << "Full Name";
        cout << "| " << setw(25) << left << "Topic";
        cout << "| " << setw(12) << left << "Session";
        cout << "| " << setw(20) << left << "Event";
        cout << "\n" << setw(5) << left << "" << "\t________________________________________________________________________________________\n";

        // Table Body
        for (size_t i = 1; i < table.size(); ++i) {
            _PrintSpeakerRecordLine(table[i]);
            cout << endl;
        }

        // Table Footer
        cout << setw(5) << left << "" << "\t________________________________________________________________________________________\n";
    }
};
