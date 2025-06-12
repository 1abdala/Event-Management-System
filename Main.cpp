#include <iostream>
#include <cstdlib>
#include "Database.h"
#include "clsLoginScreen.h"
using namespace std;

int main() {
	try {
		// Initialize database connection
		connectToDatabase();

		// Clear screen and show welcome message
		system("cls");
		cout << "\n\t\t=== Welcome to Event Management System ===\n\n";

		// Main program loop
		clsLoginScreen loginScreen;
		while (loginScreen.displayScreen()) {
			// Keep looping until user exits
		}

		// Clean up database connection
		closeDatabaseConnection();
		return 0;
	}
	catch (const exception& e) {
		cout << "\nError: " << e.what() << endl;
		return 1;
	}
}