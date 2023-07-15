#ifndef LOGIN_H
#define LOGIN_H

#include <string>
#include "Database.h"

using namespace std;

class Login {
private:
	string loggedInUsername;
	int sellerId;
	Database& database;

public:
	Login(Database& database);
	bool loginUser(const string& username, const string& password);
	void setLoggedInUsername(const string& username);
	string getUserRole();
	int getSellerId();
};


#endif