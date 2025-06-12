#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include "clsScreen.h"
#include "Database.h"
#include "Globals.h"
#include "clsDate.h"

class clsViewTicketDetailsScreen : protected clsScreen {
private:
    static void _DrawScreenHeader() {
        system("cls");
        cout << "\n===========================================\n";
        cout << "\t   Ticket Details Screen";
        cout << "\n===========================================\n";
    }

    static string _FormatDateString(const string& dbDate) {
        // Convert from YYYY-MM-DD to DD/MM/YYYY
        if (dbDate.length() < 10) return "";
        string day = dbDate.substr(8, 2);
        string month = dbDate.substr(5, 2);
        string year = dbDate.substr(0, 4);
        return day + "/" + month + "/" + year;
    }

    static pair<string, int> _GetTimeRemaining(const string& eventDate) {
        string formattedDate = _FormatDateString(eventDate);
        if (formattedDate.empty()) return make_pair("Invalid Date", 0);
        
        clsDate currentDate = clsDate::GetSystemDate();
        clsDate ticketDate(formattedDate);
        
        int daysDiff = clsDate::GetDifferenceInDays(currentDate, ticketDate);
        
        if (daysDiff == 0) {
            return make_pair("Today", 0);
        }
        else if (daysDiff > 0) {
            return make_pair("Time remaining", daysDiff);
        }
        else {
            return make_pair("Time passed", -daysDiff);
        }
    }

    static string _GetTicketStatus(const string& eventDate) {
        clsDate currentDate = clsDate::GetSystemDate();
        string formattedDate = _FormatDateString(eventDate);
        if (formattedDate.empty()) return "Invalid Date";
        
        clsDate ticketDate(formattedDate);

        if (clsDate::IsDate1BeforeDate2(currentDate, ticketDate))
            return "Upcoming";
        else if (clsDate::IsDate1EqualDate2(currentDate, ticketDate))
            return "Today";
        else
            return "Expired";
    }

    static string _FormatTimeRemaining(const pair<string, int>& timeInfo) {
        if (timeInfo.first == "Invalid Date") return "Invalid Date";
        if (timeInfo.first == "Today") return "Event is today!";
        
        int days = timeInfo.second;
        int hours = days * 24;
        
        stringstream ss;
        if (timeInfo.first == "Time remaining") {
            ss << "Time remaining: ";
        } else {
            ss << "Time passed: ";
        }
        
        if (days > 0) {
            ss << days << " day" << (days > 1 ? "s" : "") << " ";
            ss << "(" << hours << " hours)";
        } else {
            ss << hours << " hours";
        }
        
        return ss.str();
    }

    static void _PrintTicketDetails(const string& ticketId, const string& eventName, 
                                  const string& eventDate, double ticketPrice,
                                  const string& ticketNumber, const string& bookingTime,
                                  const string& speakerName, const string& speakerTopic) {
        string status = _GetTicketStatus(eventDate);
        string formattedDate = _FormatDateString(eventDate);
        pair<string, int> timeInfo = _GetTimeRemaining(eventDate);
        string timeRemaining = _FormatTimeRemaining(timeInfo);
        
        cout << "\n-------------------------------------------\n";
        cout << "Ticket Information:\n";
        cout << "-------------------------------------------\n";
        cout << "Ticket ID: " << ticketId << endl;
        cout << "Ticket Number: " << (ticketNumber.empty() ? "Not Assigned" : ticketNumber) << endl;
        cout << "Event Name: " << eventName << endl;
        cout << "Event Date: " << formattedDate << endl;
        cout << "Ticket Price: $" << fixed << setprecision(2) << ticketPrice << endl;
        cout << "Booking Time: " << bookingTime << endl;
        cout << "Status: " << status << endl;
        cout << timeRemaining << endl;
        cout << "Speaker Name: " << speakerName << endl;
        cout << "Speaker Topic: " << speakerTopic << endl;
        cout << "-------------------------------------------\n";
    }

public:
    static void ShowTicketDetails(const string& ticketId) {
        _DrawScreenHeader();

        connectToDatabase();

        string query = "SELECT t.id, e.name, e.date, e.ticket_price, "
                      "t.ticket_number, t.booking_time, "
                      "COALESCE(s.full_name, 'None') as speaker_name, "
                      "COALESCE(s.topic, 'N/A') as speaker_topic "
                      "FROM tickets t "
                      "JOIN events e ON t.event_id = e.id "
                      "LEFT JOIN speakers s ON e.id = s.event_id "
                      "WHERE t.id = '" + ticketId + "' "
                      "AND t.attendee_id = (SELECT id FROM users WHERE username = '" + CurrentUser.Username + "')";

        string result = executeQuerySilent(query);

        // Skip header line
        size_t pos = result.find('\n');
        if (pos != string::npos) {
            result = result.substr(pos + 1);
        }

        if (!result.empty()) {
            stringstream ss(result);
            string line;
            getline(ss, line);

            vector<string> fields;
            stringstream lineStream(line);
            string field;
            
            while (getline(lineStream, field, '\t')) {
                fields.push_back(field);
            }

            if (fields.size() >= 8) {
                _PrintTicketDetails(
                    fields[0],  // ticket id
                    fields[1],  // event name
                    fields[2],  // event date
                    stod(fields[3]),  // ticket price
                    fields[4],  // ticket number
                    fields[5],   // booking time
                    fields[6],  // speaker name
                    fields[7]   // speaker topic
                );
            }
        }
        else {
            cout << "\nTicket not found or you don't have access to view this ticket.\n";
        }

        closeDatabaseConnection();

        cout << "\nPress any key to return to main menu...";
        system("pause>0");
    }
}; 