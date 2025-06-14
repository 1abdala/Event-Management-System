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

class clsUpdateEventScreen {
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
        cout << "========================================\n";
        cout << "           Update My Event              \n";
        cout << "========================================\n\n";

        if (!connectToDatabase()) {
            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, "Failed to connect to database");
            return;
        }

        cout << "--- Your Upcoming Events ---\n";
        string currentDateStr = clsDate::DateToYYYYMMDDString(clsDate::GetSystemDate());
        string listQuery = "SELECT id, name, date FROM events WHERE organizer_id = " + to_string(CurrentUser.Id) +
            " AND date >= '" + currentDateStr + "' ORDER BY date ASC";
        string result = executeQuerySilent(listQuery);

        size_t first_newline = result.find('\n');
        if (first_newline != string::npos) {
            result = result.substr(first_newline + 1);
        }

        vector<vector<string>> table = _ParseResult(result);
        vector<string> validEventIds;

        if (table.empty()) {
            cout << "You have no upcoming events to update.\n";
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

        string eventID;
        do {
            cout << "Enter Event ID to update: ";
            cin.ignore();
            getline(cin, eventID);

            bool isValidId = false;
            for (const auto& id : validEventIds) {
                if (id == eventID) {
                    isValidId = true;
                    break;
                }
            }

            if (!isValidId) {
                cout << " Invalid Event ID. Please choose from the list above.\n";
                eventID.clear();
            }

        } while (eventID.empty());

        string fetchQuery = "SELECT name, description, date, time, venue, available_tickets, ticket_price FROM events WHERE id = " + eventID;
        string eventDataResult = executeQuerySilent(fetchQuery);
        first_newline = eventDataResult.find('\n');
        if (first_newline != string::npos) {
            eventDataResult = eventDataResult.substr(first_newline + 1);
        }
        vector<string> originalData = _SplitLine(eventDataResult);

        string name = originalData[0], desc = originalData[1], date = originalData[2], time = originalData[3], venue = originalData[4];
        string tickets = originalData[5], price = originalData[6];

        cout << "\n--- Updating Event #" << eventID << " (Press Enter to keep original value) ---\n";

        cout << "Name (" << name << "): ";
        string newName; getline(cin, newName); if (newName.empty()) newName = name;

        cout << "Description (" << desc << "): ";
        string newDesc; getline(cin, newDesc); if (newDesc.empty()) newDesc = desc;

        string newDate;
        do {
            cout << "Date (YYYY-MM-DD) (" << date << "): ";
            getline(cin, newDate); if (newDate.empty()) newDate = date;
            if (!clsInputValidate::IsValidDateFormat(newDate)) { cout << "Invalid format!\n"; newDate = ""; }
        } while (newDate.empty());

        string newTime;
        do {
            cout << "Time (HH:MM:SS) (" << time << "): ";
            getline(cin, newTime); if (newTime.empty()) newTime = time;
            if (!clsInputValidate::IsValidTimeFormat(newTime)) { cout << "Invalid format!\n"; newTime = ""; }
        } while (newTime.empty());

        cout << "Venue (" << venue << "): ";
        string newVenue; getline(cin, newVenue); if (newVenue.empty()) newVenue = venue;

        string newTicketsStr;
        cout << "Available Tickets (" << tickets << "): ";
        getline(cin, newTicketsStr); if (newTicketsStr.empty()) newTicketsStr = tickets;

        string newPriceStr;
        cout << "Ticket Price (" << price << "): ";
        getline(cin, newPriceStr); if (newPriceStr.empty()) newPriceStr = price;


        string query = "UPDATE Events SET "
            "name = '" + escapeString(newName) + "', "
            "description = '" + escapeString(newDesc) + "', "
            "date = '" + escapeString(newDate) + "', "
            "time = '" + escapeString(newTime) + "', "
            "venue = '" + escapeString(newVenue) + "', "
            "available_tickets = '" + newTicketsStr + "', "
            "ticket_price = '" + newPriceStr + "' "
            "WHERE id = '" + escapeString(eventID) + "'";

        executeInstruction(query);
        closeDatabaseConnection();

        cout << "\nEvent updated successfully.\n";
        cout << "Press any key to return...";
        system("pause>0");
    }

private:
    static string escapeString(const string& input) {
        string result = input;
        size_t pos = 0;
        while ((pos = result.find('\'')) != string::npos) {
            result.insert(pos, 1, '\'');
            pos += 2;
        }
        return result;
    }
};



