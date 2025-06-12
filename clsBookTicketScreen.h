#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include "Database.h"
#include "Globals.h"
#include "clsInputValidate.h"
#include "clsScreen.h"
#include "clsErrorHandler.h"

using namespace std;

class clsBookTicketScreen : protected clsScreen {
public:
    static void Show() {
        system("cls");
        cout << "========================================\n";
        cout << "           Book Event Ticket            \n";
        cout << "========================================\n\n";

        string query = "SELECT e.id, e.name, e.date, e.time, e.venue, e.available_tickets, e.ticket_price "
                       "FROM Events e "
                       "WHERE e.available_tickets > 0 "
                       "AND e.id NOT IN ("
                       "    SELECT t.event_id FROM tickets t "
                       "    WHERE t.attendee_id = (SELECT id FROM users WHERE username = '" + CurrentUser.Username + "')"
                       ")";

        connectToDatabase();
        string result = executeQuerySilent(query);
        closeDatabaseConnection();

        vector<vector<string>> events = _ParseResult(result);

        if (events.size() <= 1) {
            cout << "No available events for booking.\n";
            cout << "\nPress any key to return...";
            system("pause>0");
            return;
        }

        _PrintEvents(events);

        string eventId;
        int availableTickets = 0;
        float ticketPrice = 0.0f;
        bool validEvent = false;

        do {
            cout << "\nEnter Event ID to book: ";
            getline(cin, eventId);

            if (!clsInputValidate::IsNumber(eventId)) {
                cout << "Invalid input. Please enter a numeric Event ID.\n";
                continue;
            }

            string ticketResult;
            connectToDatabase();
            ticketResult = executeQuerySilent("SELECT available_tickets, ticket_price FROM Events WHERE id = " + eventId);
            closeDatabaseConnection();

            vector<string> lines = _SplitLine(ticketResult, '\n');
            if (lines.size() >= 2) {
                vector<string> ticketData = _SplitLine(lines[1]);
                availableTickets = stoi(ticketData[0]);
                ticketPrice = stof(ticketData[1]);
                validEvent = true;
            }
            else {
                cout << "Invalid Event ID or event is unavailable.\n";
            }
        } while (!validEvent);

        int numTickets=1;
       
        cin.ignore(); // flush newline

        cout << "\nTicket Price : RM " << fixed << setprecision(2) << ticketPrice;

        connectToDatabase();

        
            string ticketNumber = _GenerateTicketNumber();

            string insertTicketQuery = "INSERT INTO tickets (event_id, attendee_id, ticket_number) VALUES (" +
                eventId + ", " + to_string(CurrentUser.Id) + ", '" + ticketNumber + "')";
            executeInstruction(insertTicketQuery);

            string insertAttendanceQuery = "INSERT INTO attendance (event_id, user_id) VALUES (" +
                eventId + ", " + to_string(CurrentUser.Id) + ")";
            executeInstruction(insertAttendanceQuery);
        

        string updateQuery = "UPDATE Events SET available_tickets = available_tickets - " +
            to_string(numTickets) + " WHERE id = " + eventId;
        executeInstruction(updateQuery);

        closeDatabaseConnection();

        cout << "\nBooking successful! " << numTickets << " ticket reserved.\n";
        cout << "\nPress any key to return...";
        system("pause>0");
    }

    static void ShowBookTicket(const string& eventId) {
        try {
            _DrawScreenHeader();

            if (!connectToDatabase()) {
                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, 
                    "Failed to connect to database");
                return;
            }

            // Check if user already has a ticket for this event
            if (_HasExistingTicket(eventId)) {
                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Booking, 
                    "You already have a ticket for this event. Only one ticket per event is allowed.");
                closeDatabaseConnection();
                return;
            }

            // Check if tickets are still available
            if (!_HasAvailableTickets(eventId)) {
                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Booking, 
                    "This event is sold out.");
                closeDatabaseConnection();
                return;
            }

            // Get event details
            string query = "SELECT name, date, ticket_price FROM events WHERE id = " + eventId;
            string result = executeQuerySilent(query);

            if (result.empty()) {
                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, 
                    "Event not found or database error occurred.");
                closeDatabaseConnection();
                return;
            }

            size_t pos = result.find('\n');
            if (pos != string::npos) {
                result = result.substr(pos + 1);
            }

            stringstream ss(result);
            string line;
            getline(ss, line);

            vector<string> fields;
            stringstream lineStream(line);
            string field;
            
            while (getline(lineStream, field, '\t')) {
                fields.push_back(field);
            }

            if (fields.size() >= 3) {
                cout << "\nEvent Details:\n";
                cout << "Name: " << fields[0] << endl;
                cout << "Date: " << fields[1] << endl;
                cout << "Ticket Price: $" << fields[2] << endl;

                cout << "\nDo you want to book a ticket for this event?\n";
                cout << "[1] Yes\n";
                cout << "[0] No\n";

                int choice = clsInputValidate::ReadIntNumberBetween(0, 1, "\nEnter your choice [0-1]: ");

                if (choice == 1) {
                    string ticketNumber = _GenerateTicketNumber();
                    if (ticketNumber.empty()) {
                        clsErrorHandler::HandleError(clsErrorHandler::enErrorType::General, 
                            "Failed to generate ticket number.");
                        closeDatabaseConnection();
                        return;
                    }
                    
                    // Insert ticket record
                    string insertQuery = "INSERT INTO tickets (event_id, attendee_id, ticket_number) "
                                       "VALUES (" + eventId + ", "
                                       "(SELECT id FROM users WHERE username = '" + CurrentUser.Username + "'), "
                                       "'" + ticketNumber + "')";
                    
                    bool bookingSuccess = true;
                    try {
                        executeInstruction(insertQuery);
                    }
                    catch (const std::exception& e) {
                        bookingSuccess = false;
                        clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, 
                            "Failed to book ticket. Error: " + string(e.what()));
                    }

                    if (bookingSuccess) {
                        cout << "\nTicket booked successfully!\n";
                        cout << "Your ticket number is: " << ticketNumber << "\n";
                        cout << "Please keep this number for your records.\n";

                        // Decrement available tickets in the events table
                        string updateEventQuery = "UPDATE events SET available_tickets = available_tickets - 1 WHERE id = " + eventId + " AND available_tickets > 0";
                        try {
                            executeInstruction(updateEventQuery);
                        }
                        catch (const std::exception& e) {
                            // Log this error, as the ticket is booked but event count failed to update.
                            // This might indicate a need for a transaction in a more robust system.
                            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, 
                                "Failed to update event ticket count. Error: " + string(e.what()));
                        }
                    }
                }
            }
            else {
                clsErrorHandler::HandleError(clsErrorHandler::enErrorType::Database, 
                    "Invalid event data received from database.");
            }

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

private:
    static void _DrawScreenHeader() {
        system("cls");
        cout << "\n===========================================\n";
        cout << "\t   Book Ticket Screen";
        cout << "\n===========================================\n";
    }

    static bool _HasExistingTicket(const string& eventId) {
        try {
            string query = "SELECT COUNT(*) FROM tickets t "
                          "WHERE t.event_id = " + eventId + " "
                          "AND t.attendee_id = (SELECT id FROM users WHERE username = '" + CurrentUser.Username + "')";
            
            string result = executeQuerySilent(query);
            size_t pos = result.find('\n');
            if (pos != string::npos) {
                result = result.substr(pos + 1);
            }
            
            return !result.empty() && stoi(result) > 0;
        }
        catch (const exception& e) {
            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::General, 
                "Error checking existing ticket: " + string(e.what()));
            return false;
        }
    }

    static bool _HasAvailableTickets(const string& eventId) {
        try {
            string query = "SELECT (e.available_tickets - COUNT(t.id)) as available_tickets "
                          "FROM events e "
                          "LEFT JOIN tickets t ON e.id = t.event_id "
                          "WHERE e.id = " + eventId + " "
                          "GROUP BY e.id";
            
            string result = executeQuerySilent(query);
            size_t pos = result.find('\n');
            if (pos != string::npos) {
                result = result.substr(pos + 1);
            }
            
            return !result.empty() && stoi(result) > 0;
        }
        catch (const exception& e) {
            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::General, 
                "Error checking ticket availability: " + string(e.what()));
            return false;
        }
    }

    static string _GenerateTicketNumber() {
        try {
            const string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            string ticketNumber;
            srand(time(0));
            
            for (int i = 0; i < 10; i++) {
                ticketNumber += chars[rand() % chars.length()];
            }
            
            return ticketNumber;
        }
        catch (const exception& e) {
            clsErrorHandler::HandleError(clsErrorHandler::enErrorType::General, 
                "Error generating ticket number: " + string(e.what()));
            return "";
        }
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

    static void _PrintEvents(const vector<vector<string>>& events) {
        cout << "Available Events:\n";
        cout << "-------------------------------------------------------------------------------------------------------------\n";
        cout << "| ID | Name               | Date       | Time     | Venue          | Tickets Left | Ticket Price (RM)       |\n";
        cout << "-------------------------------------------------------------------------------------------------------------\n";

        for (size_t i = 1; i < events.size(); ++i) {
            vector<string> e = events[i];
            cout << "| " << setw(2) << left << e[0]
                << "| " << setw(18) << left << e[1].substr(0, 17)
                << "| " << setw(11) << left << e[2]
                << "| " << setw(9) << left << e[3]
                << "| " << setw(15) << left << e[4].substr(0, 14)
                << "| " << setw(13) << left << e[5]
                << "| RM " << setw(20) << left << e[6]
                << "|\n";
        }
        cout << "-------------------------------------------------------------------------------------------------------------\n";
    }
};
