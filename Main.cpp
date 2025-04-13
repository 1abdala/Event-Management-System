#include <iostream>
using namespace std;
#include "Database.h"

int main(){
	string res;
	connectToDatabase();
     executeInstruction("insert into user values(2,'FADAL');");
	closeDatabaseConnection();


	return 0;
}