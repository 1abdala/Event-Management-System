#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include "Database.h"
#include "Globals.h"
#include "clsScreen.h"
#include "clsInputValidate.h"
#include "clsDate.h"

using namespace std;

class clseSearchAttendee : protected clsScreen {
private:
    static void _DrawScreenHeader() {
        system("cls");
        cout << "\n===========================================\n";
        cout << "\t   Search Attendee Screen";
        cout << "\n===========================================\n";
    }

    static void _PrintAttendeeInfo(const string& attendeeId, const string& firstName, 
                                 const string& lastName, const string& username, 
                                 const string& email) {
        cout << "\nAttendee Information:\n";
        cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";
        cout << "| " << setw(15) << left << "ID" << ": " << setw(30) << left << attendeeId << " |\n";
        cout << "| " << setw(15) << left << "Name" << ": " << setw(30) << left << (firstName + " " + lastName) << " |\n";
        cout << "| " << setw(15) << left << "Username" << ": " << setw(30) << left << username << " |\n";
        cout << "| " << setw(15) << left << "Email" << ": " << setw(30) << left << email << " |\n";
        cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";
    }

    static void _PrintAttendeeEvents(const vector<vector<string>>& events) {
        if (events.empty()) {
            cout << "\nThis attendee hasn't booked any events yet.\n";
            return;
        }

        cout << "\nBooked Events:\n";
        cout << setw(100) << setfill('=') << "" << setfill(' ') << "\n";
        cout << "| " << setw(8) << left << "Event ID"
             << "| " << setw(30) << left << "Event Name"
             << "| " << setw(15) << left << "Date"
             << "| " << setw(15) << left << "Price"
             << "| " << setw(20) << left << "Booking Date"
             << " |\n";
        cout << setw(100) << setfill('=') << "" << setfill(' ') << "\n";

        double totalSpent = 0.0;
        int totalEvents = 0;
        int organizerEvents = 0;

        for (const auto& event : events) {
            if (event.size() >= 6) { // event_id, name, date, price, booking_time, organizer_id
                cout << "| " << setw(8) << left << event[0]
                     << "| " << setw(30) << left << event[1].substr(0, 28)
                     << "| " << setw(15) << left << event[2].substr(0, 10)
                     << "| $" << setw(13) << left << fixed << setprecision(2) << stod(event[3])
                     << "| " << setw(20) << left << event[4].substr(0, 19)
                     << " |\n";
                cout << setw(100) << setfill('-') << "" << setfill(' ') << "\n";

                totalSpent += stod(event[3]);
                totalEvents++;
                if (event[5] == to_string(CurrentUser.Id)) {
                    organizerEvents++;
                }
            }
        }

        cout << "\nSummary:\n";
        cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";
        cout << "| " << setw(30) << left << "Total Events Booked" << ": " << setw(15) << right << totalEvents << " |\n";
        cout << "| " << setw(30) << left << "Events from Your Organization" << ": " << setw(15) << right << organizerEvents << " |\n";
        cout << "| " << setw(30) << left << "Total Amount Spent" << ": $" << setw(14) << right << fixed << setprecision(2) << totalSpent << " |\n";
        cout << "| " << setw(30) << left << "Average Price per Event" << ": $" << setw(14) << right << fixed << setprecision(2) << (totalEvents > 0 ? totalSpent / totalEvents : 0.0) << " |\n";
        cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";
    }

public:
    static void Show() {
        _DrawScreenHeader();
        
        string attendeeId;
        cout << "Enter Attendee ID to search (or 0 to cancel): ";
        attendeeId = clsInputValidate::ReadString();

        if (attendeeId == "0") {
            return;
        }

        connectToDatabase();

        // First, get attendee information
        string attendeeQuery = "SELECT id, first_name, last_name, username, email "
                             "FROM users "
                             "WHERE id = '" + attendeeId + "' AND role = 'attendee'";

        string attendeeResult = executeQuerySilent(attendeeQuery);

        // Skip header line
        size_t pos = attendeeResult.find('\n');
        if (pos != string::npos) {
            attendeeResult = attendeeResult.substr(pos + 1);
        }

        if (attendeeResult.empty()) {
            cout << "\nNo attendee found with ID: " << attendeeId << "\n";
            closeDatabaseConnection();
            cout << "\nPress any key to return to the main menu...";
            system("pause>0");
            return;
        }

        // Parse attendee information
        stringstream attendeeSS(attendeeResult);
        string attendeeLine;
        getline(attendeeSS, attendeeLine);
        vector<string> attendeeFields;
        stringstream attendeeLineStream(attendeeLine);
        string field;
        while (getline(attendeeLineStream, field, '\t')) {
            attendeeFields.push_back(field);
        }

        if (attendeeFields.size() >= 5) {
            _PrintAttendeeInfo(attendeeFields[0], attendeeFields[1], attendeeFields[2], 
                             attendeeFields[3], attendeeFields[4]);

            // Now get all events this attendee has booked
            string eventsQuery = "SELECT e.id, e.name, e.date, e.ticket_price, t.booking_time, e.organizer_id "
                               "FROM tickets t "
                               "JOIN events e ON t.event_id = e.id "
                               "WHERE t.attendee_id = '" + attendeeId + "' "
                               "ORDER BY e.date DESC";

            string eventsResult = executeQuerySilent(eventsQuery);

            // Skip header line
            pos = eventsResult.find('\n');
            if (pos != string::npos) {
                eventsResult = eventsResult.substr(pos + 1);
            }

            // Parse events
            vector<vector<string>> events;
            stringstream eventsSS(eventsResult);
            string eventLine;
            while (getline(eventsSS, eventLine)) {
                if (eventLine.empty()) continue;
                vector<string> eventFields;
                stringstream eventLineStream(eventLine);
                while (getline(eventLineStream, field, '\t')) {
                    eventFields.push_back(field);
                }
                events.push_back(eventFields);
            }

            _PrintAttendeeEvents(events);
        }

        closeDatabaseConnection();
        
    }
};

