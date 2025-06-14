#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include "clsScreen.h"
#include "clsInputValidate.h"
#include "Database.h"
#include "Globals.h"
#include "clsUser.h"

using namespace std;

class clsAdminMainMenuScreen : protected clsScreen {
private:
    enum enAdminMenuOptions {
        eSearchUser = 1,
        eDeleteOrganizer = 2,
        eDeleteAttendee = 3,
        eViewReport = 4,
        eLogout = 5
    };

    static string escapeString(const string& input) {
        string output;
        for (char c : input) {
            if (c == '\'') {
                output += "''"; // SQL escape for single quote
            }
            else {
                output += c;
            }
        }
        return output;
    }

    static void _DrawScreenHeader() {
        system("cls");
        cout << "\n===========================================\n";
        cout << "\t   Admin Main Menu Screen";
        cout << "\n===========================================\n";
    }

    static void _PrintMenuOptions() {
        cout << "\n[1] Search User\n";
        cout << "[2] Delete Organizer\n";
        cout << "[3] Delete Attendee\n";
        cout << "[4] View Report\n";
        cout << "[5] Logout\n";
        cout << "===========================================\n";
        cout << "Enter your choice: ";
    }

    static void _SearchUser() {
        system("cls");
        cout << "\n===========================================\n";
        cout << "\t   Search User Screen";
        cout << "\n===========================================\n";

        cout << "\nSelect User Type:\n";
        cout << "[1] Organizer\n";
        cout << "[2] Attendee\n";
        cout << "Enter your choice (1-2): ";
        int userType = clsInputValidate::ReadIntNumberBetween(1, 2, "Invalid choice. Please enter 1 for Organizer or 2 for Attendee: ");

        string userId;
        cout << "\nEnter User ID to search (or 0 to cancel): ";
        userId = clsInputValidate::ReadString();

        if (userId == "0") return;

        connectToDatabase();

        string role = (userType == 1) ? "organizer" : "attendee";
        string query = "SELECT * FROM users WHERE id = '" + userId + "' AND role = '" + role + "'";
        string result = executeQuerySilent(query);

        // Check if result is empty or contains only header
        if (result.empty() || result.find("\n") == string::npos || result.find("\n") == result.length() - 1) {
            cout << "\n===========================================\n";
            cout << "\t   User Not Found";
            cout << "\n===========================================\n";
            cout << "\nNo " << role << " found with ID: " << userId << "\n";
            cout << "Please check the ID and try again.\n";
            closeDatabaseConnection();
            cout << "\nPress any key to return to the main menu...";
            system("pause>0");
            return;
        }

        // Parse user data
        stringstream ss(result);
        string line;
        getline(ss, line); // Skip header
        getline(ss, line);
        vector<string> fields;
        stringstream lineStream(line);
        string field;
        while (getline(lineStream, field, '\t')) {
            fields.push_back(field);
        }

        if (fields.size() >= 6) {
            cout << "\nUser Information:\n";
            cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";
            cout << "| " << setw(15) << left << "ID" << ": " << setw(30) << left << fields[0] << " |\n";
            cout << "| " << setw(15) << left << "Name" << ": " << setw(30) << left << (fields[1] + " " + fields[2]) << " |\n";
            cout << "| " << setw(15) << left << "Username" << ": " << setw(30) << left << fields[3] << " |\n";
            cout << "| " << setw(15) << left << "Email" << ": " << setw(30) << left << fields[4] << " |\n";
            cout << "| " << setw(15) << left << "Status" << ": " << setw(30) << left << (fields[8] == "1" ? "Active" : "Inactive") << " |\n";
            cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";

            if (role == "organizer") {
                _ShowOrganizerStats(userId);
            } else {
                _ShowAttendeeStats(userId);
            }
        }

        closeDatabaseConnection();
        cout << "\nPress any key to return to the main menu...";
        system("pause>0");
    }

    static void _ShowOrganizerStats(const string& organizerId) {
        // Get total events and revenue
        string eventsQuery = "SELECT "
                           "COUNT(DISTINCT e.id) as total_events, "
                           "COALESCE(SUM(e.ticket_price * COALESCE(t.ticket_count, 0)), 0) as total_revenue, "
                           "COALESCE(AVG(e.ticket_price), 0) as avg_ticket_price, "
                           "COALESCE(AVG((COALESCE(t.ticket_count, 0) * 100.0) / NULLIF(e.available_tickets, 0)), 0) as avg_sold_percentage "
                           "FROM events e "
                           "LEFT JOIN ("
                           "    SELECT event_id, COUNT(*) as ticket_count "
                           "    FROM tickets "
                           "    GROUP BY event_id"
                           ") t ON e.id = t.event_id "
                           "WHERE e.organizer_id = '" + organizerId + "'";
        
        string eventsResult = executeQuerySilent(eventsQuery);
        stringstream ss(eventsResult);
        string line;
        getline(ss, line); // Skip header
        getline(ss, line);
        vector<string> stats;
        stringstream lineStream(line);
        string stat;
        while (getline(lineStream, stat, '\t')) {
            stats.push_back(stat);
        }

        if (stats.size() >= 4) {
            cout << "\nOrganizer Statistics:\n";
            cout << setw(55) << setfill('-') << "" << setfill(' ') << "\n";
            cout << "| " << setw(35) << left << "Total Events Created" << " | " << setw(12) << left << stats[0] << " |\n";
            
            // Handle total revenue
            double revenue = 0.0;
            try {
                revenue = stod(stats[1]);
            } catch (...) {
                revenue = 0.0;
            }
            cout << "| " << setw(35) << left << "Total Revenue" << " | $" << setw(11) << left << fixed << setprecision(2) << revenue << " |\n";
            
            // Handle average ticket price
            double avgPrice = 0.0;
            try {
                avgPrice = stod(stats[2]);
            } catch (...) {
                avgPrice = 0.0;
            }
            cout << "| " << setw(35) << left << "Average Ticket Price" << " | $" << setw(11) << left << fixed << setprecision(2) << avgPrice << " |\n";
            
            // Handle average tickets sold percentage
            double avgPercentage = 0.0;
            try {
                avgPercentage = stod(stats[3]);
            } catch (...) {
                avgPercentage = 0.0;
            }
            cout << "| " << setw(35) << left << "Average Tickets Sold per Event %" << " | " << setw(11) << left << fixed << setprecision(1) << avgPercentage << "% |\n";
            cout << setw(55) << setfill('-') << "" << setfill(' ') << "\n";

            // Show all events
            string recentEventsQuery = "SELECT e.name, e.date, e.ticket_price, e.available_tickets, "
                                     "COALESCE(t.ticket_count, 0) as tickets_sold "
                                     "FROM events e "
                                     "LEFT JOIN ("
                                     "    SELECT event_id, COUNT(*) as ticket_count "
                                     "    FROM tickets "
                                     "    GROUP BY event_id"
                                     ") t ON e.id = t.event_id "
                                     "WHERE e.organizer_id = '" + organizerId + "' "
                                     "ORDER BY e.date DESC";
            string recentEventsResult = executeQuerySilent(recentEventsQuery);
            
            cout << "\nAll Events:\n";
            cout << setw(85) << setfill('-') << "" << setfill(' ') << "\n";
            cout << "| " << setw(35) << left << "Event Name" << " | "
                 << setw(12) << left << "Date" << " | "
                 << setw(10) << left << "Price" << " | "
                 << setw(8) << left << "Sold" << " | "
                 << setw(5) << left << "Total" << "|\n";
            cout << setw(85) << setfill('-') << "" << setfill(' ') << "\n";

            stringstream eventsSS(recentEventsResult);
            getline(eventsSS, line); // Skip header
            while (getline(eventsSS, line)) {
                vector<string> eventFields;
                stringstream eventStream(line);
                string field;
                while (getline(eventStream, field, '\t')) {
                    eventFields.push_back(field);
                }
                if (eventFields.size() >= 5) {
                    // Calculate total tickets (available + sold)
                    int availableTickets = 0;
                    int ticketsSold = 0;
                    try {
                        availableTickets = stoi(eventFields[3]);
                        ticketsSold = stoi(eventFields[4]);
                    } catch (...) {
                        availableTickets = 0;
                        ticketsSold = 0;
                    }
                    int totalTickets = availableTickets + ticketsSold;

                    cout << "| " << setw(35) << left << eventFields[0] << " | "
                         << setw(12) << left << eventFields[1] << " | "
                         << setw(10) << left << fixed << setprecision(2) << stod(eventFields[2]) << " | "
                         << setw(8) << left << ticketsSold << " | "
                         << setw(5) << left << totalTickets << "|\n";
                }
            }
            cout << setw(85) << setfill('-') << "" << setfill(' ') << "\n";
        }
    }

    static void _ShowAttendeeStats(const string& attendeeId) {
        // Get total events and spending
        string eventsQuery = "SELECT COUNT(DISTINCT e.id) as total_events, "
                           "SUM(e.ticket_price) as total_spending, "
                           "AVG(e.ticket_price) as avg_ticket_price "
                           "FROM tickets t "
                           "JOIN events e ON t.event_id = e.id "
                           "WHERE t.attendee_id = '" + attendeeId + "'";
        
        string eventsResult = executeQuerySilent(eventsQuery);
        stringstream ss(eventsResult);
        string line;
        getline(ss, line); // Skip header
        getline(ss, line);
        vector<string> stats;
        stringstream lineStream(line);
        string stat;
        while (getline(lineStream, stat, '\t')) {
            stats.push_back(stat);
        }

        if (stats.size() >= 3) {
            cout << "\nAttendee Statistics:\n";
            cout << setw(55) << setfill('-') << "" << setfill(' ') << "\n";
            cout << "| " << setw(35) << left << "Total Events Booked" << " | " << setw(12) << left << stats[0] << " |\n";
            
            // Handle total spending
            double spending = 0.0;
            try {
                spending = stod(stats[1]);
            } catch (...) {
                spending = 0.0;
            }
            cout << "| " << setw(35) << left << "Total Spending" << " | $" << setw(11) << left << fixed << setprecision(2) << spending << " |\n";
            
            // Handle average ticket price
            double avgPrice = 0.0;
            try {
                avgPrice = stod(stats[2]);
            } catch (...) {
                avgPrice = 0.0;
            }
            cout << "| " << setw(35) << left << "Average Ticket Price" << " | $" << setw(11) << left << fixed << setprecision(2) << avgPrice << " |\n";
            cout << setw(55) << setfill('-') << "" << setfill(' ') << "\n";

            // Show all attended events
            string attendedEventsQuery = "SELECT e.name, e.date, e.ticket_price, COUNT(*) as ticket_count "
                                       "FROM tickets t "
                                       "JOIN events e ON t.event_id = e.id "
                                       "WHERE t.attendee_id = '" + attendeeId + "' "
                                       "GROUP BY e.id, e.name, e.date, e.ticket_price "
                                       "ORDER BY e.date DESC";
            string attendedEventsResult = executeQuerySilent(attendedEventsQuery);
            
            cout << "\nAll Attended Events:\n";
            cout << setw(85) << setfill('-') << "" << setfill(' ') << "\n";
            cout << "| " << setw(35) << left << "Event Name" << " | "
                 << setw(12) << left << "Date" << " | "
                 << setw(10) << left << "Price" << " | "
                 << setw(8) << left << "Quantity" << " | "
                 << setw(5) << left << "Total" << "|\n";
            cout << setw(85) << setfill('-') << "" << setfill(' ') << "\n";

            stringstream eventsSS(attendedEventsResult);
            getline(eventsSS, line); // Skip header
            while (getline(eventsSS, line)) {
                vector<string> eventFields;
                stringstream eventStream(line);
                string field;
                while (getline(eventStream, field, '\t')) {
                    eventFields.push_back(field);
                }
                if (eventFields.size() >= 4) {
                    // Calculate total for this event
                    double price = 0.0;
                    int quantity = 0;
                    try {
                        price = stod(eventFields[2]);
                        quantity = stoi(eventFields[3]);
                    } catch (...) {
                        price = 0.0;
                        quantity = 0;
                    }
                    double total = price * quantity;

                    cout << "| " << setw(35) << left << eventFields[0] << " | "
                         << setw(12) << left << eventFields[1] << " | "
                         << setw(10) << left << fixed << setprecision(2) << price << " | "
                         << setw(8) << left << quantity << " | "
                         << setw(5) << left << fixed << setprecision(2) << total << "|\n";
                }
            }
            cout << setw(85) << setfill('-') << "" << setfill(' ') << "\n";
        }
    }

    static void _DeleteUser(const string& role) {
        system("cls");
        cout << "\n===========================================\n";
        cout << "\t   Delete " << role << " Screen";
        cout << "\n===========================================\n";

        string userId;
        cout << "Enter " << role << " ID to delete (or 0 to cancel): ";
        userId = clsInputValidate::ReadString();

        if (userId == "0") return;

        connectToDatabase();
        string query = "SELECT * FROM users WHERE id = '" + userId + "' AND role = '" + role + "' AND is_active = 1";
        string result = executeQuerySilent(query);

        // Check if result is empty or contains only header
        if (result.empty() || result.find("\n") == string::npos || result.find("\n") == result.length() - 1) {
            cout << "\n===========================================\n";
            cout << "\t   User Not Found";
            cout << "\n===========================================\n";
            cout << "\nNo active " << role << " found with ID: " << userId << "\n";
            cout << "Please check the ID and try again.\n";
            closeDatabaseConnection();
            cout << "\nPress any key to return to the main menu...";
            system("pause>0");
            return;
        }

        // Parse user data to show information
        stringstream ss(result);
        string line;
        getline(ss, line); // Skip header
        getline(ss, line);
        vector<string> fields;
        stringstream lineStream(line);
        string field;
        while (getline(lineStream, field, '\t')) {
            fields.push_back(field);
        }

        if (fields.size() >= 6) {
            cout << "\nUser Information:\n";
            cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";
            cout << "| " << setw(15) << left << "ID" << ": " << setw(30) << left << fields[0] << " |\n";
            cout << "| " << setw(15) << left << "Name" << ": " << setw(30) << left << (fields[1] + " " + fields[2]) << " |\n";
            cout << "| " << setw(15) << left << "Username" << ": " << setw(30) << left << fields[3] << " |\n";
            cout << "| " << setw(15) << left << "Email" << ": " << setw(30) << left << fields[4] << " |\n";
            cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";

            cout << "\nAre you sure you want to deactivate this " << role << "? (y/n): ";
            char confirm;
            cin >> confirm;

            if (confirm == 'y' || confirm == 'Y') {
                query = "UPDATE users SET is_active = 0, updated_at = CURRENT_TIMESTAMP WHERE id = '" + userId + "'";
                executeInstruction(query);
                cout << "\n===========================================\n";
                cout << "\t   Success";
                cout << "\n===========================================\n";
                cout << "\n" << role << " has been deactivated successfully!\n";
            } else {
                cout << "\n===========================================\n";
                cout << "\t   Cancelled";
                cout << "\n===========================================\n";
                cout << "\nOperation cancelled. No changes were made.\n";
            }
        }

        closeDatabaseConnection();
        cout << "\nPress any key to return to the main menu...";
        system("pause>0");
    }

    static void _ViewReport() {
        system("cls");
        cout << "\n===========================================\n";
        cout << "\t   System Report";
        cout << "\n===========================================\n";

        connectToDatabase();

        // Get total events
        string eventsQuery = "SELECT COUNT(*) as total_events, SUM(ticket_price) as total_revenue FROM events";
        string eventsResult = executeQuerySilent(eventsQuery);

        // Get total active attendees
        string attendeesQuery = "SELECT COUNT(DISTINCT attendee_id) as total_attendees FROM tickets t "
                              "JOIN users u ON t.attendee_id = u.id WHERE u.is_active = 1";
        string attendeesResult = executeQuerySilent(attendeesQuery);

        // Get total active organizers
        string organizersQuery = "SELECT COUNT(*) as total_organizers FROM users WHERE role = 'organizer' AND is_active = 1";
        string organizersResult = executeQuerySilent(organizersQuery);

        // Parse results
        stringstream eventsSS(eventsResult);
        string line;
        getline(eventsSS, line); // Skip header
        getline(eventsSS, line);
        vector<string> eventFields;
        stringstream eventLineStream(line);
        string field;
        while (getline(eventLineStream, field, '\t')) {
            eventFields.push_back(field);
        }

        stringstream attendeesSS(attendeesResult);
        getline(attendeesSS, line); // Skip header
        getline(attendeesSS, line);
        vector<string> attendeeFields;
        stringstream attendeeLineStream(line);
        while (getline(attendeeLineStream, field, '\t')) {
            attendeeFields.push_back(field);
        }

        stringstream organizersSS(organizersResult);
        getline(organizersSS, line); // Skip header
        getline(organizersSS, line);
        vector<string> organizerFields;
        stringstream organizerLineStream(line);
        while (getline(organizerLineStream, field, '\t')) {
            organizerFields.push_back(field);
        }

        if (eventFields.size() >= 2 && attendeeFields.size() >= 1 && organizerFields.size() >= 1) {
            int totalEvents = stoi(eventFields[0]);
            double totalRevenue = stod(eventFields[1]);
            int totalAttendees = stoi(attendeeFields[0]);
            int totalOrganizers = stoi(organizerFields[0]);

            cout << "\nEvent Statistics:\n";
            cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";
            cout << "| " << setw(30) << left << "Total Created Events" << ": " << setw(15) << right << totalEvents << " |\n";
            cout << "| " << setw(30) << left << "Total Revenue" << ": $" << setw(14) << right << fixed << setprecision(2) << totalRevenue << " |\n";
            cout << "| " << setw(30) << left << "Average Revenue per Event" << ": $" << setw(14) << right << fixed << setprecision(2) << (totalEvents > 0 ? totalRevenue / totalEvents : 0.0) << " |\n";
            cout << "| " << setw(30) << left << "Total Active Attendees" << ": " << setw(15) << right << totalAttendees << " |\n";
            cout << "| " << setw(30) << left << "Total Active Organizers" << ": " << setw(15) << right << totalOrganizers << " |\n";
            cout << "| " << setw(30) << left << "Average Attendees per Event" << ": " << setw(15) << right << fixed << setprecision(2) << (totalEvents > 0 ? (double)totalAttendees / totalEvents : 0.0) << " |\n";
            cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";
        }

        closeDatabaseConnection();
        cout << "\nPress any key to return to the main menu...";
        system("pause>0");
    }

    static enAdminMenuOptions _ReadAdminMenuOption() {
        int choice = clsInputValidate::ReadIntNumber();
        while (!clsInputValidate::IsNumberBetween(choice, 1, 5)) {
            cout << "Invalid choice, Enter number between 1 and 5: ";
            choice = clsInputValidate::ReadIntNumber();
        }
        return (enAdminMenuOptions)choice;
    }

    static void _PerformMenuOption(enAdminMenuOptions option) {
        switch (option) {
        case eSearchUser:
            _SearchUser();
            break;
        case eDeleteOrganizer:
            _DeleteUser("organizer");
            break;
        case eDeleteAttendee:
            _DeleteUser("attendee");
            break;
        case eViewReport:
            _ViewReport();
            break;
        case eLogout:
            CurrentUser = clsUser();
            break;
        }
    }

public:
    static void ShowAdminMenu() {
        bool running = true;
        while (running) {
            _DrawScreenHeader();
            _PrintMenuOptions();
            _PerformMenuOption(_ReadAdminMenuOption());
            if (CurrentUser.Id == 0) running = false;
        }
    }
};