#include <iostream>
#include "Database.h"
#include "constants.h"
#include "register.h"
#include "login.h"
#include "buyer.h"
#include "seller.h"


using namespace std;


int main() {
	bool isLoggedIn = false;
	Database database(DB_HOST, DB_USERNAME, DB_PASSWORD, DB_NAME);
	Registration registration(database);
	Login login(database);


	while (true) {
		//system("cls");

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


			system("pause");

		}
		else if (choice == 2) {
			// Login 
			string username, password;

			cout << "LOGIN FORM" << endl;
			cout << "Username: "; cin >> username;
			cout << "Password: "; cin >> password;
			if (login.loginUser(username, password)) {
				system("cls");
				cout << "Login successfully!" << endl;
				login.setLoggedInUsername(username);
				system("pause");
			}
			string role = login.getUserRole();
			int userId = login.getUserId();
			float userCreds = login.getUserCreds();

			if (userId) {
				isLoggedIn = true;
			}

			if (role == "Buyer" && isLoggedIn) {
				User* buyer = new Buyer(database, username, role, userId, userCreds);
				system("pause");
				system("cls");
				buyer->displayDashboard();
				buyer->logout();
				delete buyer;
			}
			else if (role == "Seller" && isLoggedIn) {
				User* seller = new Seller(database, username, role, userId, userCreds);
				system("pause");
				system("cls");
				seller->displayDashboard();
				seller->logout();
				delete seller;
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
