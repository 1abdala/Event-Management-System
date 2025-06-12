#pragma once
#include <iostream>
#include <iomanip>
#include "clsInputValidate.h"
#include "Globals.h"
#include "clsAddSpeaker.h"
#include "clsViewAddedSpeakers.h"
#include "clsViewAddedSpeakers.h"
#include "clsDeleteSpeakerScreen.h"
#include "clsUpdatSpeakerScreen.h"


class clsSpeakerScreen {
private:

    enum enSpeakerMenuOption {
        eAddSpeaker = 1,eViewSpeakers, eUpdatSpeakerInto
        ,eDeleteSpeaker,eLogout
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

    static short _ReadSpeakerMenuOption() {
        cout << "\nEnter your choice [1 - 4]: ";
        return clsInputValidate::ReadShortNumberBetween(1, 5, "Invalid choice. Please enter a number between 1 and 5: ");
    }

    static void _GoBackToMenu() {
        cout << "\nPress any key to return to the main menu...";
        system("pause>0");
        ShowSpeakerMenu();
    }

    static void _PerformMenuOption(enSpeakerMenuOption Option) {
        switch (Option) {
        case eAddSpeaker:
           
            clsAddSpeakerScreen::Show();
            _GoBackToMenu();
            break;

        case eViewSpeakers:
            clsViewAddedSpeakersScreen::ShowAddedSpeakers();
            _GoBackToMenu();
            break;

        case eUpdatSpeakerInto:
            clsUpdateSpeakerScreen::Show();
            _GoBackToMenu();
            break;

        case eDeleteSpeaker:
            clsDeleteSpeakerScreen::ShowDeleteSpeakerScreen();
            _GoBackToMenu();
            break;

        case eLogout:
            
            break;
        }
    }

public:
    static void ShowSpeakerMenu() {
        _DrawScreenHeader("Speaker Dashboard");

        cout << "======================= Speaker Menu =========================\n";
        cout << "[1]  Add New Speaker\n";
        cout << "[2]  View My Speakers\n";
        cout << "[3]  Update Speaker Info\n";
        cout << "[4]  Delete Speaker\n";
        cout << "[5]  Logout\n";
        cout << "================================================================\n";

        _PerformMenuOption((enSpeakerMenuOption)_ReadSpeakerMenuOption());
    }
};

