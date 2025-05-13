#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include "Database.h"
#include "Globals.h"

using namespace std;

class clsDeleteSpeakerScreen {
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

    static void _PrintSpeakerDetails(const vector<string>& row) {
        cout << "\nSpeaker Details:\n";
        cout << "-------------------------------------\n";
        cout << "ID           : " << row[0] << endl;
        cout << "Event ID     : " << row[1] << endl;
        cout << "Full Name    : " << row[2] << endl;
        cout << "Topic        : " << row[3] << endl;
        cout << "Session Time : " << row[4] << endl;
        cout << "-------------------------------------\n";
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
        cout << "            Delete Speaker              \n";
        cout << "========================================\n\n";

        string speakerID;
        cout << "Enter Speaker ID to delete: ";
        cin.ignore();
        getline(cin, speakerID);

        // Step 1: Fetch the speaker and verify ownership via event's organizer
        string fetchQuery =
            "SELECT s.id, s.event_id, s.full_name, s.topic, s.session_time "
            "FROM Speakers s "
            "JOIN Events e ON s.event_id = e.id "
            "WHERE s.id = '" + escapeString(speakerID) + "' AND e.organizer_id = '" + to_string(CurrentUser.Id) + "'";

        connectToDatabase();
        string result = executeQuerySilent(fetchQuery);
        closeDatabaseConnection();

        if (result.empty() || result.find('\n') == string::npos) {
            cout << "\nSpeaker not found or you don't have permission to delete it.\n";
            return;
        }

        // Step 2: Show speaker preview
        vector<string> speakerDetails = _SplitLine(result.substr(result.find('\n') + 1));
        _PrintSpeakerDetails(speakerDetails);

        // Step 3: Confirm deletion
        string confirm;
        cout << "\nAre you sure you want to delete this speaker? (Y/N): ";
        getline(cin, confirm);

        if (confirm != "Y" && confirm != "y") {
            cout << "\nDeletion cancelled.\n";
            return;
        }

        // Step 4: Delete
        string deleteQuery = "DELETE FROM Speakers WHERE id = '" + escapeString(speakerID) + "'";
        connectToDatabase();
        executeInstruction(deleteQuery);
        closeDatabaseConnection();

        cout << "\nSpeaker deleted successfully.\n";
    }
};
