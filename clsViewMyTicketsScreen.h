#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <utility>
#include "Database.h"
#include "Globals.h"
#include "clsScreen.h"
#include "clsViewTicketDetailsScreen.h"
#include "clsInputValidate.h"
#include "clsDate.h"

using namespace std;

class clsViewMyTicketsScreen : protected clsScreen {
private:
    static void _DrawScreenHeader() {
        system("cls");
        cout << "\n===========================================\n";
        cout << "\t   My Tickets Screen";
        cout << "\n===========================================\n";
    }

    static void _PrintTicketsTableHeader() {
        cout << "\n" << setw(125) << setfill('=') << "" << setfill(' ') << "\n";
        cout << "| " << setw(5) << left << "ID"
             << "| " << setw(13) << left << "Ticket #"
             << "| " << setw(25) << left << "Event Name"
             << "| " << setw(12) << left << "Date"
             << "| " << setw(10) << left << "Price"
             << "| " << setw(10) << left << "Status"
             << "| " << setw(25) << left << "Speaker(s)"
             << "|" << endl;
        cout << setw(125) << setfill('=') << "" << setfill(' ') << "\n";
    }

    static void _PrintTicketRow(const string& id, const string& ticketNumber, const string& name, 
                               const string& date, const string& price, const string& status, const string& speakers) {
        cout << "| " << setw(5) << left << id
             << "| " << setw(13) << left << ticketNumber
             << "| " << setw(25) << left << name
             << "| " << setw(12) << left << date
             << "| $" << setw(8) << left << price
             << "| " << setw(10) << left << status
             << "| " << setw(25) << left << speakers
             << "|" << endl;
        cout << setw(125) << setfill('-') << "" << setfill(' ') << "\n";
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

    static void _PrintTicketRow(const vector<string>& row) {
        // Expected row: [ticket_id, ticket_number, booking_time, event_name, event_date]
        cout << setw(10) << left << row[0];
        cout << "| " << setw(14) << left << row[1];
        cout << "| " << setw(20) << left << row[2];
        cout << "| " << setw(25) << left << row[3].substr(0, 24);
        cout << "| " << setw(12) << left << row[4];
        cout << endl;
    }

public:
    static void Show() {
        string subChoice = "0";
        do {
            _DrawScreenHeader();
            
            connectToDatabase();

            string currentDateStr = clsDate::DateToYYYYMMDDString(clsDate::GetSystemDate());

            string query = "SELECT t.id, t.ticket_number, e.name, e.date, e.ticket_price, "
                         "CASE WHEN e.date < '" + currentDateStr + "' THEN 'Finished' ELSE 'Upcoming' END AS status, "
                         "COALESCE(GROUP_CONCAT(s.full_name SEPARATOR ', '), 'None') as speakers "
                         "FROM tickets t "
                         "JOIN events e ON t.event_id = e.id "
                         "JOIN users u ON t.attendee_id = u.id "
                         "LEFT JOIN speakers s ON e.id = s.event_id "
                         "WHERE u.username = '" + CurrentUser.Username + "' "
                         "GROUP BY t.id, t.ticket_number, e.name, e.date, e.ticket_price, status "
                         "ORDER BY e.date DESC";

            string result = executeQuerySilent(query);

            // Skip header line
            size_t pos = result.find('\n');
            if (pos != string::npos) {
                result = result.substr(pos + 1);
            }

            if (!result.empty()) {
                _PrintTicketsTableHeader();

                stringstream ss(result);
                string line;
                vector<string> ticketIds;

                while (getline(ss, line)) {
                    if (line.empty()) continue;

                    vector<string> fields;
                    stringstream lineStream(line);
                    string field;
                    
                    while (getline(lineStream, field, '\t')) {
                        fields.push_back(field);
                    }

                    if (fields.size() >= 7) {
                        _PrintTicketRow(fields[0], fields[1], fields[2], fields[3], fields[4], fields[5], fields[6]);
                        ticketIds.push_back(fields[0]);
                    }
                }

                cout << "\nOptions:\n";
                cout << "[1] View Ticket Details\n";
                cout << "[2] Generate Report for Finished Attended Events\n";
                cout << "[0] Return to Main Menu\n";

                subChoice = to_string(clsInputValidate::ReadIntNumberBetween(0, 2, "\nChoose what do you want to do? [0-2] "));

                if (subChoice == "1") {
                    string ticketId;
                    do {
                        cout << "\nEnter Ticket ID (or 0 to cancel): ";
                        ticketId = clsInputValidate::ReadString();
                        
                        if (ticketId == "0") break;

                        bool validTicket = false;
                        for (const string& id : ticketIds) {
                            if (id == ticketId) {
                                validTicket = true;
                                break;
                            }
                        }

                        if (!validTicket) {
                            cout << "\nInvalid Ticket ID. Please try again.\n";
                            ticketId = "";
                        }
                    } while (ticketId.empty());

                    if (ticketId != "0") {
                        clsViewTicketDetailsScreen::ShowTicketDetails(ticketId);
                    }
                }
                else if (subChoice == "2") {
                    _DrawScreenHeader();
                    cout << "\n--- Report: Finished Attended Events ---\n\n";
                    
                    string reportQuery = "SELECT e.name, e.date, e.ticket_price "
                                         "FROM tickets t "
                                         "JOIN events e ON t.event_id = e.id "
                                         "JOIN users u ON t.attendee_id = u.id "
                                         "WHERE u.username = '" + CurrentUser.Username + "' AND e.date < '" + currentDateStr + "' "
                                         "ORDER BY e.date DESC, e.name ASC";
                    
                    string reportResult = executeQuerySilent(reportQuery);

                    size_t reportPos = reportResult.find('\n');
                    if (reportPos != string::npos) {
                        reportResult = reportResult.substr(reportPos + 1);
                    }

                    double totalCost = 0.0;
                    int totalTickets = 0;

                    if (!reportResult.empty()) {
                        cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";
                        cout << "| " << setw(30) << left << "Event Name" 
                             << "| " << setw(15) << left << "Date" << " |\n";
                        cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";

                        stringstream reportSS(reportResult);
                        string reportLine;
                        while (getline(reportSS, reportLine)) {
                            if (reportLine.empty()) continue;

                            vector<string> reportFields = _SplitLine(reportLine);
                            
                            if (reportFields.size() >= 3) {
                                string eventName = reportFields[0];
                                string eventDate = reportFields[1];
                                double ticketPrice = 0.0;
                                try {
                                    ticketPrice = stod(reportFields[2]);
                                } catch (const std::invalid_argument& ia) {
                                    cerr << "Invalid argument: " << ia.what() << endl;
                                } catch (const std::out_of_range& oor) {
                                    cerr << "Out of Range error: " << oor.what() << endl;
                                }

                                totalCost += ticketPrice;
                                totalTickets++;

                                cout << "| " << setw(30) << left << eventName
                                        << "| " << setw(15) << left << eventDate.substr(0, 10)
                                        << " |\n";
                            }
                        }
                        cout << setw(50) << setfill('-') << "" << setfill(' ') << "\n";

                        cout << "\n--- Summary ---\n";
                        cout << "Total Finished Events Attended: " << totalTickets << endl;
                        cout << fixed << setprecision(2);
                        cout << "Total Cost of Tickets: $" << totalCost << endl;
                        if (totalTickets > 0) {
                            cout << "Average Price per Ticket: $" << (totalCost / totalTickets) << endl;
                        } else {
                            cout << "Average Price per Ticket: N/A" << endl;
                        }

                    } else {
                        cout << "\nNo finished events found that you attended.\n";
                    }

                    cout << "\nPress any key to return to the tickets screen...";
                    system("pause>0");
                }
            }
            else {
                cout << "\nYou don't have any tickets yet.\n";
                cout << "\nPress any key to return to main menu...";
                system("pause>0");
                subChoice = "0";
            }

            closeDatabaseConnection();

        } while (subChoice != "0");
    }
};
