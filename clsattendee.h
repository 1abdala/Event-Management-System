#pragma once
#include <iostream>
#include <iomanip>
#include "clsInputValidate.h"
#include "Globals.h"
#include "clsAvailableEventsScreen.h"
#include "clsViewMyTicketsScreen.h"
#include "clsCancelTicketBookingScreen.h"

class clsAttendeeMainMenuScreen {
private:
    enum enAttendeeMenuOption {
        eViewEvents = 1, eViewMyTickets, eCancelBooking,
        eLogout
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
        cout << "\nEnter your choice [1 - 4]: ";
        return clsInputValidate::ReadShortNumberBetween(1, 4, "Invalid choice. Please enter a number between 1 and 4: ");
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
            
        case eViewMyTickets:
            clsViewMyTicketsScreen::Show();
            _GoBackToMenu();
            break;

        case eCancelBooking:
            clsCancelTicketBookingScreen::Show();
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
        cout << "[2]  View My Booked Tickets\n";
        cout << "[3]  Cancel Ticket Booking\n";
        cout << "[4]  Logout\n";
        cout << "================================================================\n";

        _PerformMenuOption((enAttendeeMenuOption)_ReadAttendeeMenuOption());
    }
};
