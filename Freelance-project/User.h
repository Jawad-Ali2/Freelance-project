#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include "Database.h"
#include "constants.h"

using namespace std;

// Abstract class

class User {
protected:
	int userId;
	string role;
	string username;
	float creds;
	Database database;
	bool isLoggedIn;

public:
	User() : userId(0), role(""), username(""), creds(0), isLoggedIn(false), database() {
	}
	User(Database& db, const string& username, const string& role, const int& userId, const float& userCreds)
		: database(db), userId(userId), role(role), username(username), creds(userCreds), isLoggedIn(false) {}

	virtual void displayDashboard() = 0;
	virtual void displayPosts() = 0;
	virtual void reset() = 0;
	virtual void logout() = 0;
};


#endif
