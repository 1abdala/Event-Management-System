#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <map> // For storing event data for validation
#include "Database.h"
#include "Globals.h"
#include "clsScreen.h"
#include "clsInputValidate.h"
#include "clsDate.h" // For date formatting if needed

using namespace std;

class clsViewEventAttendeesScreen : protected clsScreen {
private:
    static void _DrawScreenHeader(const string& Title, const string& SubTitle = "") {
        system("cls");
        cout << "\n===================================================================\n";
        cout << setw(50) << right << Title << "\n";
        if (!SubTitle.empty()) {
            cout << setw(50) << right << SubTitle << "\n";
        }
        cout << "===================================================================\n";
        cout << "User: " << CurrentUser.Username << "\t\t";
        cout << "Date: " << clsDate::DateToString(clsDate::GetSystemDate()) << "\n";
        cout << "-------------------------------------------------------------------\n\n";
    }

    static map<string, pair<string, string>> _PrintAllEventsWithAttendeeCount() {
        _DrawScreenHeader("View Event Attendees - All Events");
        map<string, pair<string, string>> mapEvents; // Stores ID -> {Name, Date} for validation

        connectToDatabase();
        string query = "SELECT e.id, e.name, e.date, COUNT(t.event_id) AS attendee_count "
                       "FROM events e "
                       "LEFT JOIN tickets t ON e.id = t.event_id "
                       "GROUP BY e.id, e.name, e.date "
                       "ORDER BY e.date DESC";
        
        string result = executeQuerySilent(query);
        closeDatabaseConnection();

        // Skip header line from SQL result
        size_t pos = result.find('\n');
        if (pos != string::npos) {
            result = result.substr(pos + 1);
        }

        cout << setw(90) << setfill('=') << "" << setfill(' ') << "\n";
        cout << "| " << setw(5) << left << "ID"
             << "| " << setw(35) << left << "Event Name"
             << "| " << setw(15) << left << "Date"
             << "| " << setw(20) << left << "Attendees Count"
             << " |\n";
        cout << setw(90) << setfill('=') << "" << setfill(' ') << "\n";

        if (result.empty()) {
            cout << "| " << setw(86) << left << "No events found." << " |\n";
        } else {
            stringstream ss(result);
            string line;
            while (getline(ss, line)) {
                if (line.empty()) continue;

                stringstream lineStream(line);
                string id, name, date, count;
                getline(lineStream, id, '\t');
                getline(lineStream, name, '\t');
                getline(lineStream, date, '\t');
                getline(lineStream, count, '\t');
                
                cout << "| " << setw(5) << left << id
                     << "| " << setw(35) << left << name.substr(0, 33)
                     << "| " << setw(15) << left << date.substr(0, 10) // Show YYYY-MM-DD
                     << "| " << setw(20) << left << count
                     << " |\n";
                mapEvents[id] = {name, date};
            }
        }
        cout << setw(90) << setfill('=') << "" << setfill(' ') << "\n\n";
        return mapEvents;
    }

    static void _PrintEventAttendees(const string& eventId, const string& eventName, const string& eventDate) {
        _DrawScreenHeader("View Attendees for Event", "Event: " + eventName + " (" + eventId + ") on " + eventDate.substr(0,10));

        connectToDatabase();
        string query = "SELECT u.username, CONCAT(u.first_name, ' ', u.last_name) AS full_name, t.booking_time "
                       "FROM tickets t "
                       "JOIN users u ON t.attendee_id = u.id "
                       "WHERE t.event_id = " + eventId + " "
                       "ORDER BY t.booking_time ASC";
        
        string result = executeQuerySilent(query);
        closeDatabaseConnection();

        // Skip header line from SQL result
        size_t pos = result.find('\n');
        if (pos != string::npos) {
            result = result.substr(pos + 1);
        }

        cout << setw(80) << setfill('=') << "" << setfill(' ') << "\n";
        cout << "| " << setw(20) << left << "Username"
             << "| " << setw(30) << left << "Full Name"
             << "| " << setw(23) << left << "Booking Date/Time"
             << " |\n";
        cout << setw(80) << setfill('=') << "" << setfill(' ') << "\n";

        if (result.empty()) {
            cout << "| " << setw(76) << left << "No attendees found for this event." << " |\n";
        } else {
            stringstream ss(result);
            string line;
            while (getline(ss, line)) {
                if (line.empty()) continue;

                stringstream lineStream(line);
                string username, fullName, bookingTime;
                getline(lineStream, username, '\t');
                getline(lineStream, fullName, '\t');
                getline(lineStream, bookingTime, '\t');

                cout << "| " << setw(20) << left << username
                     << "| " << setw(30) << left << fullName.substr(0,28)
                     << "| " << setw(23) << left << bookingTime.substr(0,19) // Show YYYY-MM-DD HH:MM:SS
                     << " |\n";
            }
        }
        cout << setw(80) << setfill('=') << "" << setfill(' ') << "\n\n";
        cout << "Press any key to return to the events list...";
        system("pause>0");
    }

public:
    static void Show() {
        string choice = "";
        do {
            map<string, pair<string, string>> mapEvents = _PrintAllEventsWithAttendeeCount();

            if (mapEvents.empty()) {
                cout << "Press any key to return to the main menu...";
                system("pause>0");
                return; 
            }

            cout << "Enter Event ID to view its attendees (or 0 to go back): ";
            choice = clsInputValidate::ReadString();

            if (choice == "0") {
                break;
            }

            if (mapEvents.count(choice)) {
                _PrintEventAttendees(choice, mapEvents[choice].first, mapEvents[choice].second);
            } else {
                cout << "\nInvalid Event ID. Please try again.\n";
                system("pause>0");
            }

        } while (true); // Loop indefinitely until user chooses to go back
    }
};
