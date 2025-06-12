#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include "Database.h"
#include "Globals.h"
#include "clsDate.h"
#include "clsString.h"
#include "clsInputValidate.h"

class clsReport {
private:
    static void _PrintTableHeader() {
        // Define content widths for each column
        const int W_ID = 6;
        const int W_NAME = 23;
        const int W_DATE = 13;
        const int W_ATTENDEES = 10;
        const int W_PRICE = 10;
        const int W_REVENUE = 13;

        cout << "\n" << setw(100) << setfill('=') << "" << setfill(' ') << "\n";
        cout << "| " << setw(W_ID)        << left  << "ID"
             << " | " << setw(W_NAME)      << left  << "Event Name"
             << " | " << setw(W_DATE)      << left  << "Date"
             << " | " << setw(W_ATTENDEES) << right << "Attendees"
             << " | " << setw(W_PRICE)     << right << "Price"      // Price column header
             << " | " << setw(W_REVENUE)   << right << "Revenue"    // Revenue column header
             << " |" << endl;
        cout << setw(100) << setfill('=') << "" << setfill(' ') << "\n";
    }

    static void _PrintEventRow(const string& id, const string& name, const string& date, 
                             int attendees, double price, double revenue) {
        // Use the same content widths as in the header
        const int W_ID = 6;
        const int W_NAME = 23;
        const int W_DATE = 13; // Date string like YYYY-MM-DD is 10 chars, 13 provides padding
        const int W_ATTENDEES = 10;
        const int W_PRICE = 10;     // For $XXXX.XX (e.g., $1234.56 is 8 chars, leaves space)
        const int W_REVENUE = 13;   // For $XXXXXX.XX (e.g., $123456.78 is 10 chars, leaves space)

        cout << "| " << setw(W_ID)        << left  << id.substr(0, W_ID)
             << " | " << setw(W_NAME)      << left  << name.substr(0, W_NAME)
             << " | " << setw(W_DATE)      << left  << date.substr(0, W_DATE) 
             << " | " << setw(W_ATTENDEES) << right << attendees
             << " | " << "$" << setw(W_PRICE - 1) << right << fixed << setprecision(2) << price // Subtract 1 from setw for '$'
             << " | " << "$" << setw(W_REVENUE - 1) << right << fixed << setprecision(2) << revenue // Subtract 1 from setw for '$'
             << " |" << endl;
        cout << setw(100) << setfill('-') << "" << setfill(' ') << "\n";
    }

public:
    static vector<string> GetOrganizerEvents(const string& organizerUsername) {
        vector<string> events;
        connectToDatabase();

        string query = "SELECT id, name FROM events "
                      "WHERE organizer_id = (SELECT id FROM users WHERE username = '" + organizerUsername + "') "
                      "ORDER BY date DESC";

        string result = executeQuerySilent(query);

        // Skip header line
        size_t pos = result.find('\n');
        if (pos != string::npos) {
            result = result.substr(pos + 1);
        }

        stringstream ss(result);
        string line;
        while (getline(ss, line)) {
            if (!line.empty()) {
                events.push_back(line);
            }
        }

        closeDatabaseConnection();
        return events;
    }

    static void GenerateEventReport(const string& organizerUsername, int specificEventId = -1) {
        connectToDatabase();

        string query = "SELECT e.id, e.name, e.date, "
                      "COUNT(t.id) as attendee_count, "
                      "e.ticket_price, "
                      "(COUNT(t.id) * e.ticket_price) as total_revenue "
                      "FROM events e "
                      "LEFT JOIN tickets t ON e.id = t.event_id "
                      "WHERE e.organizer_id = (SELECT id FROM users WHERE username = '" + organizerUsername + "') ";

        if (specificEventId != -1) {
            query += "AND e.id = " + to_string(specificEventId) + " ";
        }

        query += "GROUP BY e.id, e.name, e.date, e.ticket_price "
                "ORDER BY e.date DESC";

        string result = executeQuerySilent(query);

        // Skip header line
        size_t pos = result.find('\n');
        if (pos != string::npos) {
            result = result.substr(pos + 1);
        }

        double totalRevenue = 0;
        int totalAttendees = 0;
        int totalEvents = 0;

        _PrintTableHeader();

        // Parse and display results
        stringstream ss(result);
        string line;
        while (getline(ss, line)) {
            if (line.empty()) continue;

            vector<string> fields;
            stringstream lineStream(line);
            string field;
            while (getline(lineStream, field, '\t')) {
                fields.push_back(field);
            }

            if (fields.size() >= 6) {
                string eventId = fields[0];
                string eventName = fields[1];
                string eventDate = fields[2];
                int attendeeCount = stoi(fields[3]);
                double ticketPrice = stod(fields[4]);
                double revenue = stod(fields[5]);

                _PrintEventRow(eventId, eventName, eventDate, attendeeCount, ticketPrice, revenue);

                totalAttendees += attendeeCount;
                totalRevenue += revenue;
                totalEvents++;
            }
        }

        // Calculate averages
        double avgRevenuePerEvent = (totalEvents > 0) ? (totalRevenue / totalEvents) : 0;
        double avgAttendeesPerEvent = (totalEvents > 0) ? ((double)totalAttendees / totalEvents) : 0;

        // Print totals
        cout << "\nSummary:\n";
        cout << "----------------------------------------\n";
        if (specificEventId == -1) {
            cout << "Total Events: " << totalEvents << endl;
        }
        cout << "Total Attendees: " << totalAttendees << endl;
        cout << "Total Revenue: $" << fixed << setprecision(2) << totalRevenue << endl;
        if (specificEventId == -1) {
            cout << "Average Revenue per Event: $" << fixed << setprecision(2) << avgRevenuePerEvent << endl;
            cout << "Average Attendees per Event: " << fixed << setprecision(1) << avgAttendeesPerEvent << endl;
        }
        cout << "----------------------------------------\n";

        closeDatabaseConnection();
    }
}; 