#ifndef LOGIN_H
#define LOGIN_H

#include <iostream>
#include <string>

#include <cppconn/prepared_statement.h>
#include "Database.h"
#include "constants.h"
using namespace std;

class Login {
private:
	string loggedInUsername;
	int userId;
	Database& database;

public:
	Login(Database& database);
	bool loginUser(const string& username, const string& password);
	void setLoggedInUsername(const string& username);
	string getUserRole();
	int getSellerId();
};



Login::Login(Database& db) : database(db) {};

bool Login::loginUser(const string& username, const string& password) {
	try
	{
		// Retrieve user credentials
		sql::PreparedStatement* pstmt = nullptr;
		pstmt = database.prepareStatement(LOGIN_QUERY);
		pstmt->setString(1, username);
		pstmt->setString(2, password);

		// Execute the query
		sql::ResultSet* res;
		res = pstmt->executeQuery();

		// Checking if the user exists in the database
		// and compare passwords
		if (res->next()) {
			string userPassword = res->getString("password"); // Gets the password from database
			if (userPassword == password) { // compares userPassword with the entered password
				delete res;
				delete pstmt;
				return true; // user login successful
			}
			else {
				cout << "Incorrect credentials. Try again with correct ones." << endl;
				return false;
			}
		}
		delete res;
		delete pstmt;

		cout << "404: User not found. Register to get access to the platform." << endl;
		return false;
	}
	catch (sql::SQLException& e)
	{
		cout << "Login failed. Error: " << e.what() << endl;
	}

	return false; // login failure
}

void Login::setLoggedInUsername(const string& username) {
	loggedInUsername = username;
}

string Login::getUserRole() {
	string role;
	try {

		sql::PreparedStatement* pstmt = nullptr;
		pstmt = database.prepareStatement(GET_USER_ROLE);
		cout << loggedInUsername << endl;

		//Set the username parameter
		pstmt->setString(1, loggedInUsername);

		// Query execution
		sql::ResultSet* res = pstmt->executeQuery();

		//check if the query returned something
		if (res->next()) {
			role = res->getString("role");

			// When role is fetched we simply equal userId to the id role relates to
			userId = res->getInt("id");

			delete res;
			delete pstmt;
			return role;

		}

		delete res;
		delete pstmt;
	}
	catch (sql::SQLException& e) {
		cout << "Failed to get user role. Error: " << e.what() << endl;
	}
	return "unknown";
}

int Login::getSellerId() {
	return userId;
}

#endif