#pragma once
#include <iostream>
#include <iomanip>
#include "clsInputValidate.h"
#include "Globals.h"
#include "clsAvailableEventsScreen.h"
#include "clsBookTicketScreen.h"


class clsAttendeeMainMenuScreen {
private:

    enum enAttendeeMenuOption {
        eViewEvents = 1, eBookTicket, eViewMyTickets,
        eCheckIn, eViewAnnouncements, eLogout
    };

    static void _DrawScreenHeader(const string& Title, const string& SubTitle = "") {
        system("cls");
        cout << "\n===============================================================\n";
        cout << setw(45) << right << Title << "\n";
        if (!SubTitle.empty())
            cout << setw(45) << right << SubTitle << "\n";
        cout << "===============================================================\n";
        cout << "User: " << CurrentUser.Username << "\n";
        cout << "Date: " << clsDate::DateToString(clsDate::GetSystemDate()) << "\n";
        cout << "---------------------------------------------------------------\n\n";
    }

    static short _ReadAttendeeMenuOption() {
        cout << "\nEnter your choice [1 - 6]: ";
        return clsInputValidate::ReadShortNumberBetween(1, 6, "Invalid choice. Please enter a number between 1 and 6: ");
    }

    static void _GoBackToMenu() {
        cout << "\nPress any key to return to the main menu...";
        system("pause>0");
        ShowAttendeeMenu();
    }

    static void _PerformMenuOption(enAttendeeMenuOption Option) {
        switch (Option) {
        case eViewEvents:
            clsAvailableEventsScreen::Show();
            _GoBackToMenu();
            break;
            
        case eBookTicket:
            clsBookTicketScreen::Show();
            _GoBackToMenu();
            break;

        case eViewMyTickets:
            _DrawScreenHeader("My Booked Tickets");
            cout << "\n[View My Tickets functionality will be here]\n";
            _GoBackToMenu();

            break;
        case eCheckIn:
            _DrawScreenHeader("Check-in to Event");
            cout << "\n[Check-in functionality will be here]\n";
            _GoBackToMenu();
            break;

        case eViewAnnouncements:
            _DrawScreenHeader("Event Announcements");
            cout << "\n[View Announcements functionality will be here]\n";
            _GoBackToMenu();
            break;

        case eLogout:
            _DrawScreenHeader("Logging Out...");
            break;
        }
    }

public:
    static void ShowAttendeeMenu() {
        _DrawScreenHeader("Attendee Dashboard");

        cout << "======================= Attendee Menu ==========================\n";
        cout << "[1]  View Available Events\n";
        cout << "[2]  Book a Ticket\n";
        cout << "[3]  View My Booked Tickets\n";
        cout << "[4]  Check-in to Event\n";
        cout << "[5]  View Event Announcements\n";
        cout << "[6]  Logout\n";
        cout << "================================================================\n";

        _PerformMenuOption((enAttendeeMenuOption)_ReadAttendeeMenuOption());
    }
};
