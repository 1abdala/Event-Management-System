#ifndef CLCLOGIN_H
#define CLCLOGIN_H

#include <iostream>
#include <string>
#include "Database.h" // Your custom database implementation
#include "Globals.h"
#include "clsOrganizerMainMenuScreen.h"
#include "clsattendee.h"
#include "clsInputValidate.h"
#include "clsAdminMainMenuScreen.h"

using namespace std;

class clsLoginScreen {
public:
    clsLoginScreen() {
        // Constructor
    }

    ~clsLoginScreen() {
        // Destructor
    }

    bool displayScreen() {
        int choice;

        system("cls");
        cout << "\n\n";
        cout << setw(45) << "==========================================" << endl;
        cout << setw(45) << "|                                        |" << endl;
        cout << setw(45) << "| Welcome to The Event Management System |" << endl;
        cout << setw(45) << "|                                        |" << endl;
        cout << setw(45) << "==========================================" << endl;
        cout << "\n";
        system("pause");
        system("cls");

        // Login Menu
        cout << "\n\n";
        cout << setw(45) << "==============================" << endl;
        cout << setw(45) << "|         Login Menu         |" << endl;
        cout << setw(45) << "==============================" << endl;
        cout << setw(45) << "|  1. Log In                 |" << endl;
        cout << setw(45) << "|  2. Sign Up                |" << endl;
        cout << setw(45) << "|  3. Exit                   |" << endl;
        cout << setw(45) << "------------------------------" << endl;
        cout << "Enter your choice: ";
        choice = clsInputValidate::ReadIntNumber();

        while (!clsInputValidate::IsNumberBetween(choice, 1, 3)) {
            cout << "Invalid choice, Enter number 1 or 2 or 3: ";
            choice = clsInputValidate::ReadIntNumber();
        }

        switch (choice) {
        case 1:
            displayLoginScreen();
            return true;
        case 2:
            displaySignUpScreen();
            return true;
        case 3:
            return false;
        }

        return true;
    }

private:
    void displayLoginScreen() {
        string username, password;
        int attempts = 0;
        const int maxAttempts = 3;

        // Add role selection
        cout << "\nSelect Role:\n";
        cout << "[1] Attendee\n";
        cout << "[2] Organizer\n";
        cout << "[3] Admin\n";
        cout << "Enter your role (1-3): ";
        int roleChoice = clsInputValidate::ReadIntNumberBetween(1, 3, "Invalid choice. Please enter 1 for Attendee, 2 for Organizer, or 3 for Admin: ");

        string selectedRole;
        switch (roleChoice) {
            case 1:
                selectedRole = "attendee";
                break;
            case 2:
                selectedRole = "organizer";
                break;
            case 3:
                selectedRole = "admin";
                break;
        }

        while (attempts < maxAttempts) {
            cout << "\n--- Login as " << selectedRole << " --- (Attempt " << (attempts + 1) << " of " << maxAttempts << ")\n";
            cout << "Enter Username: ";
            cin >> username;
            cout << "Enter Password: ";
            password = clsInputValidate::ReadPassword();

            if (authenticateUser(username, password, selectedRole)) {
                if (CurrentUser.Role == "admin")
                    clsAdminMainMenuScreen::ShowAdminMenu();
                else if (CurrentUser.Role == "organizer")
                    clsOrganizerMainMenuScreen::ShowOrganizerMenu();
                else if (CurrentUser.Role == "attendee")
                    clsAttendeeMainMenuScreen::ShowAttendeeMenu();
                return;
            }
            else {
                cout << "Login Failed. Please check your credentials.\n";
                attempts++;
            }
        }

        cout << "\nToo many failed attempts. Returning to main menu.\n";
    }

    bool _IsValidPassword(const string& password) {
        if (password.length() < 8) {
            cout << "Password must be at least 8 characters long.\n";
            return false;
        }

        bool hasUpper = false;
        bool hasLower = false;
        bool hasDigit = false;
        bool hasSpecial = false;
        int typeCount = 0;

        for (char c : password) {
            if (isupper(c)) hasUpper = true;
            else if (islower(c)) hasLower = true;
            else if (isdigit(c)) hasDigit = true;
            else if (ispunct(c)) hasSpecial = true;
        }

        if (hasUpper) typeCount++;
        if (hasLower) typeCount++;
        if (hasDigit) typeCount++;
        if (hasSpecial) typeCount++;

        if (typeCount < 3) {
            cout << "Password must include at least 3 of the following:\n";
            cout << "- Uppercase letter (A-Z)\n";
            cout << "- Lowercase letter (a-z)\n";
            cout << "- Number (0-9)\n";
            cout << "- Special character (!@#$%^&* etc.)\n";
            return false;
        }

        return true;
    }

    void displaySignUpScreen() {
        string first_name, last_name, username, email, password, role;
        cout << "\n--- Sign Up ---\n";
        cout << "Enter First Name: ";
        cin >> first_name;
        cout << "Enter Last Name: ";
        cin >> last_name;
        cout << "Enter Username: ";
        cin >> username;
        cout << "Enter Email: ";
        cin >> email;
        
        // Password validation loop
        do {
            cout << "Enter Password: ";
            password = clsInputValidate::ReadPassword();
            if (_IsValidPassword(password)) {
                break;
            }
            cout << "\nPassword Requirements:\n";
            cout << "- Minimum 8 characters\n";
            cout << "- Must include at least 3 of:\n";
            cout << "  * Uppercase letter (A-Z)\n";
            cout << "  * Lowercase letter (a-z)\n";
            cout << "  * Number (0-9)\n";
            cout << "  * Special character (!@#$%^&* etc.)\n\n";
        } while (!_IsValidPassword(password));
        
        int roleChoice;
        cout << "Enter Role (1 for Organizer, 2 for Attendee): ";
        roleChoice = clsInputValidate::ReadIntNumberBetween(1, 2, "Invalid choice. Please enter 1 for Organizer or 2 for Attendee: ");

        if (roleChoice == 1) {
            role = "organizer";
        } else {
            role = "attendee";
        }

        if (registerUser(first_name, last_name, username, email, password, role)) {
            cout << "\n\nSign Up Successful! You can now log in." << endl;
            cout << "\nPress any key to return to the main menu...";
            system("pause>0");
        }
        else {
            cout << "Sign Up Failed. Please try again." << endl;
            cout << "\nPress any key to return to the main menu...";
            system("pause>0");
        }
    }

    bool authenticateUser(const string& username, const string& password, const string& role) {
        string safeUsername = escapeString(username);
        string safePassword = escapeString(password);

        string query = "SELECT * FROM users WHERE username = '" + safeUsername + "' AND password = '" + safePassword + "' AND role = '" + role + "' AND is_active = 1";
        cout << "[DEBUG] SQL Query: " << query << endl;

        connectToDatabase();
        string result = executeQuerySilent(query); // Should return header + data rows, tab-separated
        closeDatabaseConnection();

        if (!result.empty()) {
            vector<string> lines;
            string line;
            stringstream resultStream(result);

            // Split into lines (header + data)
            while (getline(resultStream, line)) {
                lines.push_back(line);
            }

            if (lines.size() < 2) {
                cout << "[ERROR] No user data found or account is deactivated." << endl;
                return false;
            }

            // Parse the second line (the data row)
            vector<string> tokens;
            string token;
            stringstream dataStream(lines[1]);

            while (getline(dataStream, token, '\t')) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 9) {
                // Assign to CurrentUser
                CurrentUser.Id = stoi(tokens[0]);
                CurrentUser.FirstName = tokens[1];
                CurrentUser.LastName = tokens[2];
                CurrentUser.Username = tokens[3];
                CurrentUser.Email = tokens[4];
                CurrentUser.Password = tokens[5];
                CurrentUser.Role = tokens[6];
                CurrentUser.CreatedAt = tokens[7];
                CurrentUser.UpdatedAt = tokens[8];

                return true;
            }
            else {
                cout << "[ERROR] Failed to parse user data. Expected 9 fields, got " << tokens.size() << endl;
                return false;
            }
        }

        return false;
    }

    bool registerUser(const string& first_name, const string& last_name, const string& username, const string& email, const string& password, const string& role) {
        string query = "INSERT INTO users (first_name, last_name, username, email, password, role) VALUES ('" +
            escapeString(first_name) + "', '" + escapeString(last_name) + "', '" +
            escapeString(username) + "', '" + escapeString(email) + "', '" +
            escapeString(password) + "', '" + escapeString(role) + "')";

        connectToDatabase();
        executeInstruction(query); // Using the custom function from Database.h
        closeDatabaseConnection();

        return true; // Assume success for simplicity
    }

    string escapeString(const string& input) {
        string output;
        for (char c : input) {
            if (c == '\'') {
                output += "''"; // SQL escape for single quote
            }
            else {
                output += c;
            }
        }
        return output;
    }
};

#endif // CLCLOGIN_H
