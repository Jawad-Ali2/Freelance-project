#ifndef BUYER_H
#define BUYER_H

#include <iostream>
#include "constants.h"
#include "Database.h"
#include <cppconn/prepared_statement.h>

class Buyer {
private:
	int buyerId;
	string role;
	Database& database;
	string username;
public:
	Buyer(Database& db, const string& username, const string& role, const int& buyerId) : database(db) {
		this->username = username;
		this->role = role;
		this->buyerId = buyerId;
	}
	void displayBuyerDashboard() {
		cout << "Buyer Dashboard" << endl;
		cout << "\n\t\t\t\tWelcome, " << username << "!\n\n";

		while (true) {
			int choice;

			cout << "1. Browse Services" << endl;
			cout << "2. Show active orders" << endl;
			cout << "3. Display profile" << endl;
			cin >> choice;

			if (choice == 1) {
				displaySellerPosts();
			}
			else if (choice == 2) {
				system("cls");
				displayActiveOrders();
				system("pause");
			}
			else if (choice == 3) {
			}
		}
	}
	void displaySellerPosts() {
		try
		{
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(GET_SELLER_POSTS);

			// Executing query
			sql::ResultSet* res;
			res = pstmt->executeQuery();

			int count = 0; // Counter to keep track of no of post displayed

			// Display Posts
			while (res->next()) {
				int postId = res->getInt("post_id");

				// Checking if the post is already in the active orders of the user or not
				if (isPostInActiveOrders(postId)) {
					cout << "Here" << endl;
					continue;
				}


				int sellerId = res->getInt("seller_id");
				string title = res->getString("post_title");
				string description = res->getString("post_description");
				string sellerName = getUsernameById(sellerId);

				cout << "\n\n<--------- Post ID: " << postId << " --------->" << endl;
				cout << "\n\tSeller Name: " << sellerName << endl;
				cout << "\nTitle: " << title << endl;
				cout << "\nDescriptions: \n\t" << description << endl;
				cout << "-------------------------------------" << endl;

				count++;

				// Checks if three post have been displayed on same line
				if (count % 3 == 0) {
					cout << endl;
					cout << "Want to show more posts? (Enter to Continue or q to place order): ";
					string input;
					cin.ignore();
					getline(cin, input);

					if (input == "q") {
						break;
					}
				}
			}
			delete res;
			delete pstmt;

			// Prompt for the user to select which service he would like to order
			int selectedPostId;
			cout << "Enter the ID of the post you want to order: ";
			cin >> selectedPostId;

			// For Validation of the selectedPostID if it exists
			sql::PreparedStatement* validateStmt = nullptr;
			validateStmt = database.prepareStatement(VALIDATE_POST_ID);
			validateStmt->setInt(1, selectedPostId);
			sql::ResultSet* validateRes;
			validateRes = validateStmt->executeQuery();

			if (validateRes->next()) {
				// If id exists, place order for the selected postId
				placeOrder(selectedPostId);

			}
			else {
				// Id does not exists, show error message
				cout << "Invalid Post ID. Please Try Again." << endl;
			}
			delete validateRes;
			delete validateStmt;

		}
		catch (sql::SQLException& e)
		{
			cout << "Failed to retrieve services. Error: " << e.what() << endl;
		}
	}
	void placeOrder(const int& selectedPostId) {
		try
		{
			// First of all we will get the buyer id
			int buyerId = getIdByUsername(username);

			// Getting seller's id
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(GET_ORDER_POST_DETAILS);
			pstmt->setInt(1, selectedPostId);

			sql::ResultSet* res;
			res = pstmt->executeQuery();

			if (res->next()) {
				int sellerId = res->getInt("seller_id");

				// Inserting data into the table
				sql::PreparedStatement* orderStmt = nullptr;
				orderStmt = database.prepareStatement(INSERT_ORDER);
				orderStmt->setInt(1, buyerId);
				orderStmt->setInt(2, sellerId);
				orderStmt->setInt(3, selectedPostId);
				orderStmt->setString(4, "Placed"); //Initial status

				orderStmt->execute();

				cout << "\n\n\t\t\t\t\t\tOrder placed successfully!" << endl;
				delete orderStmt;
			}
			delete res;
			delete pstmt;

		}
		catch (sql::SQLException& e)
		{
			cout << "Failed to place order. Error: " << e.what() << endl;
		}
	}
	void displayActiveOrders() {
		try
		{
			// First of all we will get the buyer id
			int buyerId = getIdByUsername(username);

			// Prepare SQL query to fetch active orders
			sql::PreparedStatement* pstmt;
			pstmt = database.prepareStatement(GET_ACTIVE_ORDERS_BUYER);
			pstmt->setInt(1, buyerId);

			sql::ResultSet* res;
			res = pstmt->executeQuery();

			if (res->next()) {
				cout << "\n\n\t\t\t\t\t\tActive Orders" << endl;

				while (true) {
					int orderId = res->getInt("order_id");
					int sellerId = res->getInt("seller_id");
					int postId = res->getInt("post_id");
					string orderStatus = res->getString("order_status");
					string sellerName = getUsernameById(sellerId);

					// Fetching Post Details
					sql::PreparedStatement* postStmt = database.prepareStatement(GET_ORDER_POST_DETAILS);
					postStmt->setInt(1, postId);
					sql::ResultSet* postRes = postStmt->executeQuery();

					if (postRes->next()) {
						string postTitle = postRes->getString("post_title");
						string postDescription = postRes->getString("post_description");

						cout << endl;
						cout << "Order ID: " << orderId << endl;
						cout << "Seller: " << sellerName << endl;
						cout << "Post ID: " << postId << endl;
						cout << "Post Title: " << postTitle << endl;
						cout << "Post Description: " << postDescription << endl;
						cout << "Order Status: " << orderStatus << endl;
						cout << "-------------------------------------\n\n\n" << endl;
					}
					delete postRes;
					delete postStmt;

					if (!res->next()) {
						break;
					}
				}
			}
			else {
				cout << "No active orders found. Browse through post to place one." << endl;
			}
			delete res;
			delete pstmt;
		}
		catch (sql::SQLException& e)
		{
			cout << "Could not get active orders. Error: " << e.what() << endl;
		}
	}

	string getUsernameById(int userId) {
		try
		{
			// Prepare SQL Query
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(GET_USERNAME_BY_ID);
			pstmt->setInt(1, userId);


			sql::ResultSet* res;
			res = pstmt->executeQuery();

			if (res->next()) {
				string username = res->getString("username");
				delete res;
				delete pstmt;

				return username;
			}

		}
		catch (sql::SQLException& e)
		{
			cout << "Failed to get User Name. Error: " << e.what() << endl;
		}
		return "unknown";
	}
	int getIdByUsername(string& username) {
		try
		{
			// Prepare SQL Query
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(GET_ID_BY_USERNAME);
			pstmt->setString(1, username);


			sql::ResultSet* res;
			res = pstmt->executeQuery();

			if (res->next()) {
				int userId = res->getInt("id");
				delete res;
				delete pstmt;

				return userId;
			}

		}
		catch (sql::SQLException& e)
		{
			cout << "Failed to get User Name. Error: " << e.what() << endl;
		}
		return 404;
	}

	bool isPostInActiveOrders(int postId) {
		try
		{
			cout << "here" << endl;
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(CHECK_POST_IN_ACTIVE_ORDERS);
			pstmt->setInt(1, postId);
			pstmt->setInt(2, buyerId);

			sql::ResultSet* res;
			res = pstmt->executeQuery();

			//Checking if the query returns result
			bool isInActiveOrders = res->next();

			delete res;
			delete pstmt;

			return isInActiveOrders;
		}
		catch (sql::SQLException& e)
		{
			cout << "Failed to check if post is in the active orders. Error: " << e.what() << endl;
			return false;
		}
	}
};


#endif // !BUYER_H

