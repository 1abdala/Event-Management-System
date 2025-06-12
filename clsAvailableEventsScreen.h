#pragma once
// Standard library includes
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

// Project includes
#include "clsScreen.h"
#include "Database.h"
#include "Globals.h"
#include "clsErrorHandler.h"
#include "clsInputValidate.h"

// Include this last to prevent circular dependencies
#include "clsBookTicketScreen.h"

class clsAvailableEventsScreen : protected clsScreen {
private:
    static void _DrawScreenHeader() {
        system("cls");
        cout << "\n===========================================\n";
        cout << "\t   Available Events Screen";
        cout << "\n===========================================\n";
    }

    static void _PrintEventsTableHeader() {
        cout << "\n" << setw(115) << setfill('=') << "" << setfill(' ') << "\n";
        cout << "| " << setw(5) << left << "ID"
             << "| " << setw(25) << left << "Event Name"
             << "| " << setw(12) << left << "Date"
             << "| " << setw(10) << left << "Price"
             << "| " << setw(18) << left << "Available Tickets"
             << "| " << setw(25) << left << "Speaker(s)"
             << "|" << endl;
        cout << setw(115) << setfill('=') << "" << setfill(' ') << "\n";
    }

    static void _PrintEventRow(const string& id, const string& name, 
                             const string& date, const string& price,
                             int availableTickets, const string& speakers) {
        cout << "| " << setw(5) << left << id
             << "| " << setw(25) << left << name
             << "| " << setw(12) << left << date
             << "| $" << setw(8) << left << price
             << "| " << setw(18) << left << availableTickets
             << "| " << setw(25) << left << speakers
             << " |" << endl;
        cout << setw(115) << setfill('-') << "" << setfill(' ') << "\n";
    }

public:
    static void Show() {
        try {
            string subChoice = "0";
            do {
                _DrawScreenHeader();
                
                if (!connectToDatabase()) {
                    clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, 
                        "Failed to connect to database");
                    return;
                }

                string query = "SELECT e.id, e.name, e.date, e.ticket_price, "
                             "e.available_tickets - COALESCE(COUNT(t.id), 0) as available_tickets, "
                             "COALESCE(GROUP_CONCAT(s.full_name SEPARATOR ', '), 'None') as speakers "
                             "FROM events e "
                             "LEFT JOIN tickets t ON e.id = t.event_id "
                             "LEFT JOIN speakers s ON e.id = s.event_id "
                             "WHERE e.date >= CURDATE() "
                             "AND e.id NOT IN ( "
                             "    SELECT event_id FROM tickets "
                             "    WHERE attendee_id = (SELECT id FROM users WHERE username = '" + CurrentUser.Username + "') "
                             ") "
                             "GROUP BY e.id, e.name, e.date, e.ticket_price, e.available_tickets "
                             "HAVING available_tickets > 0 "
                             "ORDER BY e.date ASC";

                string result = executeQuerySilent(query);
                if (result.empty()) {
                    clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, 
                        "Failed to retrieve events from database");
                    closeDatabaseConnection();
                    return;
                }

                // Skip header line
                size_t pos = result.find('\n');
                if (pos != string::npos) {
                    result = result.substr(pos + 1);
                }

                if (!result.empty()) {
                    _PrintEventsTableHeader();

                    stringstream ss(result);
                    string line;
                    vector<string> eventIds;

                    while (getline(ss, line)) {
                        if (line.empty()) continue;

                        vector<string> fields;
                        stringstream lineStream(line);
                        string field;
                        
                        while (getline(lineStream, field, '\t')) {
                            fields.push_back(field);
                        }

                        if (fields.size() >= 6) {
                            try {
                                _PrintEventRow(fields[0], fields[1], fields[2], fields[3], stoi(fields[4]), fields[5]);
                                eventIds.push_back(fields[0]);
                            }
                            catch (const exception& e) {
                                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::General, 
                                    "Error processing event data: " + string(e.what()));
                                continue;
                            }
                        }
                    }

                    if (eventIds.empty()) {
                        clsErrorHandler::HandleError(clsErrorHandler::enErrorType::General, 
                            "No valid events found to display");
                        closeDatabaseConnection();
                        return;
                    }

                    cout << "\nOptions:\n";
                    cout << "[1] Book a Ticket\n";
                    cout << "[0] Return to Main Menu\n";

                    subChoice = to_string(clsInputValidate::ReadIntNumberBetween(0, 1, "\nChoose what do you want to do? [0-1] "));

                    if (subChoice == "1") {
                        string eventId;
                        do {
                            cout << "\nEnter Event ID (or 0 to cancel): ";
                            eventId = clsInputValidate::ReadString();
                            
                            if (eventId == "0") break;

                            bool validEvent = false;
                            for (const string& id : eventIds) {
                                if (id == eventId) {
                                    validEvent = true;
                                    break;
                                }
                            }

                            if (!validEvent) {
                                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Validation, 
                                    "Invalid Event ID. Please try again.");
                                eventId = "";
                            }
                        } while (eventId.empty());

                        if (eventId != "0") {
                            clsBookTicketScreen::ShowBookTicket(eventId);
                        }
                    }
                }
                else {
                    clsErrorHandler::HandleError(clsErrorHandler::enErrorType::General, 
                        "No available events found.");
                    cout << "\nPress any key to return to main menu...";
                    system("pause>0");
                    subChoice = "0";
                }

                closeDatabaseConnection();

            } while (subChoice != "0");
        }
        catch (const exception& e) {
            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::General, 
                "An unexpected error occurred: " + string(e.what()));
            closeDatabaseConnection();
            cout << "\nPress any key to return...";
            system("pause>0");
        }
    }
};
