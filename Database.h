#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include<sstream>
#include<string>
#include <map>
using namespace std;
extern int currentCustomerID;

// Forward declarations
namespace sql {
    class Connection;
    class ResultSet;
}

// Database configuration
struct DatabaseConfig {
    static const char* HOST;
    static const char* USER;
    static const char* PASS;
    static const char* SCHEMA;
};

// Database connection management
bool connectToDatabase();
void closeDatabaseConnection();
void executeInstruction(const string& instruction);
string executeQuery(const string& query);
string executeQuerySilent(const string& query);

// Database exception class
class DatabaseException : public std::runtime_error {
public:
    DatabaseException(const std::string& message) : std::runtime_error(message) {}
};

// Function to create the announcements table
void createAnnouncementsTable();
#endif