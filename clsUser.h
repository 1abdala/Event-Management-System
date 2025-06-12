#pragma once
#include <iostream>
#include <string>
using namespace std;

class User {
private:
    int _Id;
    string _FirstName;
    string _LastName;
    string _Username;
    string _Email;
    string _Password;
    string _Role;
    string _CreatedAt;
    string _UpdatedAt;

public:
    // Constructors
    User() {}

    User(int Id, string FirstName, string LastName, string Username,
        string Email, string Password, string Role,
        string CreatedAt, string UpdatedAt) {
        _Id = Id;
        _FirstName = FirstName;
        _LastName = LastName;
        _Username = Username;
        _Email = Email;
        _Password = Password;
        _Role = Role;
        _CreatedAt = CreatedAt;
        _UpdatedAt = UpdatedAt;
    }

    // Setters and Getters with __declspec(property)

    void SetId(int Id) { _Id = Id; }
    int GetId() { return _Id; }
    __declspec(property(get = GetId, put = SetId)) int Id;

    void SetFirstName(string FirstName) { _FirstName = FirstName; }
    string GetFirstName() { return _FirstName; }
    __declspec(property(get = GetFirstName, put = SetFirstName)) string FirstName;

    void SetLastName(string LastName) { _LastName = LastName; }
    string GetLastName() { return _LastName; }
    __declspec(property(get = GetLastName, put = SetLastName)) string LastName;

    void SetUsername(string Username) { _Username = Username; }
    string GetUsername() { return _Username; }
    __declspec(property(get = GetUsername, put = SetUsername)) string Username;

    void SetEmail(string Email) { _Email = Email; }
    string GetEmail() { return _Email; }
    __declspec(property(get = GetEmail, put = SetEmail)) string Email;

    void SetPassword(string Password) { _Password = Password; }
    string GetPassword() { return _Password; }
    __declspec(property(get = GetPassword, put = SetPassword)) string Password;

    void SetRole(string Role) { _Role = Role; }
    string GetRole() { return _Role; }
    __declspec(property(get = GetRole, put = SetRole)) string Role;

    void SetCreatedAt(string CreatedAt) { _CreatedAt = CreatedAt; }
    string GetCreatedAt() { return _CreatedAt; }
    __declspec(property(get = GetCreatedAt, put = SetCreatedAt)) string CreatedAt;

    void SetUpdatedAt(string UpdatedAt) { _UpdatedAt = UpdatedAt; }
    string GetUpdatedAt() { return _UpdatedAt; }
    __declspec(property(get = GetUpdatedAt, put = SetUpdatedAt)) string UpdatedAt;

    string FullName() {
        return _FirstName + " " + _LastName;
    }

    // Display Function
    void DisplayInfo() {
        cout << "User ID: " << Id << endl;
        cout << "Name: " << FirstName << " " << LastName << endl;
        cout << "Username: " << Username << endl;
        cout << "Email: " << Email << endl;
        cout << "Role: " << Role << endl;
        cout << "Created At: " << CreatedAt << endl;
        cout << "Updated At: " << UpdatedAt << endl;
    }
};
