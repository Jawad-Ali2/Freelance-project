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
			cout << "2. Show active orders (" << activeOrdersCount() << ")" << endl;
			cout << "3. Show completed orders" << endl;
			cout << "4. Display profile" << endl;
			cout << "5. Log out" << endl;
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
				displayCompletedOrders();
				system("pause");
			}
			else if (choice == 4) {

			}
			else if (choice == 5) {
				reset();
				system("cls");
				cout << "\n\n\n\t\t\t\t\tLogged Out Successfully!" << endl;

				cout << "\n\n\n\t\t\t\tGoing back to the main screen" << endl;
				system("pause");
				return;
			}
		}
	}
	void displaySellerPosts() {
		string selectedCategory;

		cout << "Which Category You Want To Search For? "; cin >> selectedCategory;

		try
		{
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(GET_SELLER_POSTS);
			pstmt->setString(1, selectedCategory);

			// Executing query
			sql::ResultSet* res;
			res = pstmt->executeQuery();

			int count = 0; // Counter to keep track of no of post displayed

			// Display Posts
			while (res->next()) {
				int postId = res->getInt("post_id");
				cout << postId << endl;

				// Checking if the post is already in the active orders of the user or not
				if (isPostInActiveOrders(postId)) {
					continue;
				}


				int sellerId = res->getInt("seller_id");
				string title = res->getString("post_title");
				string description = res->getString("post_description");
				string sellerName = getUsernameById(sellerId);

				cout << "\n\n\t\t\t\t<--------- Post ID: " << postId << " --------->" << endl;
				cout << "\n\t\t\t\tSeller Name: " << sellerName << endl;
				cout << "\n\t\t\t\Category: " << selectedCategory << endl;
				cout << "\n\t\t\t\tTitle: " << title << endl;
				cout << "\n\t\t\t\tDescription: \n\t\t\t\t" << description << endl;
				cout << "\n\t\t\t\t-------------------------------------" << endl;


				// Checks if three post have been displayed on same line
				count++;
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
			if (count == 0) {
				cout << "There are no services with given category. Please select different category." << endl;
				system("pause");
				system("cls");
				displayBuyerDashboard();
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
			cout << "Failed to retrieve services check category. Error: " << e.what() << endl;
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
	int activeOrdersCount() {
		int activeOrders = 0;

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

				while (true) {
					int orderId = res->getInt("order_id");
					int sellerId = res->getInt("seller_id");
					int postId = res->getInt("post_id");
					string orderStatus = res->getString("order_status");
					string sellerName = getUsernameById(sellerId);

					if (orderStatus == "Completed" || orderStatus == "Rejected") {
						cout << "Here" << endl;
						break;
					}

					// Fetching Post Details
					sql::PreparedStatement* postStmt = database.prepareStatement(GET_ORDER_POST_DETAILS);
					postStmt->setInt(1, postId);
					sql::ResultSet* postRes = postStmt->executeQuery();

					if (postRes->next()) {
						string postTitle = postRes->getString("post_title");
						string postDescription = postRes->getString("post_description");

					}
					delete postRes;
					delete postStmt;
					activeOrders++;

					if (!res->next()) {
						break;
					}
				}
			}
			delete res;
			delete pstmt;

			return activeOrders;
		}
		catch (sql::SQLException& e)
		{
			cout << "Could not get active orders. Error: " << e.what() << endl;
			return 404;
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


					if (orderStatus == "Completed" || orderStatus == "Rejected") {

						if (!res->next()) {
							break;
						}
						continue;
					}

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

	void displayCompletedOrders() {
		string orderStatus;
		int orderId;
		int sellerId;
		string sellerName;
		int postId;
		try
		{
			// First we get order status and buyer name from this query
			sql::PreparedStatement* preStmt = nullptr;
			preStmt = database.prepareStatement(GET_ACTIVE_ORDERS_BUYER);
			preStmt->setInt(1, buyerId);

			sql::ResultSet* preRes;
			preRes = preStmt->executeQuery();

			while (preRes->next()) {
				orderId = preRes->getInt("order_id");
				orderStatus = preRes->getString("order_status");
				sellerId = preRes->getInt("seller_id");
				sellerName = getUsernameById(sellerId);



				// Skips the post which does not have status of completed
				if (orderStatus != "Completed") {
					continue;
				}
				// Then we get post id of the status that are completed
				sql::PreparedStatement* pstmt = nullptr;
				pstmt = database.prepareStatement(GET_COMPLETED_ORDERS_SELLER_BUYER);
				pstmt->setString(1, orderStatus);
				pstmt->setInt(2, buyerId);

				sql::ResultSet* res;
				res = pstmt->executeQuery();

				if (res->next()) {
					postId = res->getInt("post_id");

					delete res;
					delete pstmt;
				}
				// Fetch  Post details
				sql::PreparedStatement* postStmt = database.prepareStatement(GET_ORDER_POST_DETAILS);
				postStmt->setInt(1, postId);
				sql::ResultSet* postRes = postStmt->executeQuery();


				if (postRes->next()) {
					string postTitle = postRes->getString("post_title");
					string postDescription = postRes->getString("post_description");
					string category = postRes->getString("category");

					cout << "\n\nOrder ID: " << orderId << endl;
					cout << "Seller: " << sellerName << endl;
					cout << "Category: " << category << endl;
					cout << "Post ID: " << postId << endl;
					cout << "Post Title: \n" << postTitle << endl;
					cout << "Post Description: \n" << postDescription << endl;
					cout << "Order Status: " << orderStatus << endl;
					cout << "-----------------------------------------" << endl;

				}
				else {
					cout << "No post to display" << endl;
				}
				delete postRes;
				delete postStmt;
			}
			delete preRes;
			delete preStmt;

		}
		catch (sql::SQLException& e)
		{
			cout << "Could not get active orders. Error:" << e.what() << endl;
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
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(CHECK_POST_IN_ACTIVE_ORDERS);
			pstmt->setInt(1, buyerId);
			pstmt->setInt(2, postId);

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
		}
		return false;
	}
	void reset() {
		int buyerId = 0;
		string role = "";
		string username = "";
	}
};


#endif // !BUYER_H

