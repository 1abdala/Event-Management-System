
#pragma once
#include <iostream>
#include "clsInputValidate.h"
#include <string>
#include <iomanip>
#include "Globals.h"
class clsattendee
{

    enum enUsersMenueOption {
        eListUsers = 1, eAddNewUser, eDeleteUser,
        eUpdateUser, eFindUser, eExit

    };

    static void _DrawScreenHeader(string Title, string SubTitle = "") {

        clsDate date = clsDate::GetSystemDate();
        cout << "\t\t\t\t\t______________________________________";
        cout << "\n\n\t\t\t\t\t  " << Title;
        if (SubTitle != "")
        {
            cout << "\n\t\t\t\t\t  " << SubTitle;
        }
        cout << "\n\t\t\t\t\t______________________________________\n\n";
        cout << "\t\t\t\t\tuser: " << CurrentUser.Username << endl;
        cout << "\t\t\t\t\tdate: " << clsDate::DateToString(clsDate());
        cout << "\n\n";
    }


    static short _ReadManageUsersMenueOption() {
        cout << setw(37) << left << "" << "choose what do you want to do from [1] to [6] : ";
        short choise = clsInputValidate::ReadShortNumberBetween(1, 6, "Invalid Number,enter number between 1 and 6 ");
        return choise;

    }


    static void  _GoBackToManageUsersMenue() {

        cout << setw(37) << "" << "press any key to get back to main menu";
        system("pause>0");
        ShowManageattendeeMenue();


    }

    static void _ShowAllUsersScreen() {
        cout << "will do";

        //clsListUsersScreen::ShowUsersList();
    }

    static void _ShowAddNewUserScreen() {
        cout << "will do";

        //clsAddNewUserScreen::ShowAddNewUserScreen();
    }
    static void _ShowDeleteUserScreen() {
        cout << "will do";

        //clsDeleteUserScreen::ShowDeletUserScreen();
    }
    static void _ShowUpdateUserScreen() {
        cout << "will do";

        //clsUpdateUserScreen::ShowUpdateUserScreen();
    }
    static void _ShowFindCUserScreen() {
        cout << "will do";

        //clsFindCUserScreen::ShowFindUserScreen();
    }

    static void _PerfromManageUsersMenueOption(enUsersMenueOption MainMenueOption)
    {
        switch (MainMenueOption)
        {
        case enUsersMenueOption::eListUsers:
        {
            system("cls");
            _ShowAllUsersScreen();
            _GoBackToManageUsersMenue();
            break;
        }
        case enUsersMenueOption::eAddNewUser:
            system("cls");
            _ShowAddNewUserScreen();
            _GoBackToManageUsersMenue();
            break;

        case enUsersMenueOption::eDeleteUser:
            system("cls");
            _ShowDeleteUserScreen();
            _GoBackToManageUsersMenue();
            break;

        case enUsersMenueOption::eUpdateUser:
            system("cls");
            _ShowUpdateUserScreen();
            _GoBackToManageUsersMenue();
            break;

        case enUsersMenueOption::eFindUser:
            system("cls");
            _ShowFindCUserScreen();
            _GoBackToManageUsersMenue();
            break;

        case enUsersMenueOption::eExit:
            system("cls");

            break;

        }

    }

public:
    static void ShowManageattendeeMenue() {

        /*if (!CheckAccessRights(clsUser::enPermissions::pManageUsers)) {

            return;

        }*/
        system("cls");
        _DrawScreenHeader("\tOrgnizer Screen");

        cout << setw(37) << left << "" << "===========================================\n";
        cout << setw(37) << left << "" << "\t\t   attendee Menue\n";
        cout << setw(37) << left << "" << "===========================================\n";
        cout << setw(37) << left << "" << "\t[1] Show user List.\n";
        cout << setw(37) << left << "" << "\t[2] Add New user.\n";
        cout << setw(37) << left << "" << "\t[3] Delete user.\n";
        cout << setw(37) << left << "" << "\t[4] Update user Info.\n";
        cout << setw(37) << left << "" << "\t[5] Find user.\n";
        cout << setw(37) << left << "" << "\t[6] Logout.\n";
        cout << setw(37) << left << "" << "===========================================\n";

        _PerfromManageUsersMenueOption((enUsersMenueOption)_ReadManageUsersMenueOption());



    }





};


