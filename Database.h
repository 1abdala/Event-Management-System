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
void connectToDatabase();
void closeDatabaseConnection();
void executeInstruction(const string& instruction);
string executeQuery(const string& query);
#endif