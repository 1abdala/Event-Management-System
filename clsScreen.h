#pragma once
#include <iostream>
#include <string>
#include "clsUser.h"
#include "clsDate.h"
#include "Globals.h"
using namespace std;

class clsScreen
{
public:
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
        cout << "\t\t\t\t\tdate: " << clsDate::DateToString(date);
        cout << "\n\n";
    }
};
