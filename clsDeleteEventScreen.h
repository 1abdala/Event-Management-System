#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include "Database.h"
#include "Globals.h"
#include "clsErrorHandler.h"
#include "clsScreen.h"
#include "clsDate.h"

using namespace std;

class clsDeleteEventScreen : protected clsScreen {
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

    static bool _ValidateEventID(const string& eventID) {
        if (eventID.empty()) {
            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Validation, 
                "Event ID cannot be empty.");
            return false;
        }

        // Check if ID contains only digits
        for (char c : eventID) {
            if (!isdigit(c)) {
                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Validation, 
                    "Event ID must contain only numbers.");
                return false;
            }
        }
        return true;
    }

public:
    static void Show() {
        try {
            system("cls");
            cout << "========================================\n";
            cout << "           Delete My Event              \n";
            cout << "========================================\n\n";

            if (!connectToDatabase()) {
                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, 
                    "Failed to connect to database");
                return;
            }

            // Step 0: Display existing events
            string query = "SELECT id, name, description, date, time, venue FROM Events WHERE organizer_id = " + to_string(CurrentUser.Id);
            string result = executeQuerySilent(query);
            vector<vector<string>> table = _ParseResult(result);

            if (table.size() <= 1) {
                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Validation, 
                    "No events found for this user.");
                closeDatabaseConnection();
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

            // Store valid event IDs
            vector<string> validEventIds;
            for (size_t i = 1; i < table.size(); ++i) {
                _PrintEventRecordLine(table[i]);
                cout << endl;
                validEventIds.push_back(table[i][0]); // Store the ID from first column
            }

            cout << setw(5) << left << "" << "\t________________________________________________________________________________________\n";

            do {
                // Step 1: Ask for ID to delete
                string eventID;
                cout << "\nEnter Event ID to delete (or 'X' to cancel): ";
                getline(cin >> ws, eventID);

                if (eventID == "X" || eventID == "x") {
                    cout << "\nOperation cancelled.\n";
                    closeDatabaseConnection();
                    return;
                }

                // Validate event ID format
                if (!_ValidateEventID(eventID)) {
                    continue;
                }

                // Check if the event ID exists in the user's events
                bool isValidId = false;
                size_t eventIndexInTable = 0;
                for (size_t i = 0; i < validEventIds.size(); i++)
                {
                    if (validEventIds[i] == eventID)
                    {
                        isValidId = true;
                        eventIndexInTable = i + 1; // +1 because table has a header row
                        break;
                    }
                }

                if (!isValidId) {
                    clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Validation, 
                        "Event ID " + eventID + " does not exist in your events list.");
                    continue;
                }

                string eventDateStr = table[eventIndexInTable][3]; // Date is 4th column
                short year = stoi(eventDateStr.substr(0, 4));
                short month = stoi(eventDateStr.substr(5, 2));
                short day = stoi(eventDateStr.substr(8, 2));
                clsDate eventDate(day, month, year);

                if (clsDate::IsDate1BeforeDate2(eventDate, clsDate::GetSystemDate()))
                {
                    cout << "\nThis event has already finished and cannot be deleted." << endl;
                    cout << "Press any key to choose another event...";
                    system("pause>0");
                    continue;
                }

                // Get event name for confirmation
                string eventName = table[eventIndexInTable][1];
                if (!eventName.empty()) {
                    cout << "\nEvent Details:";
                    cout << "\n-----------------\n";
                    cout << "ID: " << eventID << endl;
                    cout << "Name: " << eventName << endl;
                    cout << "-----------------\n";
                }

                // Step 3: Confirm deletion
                cout << "\nAre you sure you want to delete this event? (Y/N): ";
                string confirm;
                getline(cin >> ws, confirm);

                if (confirm == "Y" || confirm == "y") {
                    try {
                        string deleteQuery = "DELETE FROM Events WHERE id = '" + escapeString(eventID) + 
                            "' AND organizer_id = '" + to_string(CurrentUser.Id) + "'";
                        executeInstruction(deleteQuery);
                        cout << "\nEvent deleted successfully!\n";
                        break;
                    }
                    catch (const exception& e) {
                        clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, 
                            "Failed to delete event: " + string(e.what()));
                        continue;
                    }
                }
                else {
                    cout << "\nDeletion cancelled.\n";
                    break;
                }

            } while (true);

            closeDatabaseConnection();
        }
        catch (const exception& e) {
            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::General, 
                "An unexpected error occurred: " + string(e.what()));
            closeDatabaseConnection();
        }

        cout << "\nPress any key to return...";
        system("pause>0");
    }
};
