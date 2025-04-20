#include "Database.h"
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <fstream>
sql::Connection* globalCon = nullptr;


void connectToDatabase() {
    try {
        // Get a pointer to the database driver instance
        sql::Driver* driver = get_driver_instance();
        // Establish connection to the database server at the specified address and port
        globalCon = driver->connect("tcp://127.0.0.1:3306", "root", "");
        // Set the current database schema to " event_management"
        globalCon->setSchema(" event_management");
    }
    catch (sql::SQLException& e) {
        // Output error details if an SQL exception occurs
        cout << "SQL Error: " << e.what() << " (MySQL error code: " << e.getErrorCode()
            << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}

void closeDatabaseConnection() {
    // Check if the global connection object is not null
    if (globalCon) {

        globalCon->close(); // Close the database connection
        delete globalCon; // Delete the connection object to free memory
        globalCon = nullptr; // Reset the global connection pointer to null to avoid dangling pointer
    }
}

void executeInstruction(const string& instruction) {
    try {
        // Create a new SQL statement using the global connection
        sql::Statement* stmt = globalCon->createStatement();
        // Execute the SQL instruction passed to the function
        stmt->execute(instruction);
        // Delete the statement to free memory
        delete stmt;
    }
    catch (sql::SQLException& e) {
        // Output error details if an SQL exception occurs during the operation
        cout << "SQL Error: " << e.what() << " (MySQL error code: " << e.getErrorCode()
            << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}


string executeQuery(const string& query) {
    string result;// Variable to store the result of the query
    try {
        // Create a statement object using the global database connection
        sql::Statement* stmt = globalCon->createStatement();
        // Execute the query and get the result set
        sql::ResultSet* res = stmt->executeQuery(query);

        // Get metadata
        sql::ResultSetMetaData* res_meta = res->getMetaData();
        int columns = res_meta->getColumnCount();

        // Print column names
        for (int i = 1; i <= columns; i++) {
            result += res_meta->getColumnName(i) + "\t";
        }
        result += "\n";

        // Print query results
        while (res->next()) {
            for (int i = 1; i <= columns; i++) {
                result += res->getString(i) + "\t";
            }

            result += "\n";
            cout << result;
        }
        //clear the memory
        delete res;// Delete the result set
        delete stmt;//Delete the result statement
    }
    catch (sql::SQLException& e) {
        // Handle exceptions by printing error details
        cout << "SQL Error: " << e.what() << " (MySQL error code: " << e.getErrorCode()
            << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
    return result;// Return the result of the query
}