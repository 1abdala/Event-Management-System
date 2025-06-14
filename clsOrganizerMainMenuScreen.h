#pragma once
#include <iostream>
#include <iomanip>
#include "clsInputValidate.h"
#include "Globals.h"
#include "clsCreateEventScreen.h"
#include "clsViewCreatedEventsScreen.h"
#include "clsUpdateEventScreen.h"
#include "clsDeleteEventScreen.h"
#include "clsSpeakerScreen.h"
#include "clsViewEventAttendeesScreen.h"
#include "clsLoginScreen.h"
#include "clsViewReportScreen.h"
#include "clseSearchAttendee.h"

class clsOrganizerMainMenuScreen {
private:
    enum enOrganizerMenuOption {
        eCreateEvent = 1, eViewEvents, eUpdateEvent,
        eDeleteEvent, eAddSpeaker, eViewAttendees,
        eViewReport, eSearchAttendee, eLogout
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

    static short _ReadOrganizerMenuOption() {
        cout << "\nEnter your choice [1 - 9]: ";
        return clsInputValidate::ReadShortNumberBetween(1, 9, "Invalid choice. Please enter a number between 1 and 9: ");
    }

    static void _GoBackToMenu() {
        cout << "\nPress any key to return to the main menu...";
        system("pause>0");
        ShowOrganizerMenu();
    }

    static void _PerformMenuOption(enOrganizerMenuOption Option) {
        switch (Option) {
        case eCreateEvent:
            clsCreateEventScreen::Show();
            _GoBackToMenu();
            break;

        case eViewEvents:
            clsViewCreatedEventsScreen::ShowCreatedEvents();
            _GoBackToMenu();
            break;

        case eUpdateEvent:
            clsUpdateEventScreen::Show();
            _GoBackToMenu();
            break;

        case eDeleteEvent:
            clsDeleteEventScreen::Show();
            _GoBackToMenu();
            break;

        case eAddSpeaker:
            clsSpeakerScreen::ShowSpeakerMenu();
            _GoBackToMenu();
            break;

        case eViewAttendees:
            _DrawScreenHeader("View Event Attendees");
            clsViewEventAttendeesScreen::Show();
            _GoBackToMenu();
            break;

        case eViewReport:
            clsViewReportScreen::Show();
            _GoBackToMenu();
            break;

        case eSearchAttendee:
            clseSearchAttendee::Show();
            _GoBackToMenu();
            break;

        case eLogout:
            break;
        }
    }

public:
    static void ShowOrganizerMenu() {
        _DrawScreenHeader("Organizer Dashboard");

        cout << "======================= Organizer Menu =========================\n";
        cout << "[1]  Create New Event\n";
        cout << "[2]  View My Events\n";
        cout << "[3]  Update Event Info\n";
        cout << "[4]  Delete Event\n";
        cout << "[5]  Speaker Screen\n";
        cout << "[6]  View Event Attendees\n";
        cout << "[7]  View Events Report\n";
        cout << "[8]  Search attendee\n";
        cout << "[9]  Logout\n";
        cout << "================================================================\n";

        _PerformMenuOption((enOrganizerMenuOption)_ReadOrganizerMenuOption());
    }
};
