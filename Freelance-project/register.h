
#ifndef REGISTER_H
#define REGISTER_H

#include <string>
#include <regex>
#include "Database.h"

using namespace std;

class Registration {
private:
	Database& database;

	bool isValidEmail(const string& email);
	bool isStrongPassword(const string& password);
	bool isValidRole(const string& role);
public:
	Registration(Database& db) : database(db) {};

	void registerUser();
};




#endif

