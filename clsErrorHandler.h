#pragma once
#include <iostream>
#include <string>

using namespace std;

class clsErrorHandler {
public:
    enum class enErrorType {
        Database,
        Validation,
        Authentication,
        Booking,
        General
    };

    static void HandleError(enErrorType errorType, const string& message) {
        string errorPrefix;
        
        switch (errorType) {
            case enErrorType::Database:
                errorPrefix = "[Database Error] ";
                break;
            case enErrorType::Validation:
                errorPrefix = "[Validation Error] ";
                break;
            case enErrorType::Authentication:
                errorPrefix = "[Authentication Error] ";
                break;
            case enErrorType::Booking:
                errorPrefix = "[Booking Error] ";
                break;
            default:
                errorPrefix = "[Error] ";
        }

        string fullError = errorPrefix + message;
        
        cout << "\n" << string(40, '*') << endl;
        cout << fullError << endl;
        cout << string(40, '*') << "\n" << endl;
    }
}; 