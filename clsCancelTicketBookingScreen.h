#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "Database.h"
#include "Globals.h"
#include "clsInputValidate.h"
#include "clsScreen.h"
#include "clsErrorHandler.h"
#include "clsDate.h" // Required for date comparisons

using namespace std;

class clsCancelTicketBookingScreen : protected clsScreen {
private:
    static void _DrawScreenHeader() {
        system("cls");
        cout << "\n===========================================\n";
        cout << "\t   Cancel Ticket Booking Screen";
        cout << "\n===========================================\n";
    }

    static void _PrintTicketsTableHeader() {
        cout << "\n" << setw(100) << setfill('=') << "" << setfill(' ') << "\n";
        cout << setw(8) << left << "| ID"
             << setw(15) << left << "| Ticket #"
             << setw(30) << left << "| Event Name"
             << setw(15) << left << "| Event Date"
             << setw(15) << left << "| Price"
             << " |" << endl;
        cout << setw(100) << setfill('=') << "" << setfill(' ') << "\n";
    }

    static void _PrintTicketRow(const string& ticketId, const string& ticketNumber, const string& eventName, 
                                const string& eventDate, const string& price) {
        cout << "| " << setw(6) << left << ticketId
             << "| " << setw(13) << left << ticketNumber
             << "| " << setw(28) << left << eventName
             << "| " << setw(13) << left << eventDate
             << "| $" << setw(12) << left << price
             << " |" << endl;
        cout << setw(100) << setfill('-') << "" << setfill(' ') << "\n";
    }

public:
    static void Show() {
        _DrawScreenHeader();

        if (!connectToDatabase()) {
            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, "Failed to connect to database");
            cout << "\nPress any key to return...";
            system("pause>0");
            return;
        }

        // Fetch booked tickets for the current user
        string query = "SELECT t.id, t.ticket_number, e.name, e.date, e.ticket_price, e.id as event_id "
                     "FROM tickets t "
                     "JOIN events e ON t.event_id = e.id "
                     "WHERE t.attendee_id = (SELECT id FROM users WHERE username = '" + CurrentUser.Username + "') "
                     "ORDER BY e.date ASC";

        string result = executeQuerySilent(query);
        
        size_t headerPos = result.find('\n');
        string dataOnlyResult = (headerPos != string::npos) ? result.substr(headerPos + 1) : "";

        if (dataOnlyResult.empty()) {
            cout << "\nYou have no booked tickets to cancel.\n";
        } else {
            _PrintTicketsTableHeader();
            stringstream ss(dataOnlyResult);
            string line;
            vector<pair<string, string>> ticketIdToEventIdMap; // Stores pair <ticket_id, event_id>

            while (getline(ss, line)) {
                if (line.empty()) continue;
                vector<string> fields;
                stringstream lineStream(line);
                string field;
                while (getline(lineStream, field, '\t')) {
                    fields.push_back(field);
                }
                if (fields.size() >= 6) { // id, ticket_number, name, date, price, event_id
                    _PrintTicketRow(fields[0], fields[1], fields[2], fields[3], fields[4]);
                    ticketIdToEventIdMap.push_back({fields[0], fields[5]});
                }
            }

            if (ticketIdToEventIdMap.empty()) {
                 cout << "\nNo valid ticket data found to display.\n";
            } else {
                cout << "\nEnter Ticket ID to cancel (or 0 to go back): ";
                string ticketIdToCancel = clsInputValidate::ReadString();

                if (ticketIdToCancel != "0") {
                    bool foundTicket = false;
                    string eventIdForTicket;
                    string eventDateForTicket;

                    // Find the event_id and event_date for the selected ticket_id
                    // This requires re-parsing or finding from the initially fetched 'result'
                    // For simplicity here, let's re-fetch the specific ticket's event date
                    
                    bool validTicketId = false;
                    for(const auto& pair : ticketIdToEventIdMap) {
                        if (pair.first == ticketIdToCancel) {
                            validTicketId = true;
                            eventIdForTicket = pair.second; // Get the event_id
                            
                            // Fetch event date for this specific ticket
                            string eventDateQuery = "SELECT date FROM events WHERE id = " + eventIdForTicket;
                            string eventDateResult = executeQuerySilent(eventDateQuery);
                            size_t dateHeaderPos = eventDateResult.find('\n');
                            if (dateHeaderPos != string::npos && eventDateResult.length() > dateHeaderPos + 1) {
                                eventDateForTicket = eventDateResult.substr(dateHeaderPos + 1);
                                // Remove potential trailing newline/whitespace from single value result
                                eventDateForTicket.erase(eventDateForTicket.find_last_not_of(" \n\r\t")+1);
                            } else {
                                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, "Could not fetch event date for validation.");
                                closeDatabaseConnection();
                                cout << "\nPress any key to return...";
                                system("pause>0");
                                return;
                            }
                            foundTicket = true;
                            break;
                        }
                    }


                    if (!foundTicket) {
                        cout << "\nInvalid Ticket ID entered.\n";
                    } else {
                        // Convert event date string (YYYY-MM-DD) to clsDate object
                        short eventYear = stoi(eventDateForTicket.substr(0, 4));
                        short eventMonth = stoi(eventDateForTicket.substr(5, 2));
                        short eventDay = stoi(eventDateForTicket.substr(8, 2));
                        clsDate eventDateObj(eventDay, eventMonth, eventYear);
                        clsDate currentDateObj = clsDate::GetSystemDate();
                        
                        int daysDifference = clsDate::GetDifferenceInDays(currentDateObj, eventDateObj);

                        if (daysDifference <= 1) {
                            cout << "\nTickets cannot be cancelled if the event is today or tomorrow.\n";
                        } else {
                            // Proceed with cancellation
                            string deleteQuery = "DELETE FROM tickets WHERE id = " + ticketIdToCancel + 
                                                 " AND attendee_id = (SELECT id FROM users WHERE username = '" + CurrentUser.Username + "')";
                            try {
                                executeInstruction(deleteQuery); // This should ideally return rows affected

                                // Increment available_tickets in events table
                                string updateEventQuery = "UPDATE events SET available_tickets = available_tickets + 1 WHERE id = " + eventIdForTicket;
                                executeInstruction(updateEventQuery);

                                cout << "\nTicket cancelled successfully!\n";
                            } catch (const std::exception& e) {
                                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, 
                                    "Failed to cancel ticket. Error: " + string(e.what()));
                            }
                        }
                    }
                }
            }
        }
        closeDatabaseConnection();
        cout << "\nPress any key to return...";
        system("pause>0");
    }
}; 