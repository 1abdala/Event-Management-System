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

		// Show exit message
		system("cls");
		cout << "\n\n\t\t==========================================\n";
		cout << "\t\t|                                        |\n";
		cout << "\t\t|     Thank you for using our system!    |\n";
		cout << "\t\t|           Have a great day!            |\n";
		cout << "\t\t|                                        |\n";
		cout << "\t\t==========================================\n\n";
		
		// Wait for a moment before closing
		system("timeout /t 2 > nul");
		return 0;
	}
	catch (const exception& e) {
		cout << "\nError: " << e.what() << endl;
		cout << "\nPress any key to exit...";
		system("pause>nul");
		return 1;
	}
}