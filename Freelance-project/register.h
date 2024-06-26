
#ifndef REGISTER_H
#define REGISTER_H

#include <iostream>
#include <string>
#include <regex>
#include <cppconn/prepared_statement.h>
#include "Database.h"
#include "constants.h"
#include "register.h"

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



bool Registration::isValidEmail(const string& email) {
	// Validation format for email
	const regex emailPattern(R"([\w-]+(?:\.[\w-]+)*@(?:[\w-]+\.)+[a-zA-Z]{2,7})");
	return regex_match(email, emailPattern);
}

bool Registration::isStrongPassword(const string& password) {
	// validation for password
	const int minPasswordLength = 8;
	if (password.length() < minPasswordLength) {
		return false;
	}

	return true;
}

bool Registration::isValidRole(const string& role) {
	// Validation for role
	return (role == "buyer" || role == "seller");
}
void Registration::registerUser() {
	float creds = 10000;
	string username;
	string email;
	string password;
	string role;

	// Getting user input
	cout << "Registration Form" << endl;
	cout << "Username: "; cin >> username;
	cin.ignore();

	cout << "Email: "; cin >> email;

	if (!isValidEmail(email)) {
		cout << "Invalid email format. Please enter a valid email address." << endl;
		return;
	}

	cout << "Password: "; cin >> password;

	if (!isStrongPassword(password)) {
		cout << "Weak Password. Please choose a stronger password." << endl;
		return;
	}
	cout << "Select your role: (buyer/seller)"; cin >> role;

	if (!isValidRole(role)) {
		cout << "Invalid role. Please choose either 'buyer' or 'seller'." << endl;
		return;
	}

	if (role == "seller") {
		creds = 0;
	}

	try
	{
		// Connection already built with Database Class

		// Prepared SQL statement
		sql::PreparedStatement* pstmt = nullptr;
		pstmt = database.prepareStatement(REGISTER_QUERY);

		// Setting parameters values
		pstmt->setString(1, username);
		pstmt->setString(2, email);
		pstmt->setString(3, password);
		pstmt->setDouble(4, creds);
		pstmt->setString(5, role);

		// Executing the insert statement
		pstmt->execute();

		//delete stmt;
		delete pstmt;

		if (role == "seller") {
			cout << "Registered Successfully!" << endl;
		}
		else {
			cout << "Registered Successfully! You've been given 10000 credits." << endl;
		}
	}
	catch (sql::SQLException& e) {
		std::cout << "User registration failed. Error: " << e.what() << std::endl;
	}
}



#endif

