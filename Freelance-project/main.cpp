#include <iostream>
#include "Database.h"
#include "constants.h"
#include "register.h"
#include "login.h"
#include "buyer.h"
#include "seller.h"

// this is for testing
using namespace std;


int main() {
	Database database(DB_HOST, DB_USERNAME, DB_PASSWORD, DB_NAME);
	Registration registration(database);
	Login login(database);


	while (true) {
		int choice;
		cout << "Welcome to Freelance Platform" << endl;
		cout << "1. Register" << endl;
		cout << "2. Login" << endl;
		cout << "3. Exit" << endl;
		cout << "Enter your choice: ";
		cin >> choice;

		if (choice == 1) {
			system("cls");
			// Register new user
			registration.registerUser();

			cout << "Registered Successfully!" << endl;
			system("pause");

		}
		else if (choice == 2) {
			// Login 
			string username, password;

			cout << "LOGIN FORM" << endl;
			cout << "Username: "; cin >> username;
			cout << "Password: "; cin >> password;
			if (login.loginUser(username, password)) {
				cout << "Login successfully!" << endl;
				login.setLoggedInUsername(username);
				system("pause");
			}
			string role = login.getUserRole();
			int sellerId = login.getSellerId();
			system("cls");
			if (role == "Buyer") {
				Buyer buyer;
				buyer.displayBuyerDashboard();
			}
			else if (role == "Seller") {
				Seller seller(database, username, role, sellerId);
				seller.displaySellerDashboard();
			}
		}
		else if (choice == 3)
		{
			exit(0);
		}
		else {
			cout << "Invalid operation!" << endl;
			exit(404);
		}


	}
	return 0;
}
