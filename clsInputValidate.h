#pragma once
#include<iostream>
using namespace std;
#include "clsString.h";
#include "clsDate.h";
#include<vector>
class clsInputValidate
{
public:
	static bool IsValidDateFormat(const string& date) {
		// YYYY-MM-DD
		if (date.length() != 10) return false;
		if (date[4] != '-' || date[7] != '-') return false;
		string yyyy = date.substr(0, 4);
		string mm = date.substr(5, 2);
		string dd = date.substr(8, 2);
		return all_of(yyyy.begin(), yyyy.end(), ::isdigit) &&
			all_of(mm.begin(), mm.end(), ::isdigit) &&
			all_of(dd.begin(), dd.end(), ::isdigit);
	}

	static bool IsValidTimeFormat(const string& time) {
		// HH:MM:SS
		if (time.length() != 8) return false;
		if (time[2] != ':' || time[5] != ':') return false;
		string hh = time.substr(0, 2);
		string mm = time.substr(3, 2);
		string ss = time.substr(6, 2);
		return all_of(hh.begin(), hh.end(), ::isdigit) &&
			all_of(mm.begin(), mm.end(), ::isdigit) &&
			all_of(ss.begin(), ss.end(), ::isdigit);
	}

	static bool IsNumberBetween(short number, short from, short to) {

		return number >= from && number <= to;

	}

	static bool IsNumberBetween(int number, int from, int to) {

		return number >= from && number <= to;

	}

	static bool IsNumberBetween(float number, float from, float to) {

		return number >= from && number <= to;

	}

	static double IsNumberBetween(double number, double  from, double to) {

		return number >= from && number <= to;

	}



	static bool IsDateBetween(clsDate date(), clsDate from(), clsDate to()) {
		//date1 >=from & <=to
		if ((clsDate::IsDate1AfterDate2(date(), from()) || clsDate::IsDate1EqualDate2(date(), to()))
			&& (clsDate::IsDate1BeforeDate2(date(), to()) || clsDate::IsDate1EqualDate2(date(), to())))
			return true;

		//date1 <=from & >=to

		if (clsDate::IsDate1AfterDate2(date(), to()) || clsDate::IsDate1EqualDate2(date(), to())
			&& (clsDate::IsDate1BeforeDate2(date(), from()) || clsDate::IsDate1EqualDate2(date(), to())))
			return true;

		return false;


	}

	static int ReadIntNumber() {
		int number;
		//cout << "\ninter an intger: ";
		while (true) {
			cin >> number;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "invalid number, enter agin: ";


			}
			else {
				return number;
			}

		}


	}

	static short ReadShortNumber() {
		short number;
		//cout << "\ninter an intger: ";
		while (true) {
			cin >> number;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "invalid number, enter agin: ";


			}
			else {
				return number;
			}

		}


	}

	static double ReadDplNumber() {
		double number;
		//cout << "\ninter a duoble: ";
		while (true) {
			cin >> number;
			if (cin.fail()) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "invalid number, enter agin: ";
				;


			}
			else {
				return number;
			}

		}


	}

	static short  ReadShortNumberBetween(short from, short to, string invalidm) {
		short number;
		//cout << "inter a number from " << from << " to " << to << ":";

		number = ReadIntNumber();
		while (!IsNumberBetween(number, from, to)) {
			cout << invalidm;
			number = ReadIntNumber();


		}
		return number;
	};
	static int  ReadIntNumberBetween(int from, int to, string invalidm) {
		int number;
		//cout << "inter a number from " << from << " to " << to << ":";

		number = ReadIntNumber();
		while (!IsNumberBetween(number, from, to)) {
			cout << invalidm;
			number = ReadIntNumber();


		}
		return number;
	};


	static double ReadDblNumberBetween(double from, double to, string invalidm) {
		double number;
		//cout << "inter a number from " << from << " to " << to << ":";

		number = ReadDplNumber();
		while (!IsNumberBetween(number, from, to)) {
			cout << invalidm;
			number = ReadIntNumber();


		}
		return number;
	}

	static string ReadString() {
		string s1 = "";
		getline(cin >> ws, s1);
		return s1;


	}
	static	bool IsValidDate(clsDate date)
	{
		return clsDate::IsValidDate(date);
	}
};