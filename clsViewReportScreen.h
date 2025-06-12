#pragma once
#include <iostream>
#include <sstream>
#include "clsScreen.h"
#include "clsReport.h"
#include "clsInputValidate.h"
#include "Globals.h"

class clsViewReportScreen : protected clsScreen {
private:
    static void _DrawScreenHeader() {
        system("cls");
        cout << "\n==========================================================\n";
        cout << "\t\tEvent Reports Screen";
        cout << "\n==========================================================\n";
    }

    static void _ShowReportMenu() {
        cout << "\nReport Options:\n";
        cout << "[1] View Report for All Events\n";
        cout << "[2] View Report for a Specific Event\n";
        cout << "[3] Return to Main Menu\n";
    }

    static void _ListEvents(const vector<string>& events) {
        cout << "\nYour Events:\n";
        cout << "----------------------------------------\n";
        
        for (const string& event : events) {
            stringstream ss(event);
            string id, name;
            getline(ss, id, '\t');
            getline(ss, name);
            cout << "[" << id << "] " << name << endl;
        }
        
        cout << "----------------------------------------\n";
    }

public:
    static void Show() {
        while (true) {
            _DrawScreenHeader();
            _ShowReportMenu();

            int choice = clsInputValidate::ReadIntNumberBetween(1, 3, "\nEnter your choice [1-3]: ");

            switch (choice) {
                case 1: {
                    cout << "\nGenerating report for all events...\n\n";
                    clsReport::GenerateEventReport(CurrentUser.Username);
                    break;
                }
                case 2: {
                    vector<string> events = clsReport::GetOrganizerEvents(CurrentUser.Username);
                    
                    if (events.empty()) {
                        cout << "\nYou don't have any events yet.\n";
                        break;
                    }

                    _ListEvents(events);

                    cout << "\nEnter Event ID (or 0 to cancel): ";
                    int eventId = clsInputValidate::ReadIntNumber();
                    
                    if (eventId == 0) {
                        continue;
                    }

                    // Verify the event belongs to this organizer
                    bool validEvent = false;
                    for (const string& event : events) {
                        if (event.substr(0, event.find('\t')) == to_string(eventId)) {
                            validEvent = true;
                            break;
                        }
                    }

                    if (!validEvent) {
                        cout << "\nInvalid Event ID. Please try again.\n";
                        break;
                    }

                    cout << "\nGenerating report for event ID " << eventId << "...\n\n";
                    clsReport::GenerateEventReport(CurrentUser.Username, eventId);
                    break;
                }
                case 3:
                    return;
            }

            cout << "\nPress any key to continue...";
            system("pause>0");
        }
    }
}; 