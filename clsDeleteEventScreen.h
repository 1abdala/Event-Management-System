#pragma once
#include <iostream>
#include <string>
#include "Database.h"
#include "Globals.h"

using namespace std;

class clsDeleteEventScreen {
public:
    static void Show() {
        system("cls");
        cout << "========================================\n";
        cout << "           Delete My Event              \n";
        cout << "========================================\n\n";

        string eventID;
        cout << "Enter Event ID to delete: ";
        cin.ignore();
        getline(cin, eventID);

        // Step 1: Verify ownership
        string checkQuery = "SELECT * FROM Events WHERE id = '" + eventID + "' AND organizer_id = '" + to_string(CurrentUser.Id) + "'";
        connectToDatabase();
        string result = executeQuerySilent(checkQuery);
        closeDatabaseConnection();

        if (result.empty() || result.find('\n') == string::npos) {
            cout << "\nEvent not found or you don't have permission to delete it.\n";
            
            return;
        }

        // Step 2: Confirm deletion
        string confirm;
        cout << "\nAre you sure you want to delete this event? (Y/N): ";
        getline(cin, confirm);

        if (confirm != "Y" && confirm != "y") {
            cout << "\nDeletion cancelled.\n";
          
        }

        // Step 3: Delete
        string deleteQuery = "DELETE FROM Events WHERE id = '" + escapeString(eventID) + "' AND organizer_id = '" + to_string(CurrentUser.Id) + "'";
        connectToDatabase();
        executeInstruction(deleteQuery);
        closeDatabaseConnection();

        cout << "\nEvent deleted successfully.\n";
        
    }

private:
    static string escapeString(const string& input) {
        string result = input;
        size_t pos = 0;
        while ((pos = result.find('\'')) != string::npos) {
            result.insert(pos, 1, '\'');
            pos += 2;
        }
        return result;
    }
};
