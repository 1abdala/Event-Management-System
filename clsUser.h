#pragma once
#include <string>
#include <iostream>

using namespace std;

class clsUser {
public:
    int Id;
    string FirstName;
    string LastName;
    string Username;
    string Email;
    string Password;
    string Role;
    string CreatedAt;
    string UpdatedAt;

    clsUser() {
        Id = 0;
        FirstName = "";
        LastName = "";
        Username = "";
        Email = "";
        Password = "";
        Role = "";
        CreatedAt = "";
        UpdatedAt = "";
    }

    string FullName() {
        return FirstName + " " + LastName;
    }

    // Setters and Getters with __declspec(property)

    void SetId(int Id) { this->Id = Id; }
    int GetId() { return this->Id; }
    __declspec(property(get = GetId, put = SetId)) int Id;

    void SetFirstName(string FirstName) { this->FirstName = FirstName; }
    string GetFirstName() { return this->FirstName; }
    __declspec(property(get = GetFirstName, put = SetFirstName)) string FirstName;

    void SetLastName(string LastName) { this->LastName = LastName; }
    string GetLastName() { return this->LastName; }
    __declspec(property(get = GetLastName, put = SetLastName)) string LastName;

    void SetUsername(string Username) { this->Username = Username; }
    string GetUsername() { return this->Username; }
    __declspec(property(get = GetUsername, put = SetUsername)) string Username;

    void SetEmail(string Email) { this->Email = Email; }
    string GetEmail() { return this->Email; }
    __declspec(property(get = GetEmail, put = SetEmail)) string Email;

    void SetPassword(string Password) { this->Password = Password; }
    string GetPassword() { return this->Password; }
    __declspec(property(get = GetPassword, put = SetPassword)) string Password;

    void SetRole(string Role) { this->Role = Role; }
    string GetRole() { return this->Role; }
    __declspec(property(get = GetRole, put = SetRole)) string Role;

    void SetCreatedAt(string CreatedAt) { this->CreatedAt = CreatedAt; }
    string GetCreatedAt() { return this->CreatedAt; }
    __declspec(property(get = GetCreatedAt, put = SetCreatedAt)) string CreatedAt;

    void SetUpdatedAt(string UpdatedAt) { this->UpdatedAt = UpdatedAt; }
    string GetUpdatedAt() { return this->UpdatedAt; }
    __declspec(property(get = GetUpdatedAt, put = SetUpdatedAt)) string UpdatedAt;

    // Display Function
    void DisplayInfo() {
        cout << "User ID: " << this->Id << endl;
        cout << "Name: " << this->FirstName << " " << this->LastName << endl;
        cout << "Username: " << this->Username << endl;
        cout << "Email: " << this->Email << endl;
        cout << "Role: " << this->Role << endl;
        cout << "Created At: " << this->CreatedAt << endl;
        cout << "Updated At: " << this->UpdatedAt << endl;
    }
};
