#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include "Database.h"
#include "Globals.h"
#include "clsInputValidate.h"

using namespace std;

class clsBookTicketScreen {
public:
    static void Show() {
        system("cls");
        cout << "========================================\n";
        cout << "           Book Event Ticket            \n";
        cout << "========================================\n\n";

        string query = "SELECT id, name, date, time, venue, available_tickets FROM Events WHERE available_tickets > 0";
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
        cin.ignore();
        cout << "\nEnter Event ID to book: ";
        getline(cin, eventId);

        string ticketResult;
        connectToDatabase();
        ticketResult = executeQuerySilent("SELECT available_tickets FROM Events WHERE id = " + eventId);
        closeDatabaseConnection();

        vector<string> lines = _SplitLine(ticketResult, '\n');
        if (lines.size() < 2) {
            cout << "Invalid Event ID or event is unavailable.\n";
            cout << "\nPress any key to return...";
            system("pause>0");
            return;
        }

        int availableTickets = stoi(lines[1]);


        int numTickets;
        do {
            cout << "Enter number of tickets to book (1 - " << availableTickets << "): ";
            cin >> numTickets;
            if (numTickets < 1 || numTickets > availableTickets) {
                cout << "Invalid number of tickets.\n";
            }
        } while (numTickets < 1 || numTickets > availableTickets);
        cin.ignore(); // Flush newline

        connectToDatabase();

        for (int i = 0; i < numTickets; ++i) {
            string ticketNumber = _GenerateTicketNumber();
            string insertQuery = "INSERT INTO tickets (event_id, attendee_id, ticket_number) VALUES (" +
                eventId + ", " + to_string(CurrentUser.Id) + ", '" + ticketNumber + "')";
            executeInstruction(insertQuery);
        }

        string updateQuery = "UPDATE Events SET available_tickets = available_tickets - " +
            to_string(numTickets) + " WHERE id = " + eventId;
        executeInstruction(updateQuery);

        closeDatabaseConnection();

        cout << "\nBooking successful! " << numTickets << " ticket(s) reserved.\n";
        cout << "\nPress any key to return...";
        system("pause>0");
    }

private:
    static string _GenerateTicketNumber() {
        static const char alphanum[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        string ticket;
        srand(time(0) + rand());

        for (int i = 0; i < 10; ++i) {
            ticket += alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        return ticket;
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
        cout << "-------------------------------------------------------------------------------------\n";
        cout << "| ID | Name               | Date       | Time     | Venue          | Tickets Left |\n";
        cout << "-------------------------------------------------------------------------------------\n";

        for (size_t i = 1; i < events.size(); ++i) {
            vector<string> e = events[i];
            cout << "| " << setw(2) << left << e[0]
                << "| " << setw(18) << left << e[1].substr(0, 17)
                << "| " << setw(11) << left << e[2]
                << "| " << setw(9) << left << e[3]
                << "| " << setw(15) << left << e[4].substr(0, 14)
                << "| " << setw(13) << left << e[5] << "|\n";
        }
        cout << "-------------------------------------------------------------------------------------\n";
    }
};
