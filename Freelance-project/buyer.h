#ifndef BUYER_H
#define BUYER_H

#include <iostream>
#include "constants.h"
#include "Database.h"
#include "User.h"
#include <cppconn/prepared_statement.h>

class Buyer : public User {
public:
	Buyer(Database& db, const string& username, const string& role, const int& userId);

	void displayDashboard() override;
	void displayPosts() override;

	void placeOrder(const int& selectedPostId);
	int countActiveOrders();
	void displayActiveOrders();
	int countCompletedOrders();
	void displayCompletedOrders();
	int countRejectedOrders();
	void displayRejectedOrders();
	string getUsernameById(int userId);
	int getIdByUsername(string& username);
	bool isPostInActiveOrders(int postId);
	void reset() override;
	void logout() override;
};

Buyer::Buyer(Database& db, const string& username, const string& role, const int& userId) : User(db, username, role, userId) {}
void Buyer::displayDashboard() {

	while (true) {
		int choice;

		cout << "Buyer Dashboard" << endl;
		cout << "\n\t\t\t\tWelcome, " << username << "!\n\n";
		cout << "1. Browse Services" << endl;
		cout << "2. Show active orders (" << countActiveOrders() << ")" << endl;
		cout << "3. Show completed orders (" << countCompletedOrders() << ")" << endl;
		cout << "4. Show rejected orders (" << countRejectedOrders() << ")" << endl;
		cout << "5. Display profile" << endl;
		cout << "5. Log out" << endl;
		cin >> choice;

		if (choice == 1) {
			displayPosts();
		}
		else if (choice == 2) {
			system("cls");
			displayActiveOrders();
		}
		else if (choice == 3) {
			displayCompletedOrders();
		}
		else if (choice == 4) {
			displayRejectedOrders();
		}
		else if (choice == 5) {

		}
		else if (choice == 6) {
			reset();
			system("cls");
			cout << "\n\n\n\t\t\t\t\tLogged Out Successfully!" << endl;

			cout << "\n\n\n\t\t\t\tGoing back to the main screen" << endl;
			system("pause");
			return;
		}
	}
}
void  Buyer::displayPosts() {
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

			// Checking if the post is already in the active orders of the user or not
			if (isPostInActiveOrders(postId)) {
				continue;
			}


			int sellerId = res->getInt("seller_id");
			string title = res->getString("post_title");
			string description = res->getString("post_description");
			string postCat = res->getString("category");
			float postPrice = res->getDouble("price");
			string sellerName = getUsernameById(sellerId);

			cout << "\n\n\t\t\t\t<--------- Post ID: " << postId << " --------->" << endl;
			cout << "\n\t\t\t\tSeller Name: " << sellerName << endl;
			cout << "\n\t\t\t\tPrice: " << postPrice << endl;
			cout << "\n\t\t\t\tCategory: " << postCat << endl;
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
			displayDashboard();
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
void  Buyer::placeOrder(const int& selectedPostId) {
	try
	{
		// First of all we will get the buyer id
		int userId = getIdByUsername(username);

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
			orderStmt->setInt(1, userId);
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
int  Buyer::countActiveOrders() {
	int activeOrders = 0;

	try
	{
		// First of all we will get the buyer id
		int userId = getIdByUsername(username);

		// Prepare SQL query to fetch active orders
		sql::PreparedStatement* pstmt;
		pstmt = database.prepareStatement(GET_ACTIVE_ORDERS_BUYER);
		pstmt->setInt(1, userId);

		sql::ResultSet* res;
		res = pstmt->executeQuery();
		while (res->next()) {
			int orderId = res->getInt("order_id");
			int sellerId = res->getInt("seller_id");
			int postId = res->getInt("post_id");
			string orderStatus = res->getString("order_status");
			string sellerName = getUsernameById(sellerId);


			if (orderStatus == "Completed" || orderStatus == "Rejected") {
				if (!res->next()) {
					system("pause");
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

			}
			delete postRes;
			delete postStmt;
			activeOrders++;

			if (!res->next()) {
				break;
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


void  Buyer::displayActiveOrders() {
	try
	{
		// First of all we will get the buyer id
		int userId = getIdByUsername(username);

		// Prepare SQL query to fetch active orders
		sql::PreparedStatement* pstmt;
		pstmt = database.prepareStatement(GET_ACTIVE_ORDERS_BUYER);
		pstmt->setInt(1, userId);


		sql::ResultSet* res;
		res = pstmt->executeQuery();
		cout << "\n\n\t\t\t\t\t\tActive Orders" << endl;

		// Flag to keep track of the posts it becomes true if at least 1 post is detected in the database
		bool activeOrderFound = false;

		while (res->next()) {
			int orderId = res->getInt("order_id");
			int sellerId = res->getInt("seller_id");
			int postId = res->getInt("post_id");
			string orderStatus = res->getString("order_status");
			string sellerName = getUsernameById(sellerId);


			if (orderStatus == "Completed" || orderStatus == "Rejected") {
				if (!res->next()) {
					system("pause");
					break;
				}
				continue;
			}

			activeOrderFound = true;

			// Fetching Post Details
			sql::PreparedStatement* postStmt = database.prepareStatement(GET_ORDER_POST_DETAILS);
			postStmt->setInt(1, postId);
			sql::ResultSet* postRes = postStmt->executeQuery();

			if (postRes->next()) {
				string postTitle = postRes->getString("post_title");
				string postDescription = postRes->getString("post_description");
				string postCat = res->getString("category");
				float postPrice = res->getDouble("price");

				cout << endl;
				cout << "Order ID: " << orderId << endl;
				cout << "Seller: " << sellerName << endl;
				cout << "Post ID: " << postId << endl;
				cout << "Price: " << postPrice << endl;
				cout << "Category: " << postCat << endl;
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
		delete res;
		delete pstmt;

		if (!activeOrderFound) {
			cout << "No active order found. Browse through posts to place order" << endl;
			system("pause");
			return;
		}

	}
	catch (sql::SQLException& e)
	{
		cout << "Could not get active orders. Error: " << e.what() << endl;
	}
}

int  Buyer::countCompletedOrders() {
	int completedOrder = 0;

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
		preStmt->setInt(1, userId);

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
			pstmt->setInt(2, userId);

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
			}
			delete postRes;
			delete postStmt;
			completedOrder++;
		}
		delete preRes;
		delete preStmt;
		return completedOrder;
	}
	catch (sql::SQLException& e)
	{
		cout << "Could not get active orders. Error:" << e.what() << endl;
		return 404;
	}

}

void  Buyer::displayCompletedOrders() {
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
		preStmt->setInt(1, userId);

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
			pstmt->setInt(2, userId);

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
				float postPrice = res->getDouble("price");

				cout << "\n\nOrder ID: " << orderId << endl;
				cout << "Seller: " << sellerName << endl;
				cout << "Category: " << category << endl;
				cout << "Post ID: " << postId << endl;
				cout << "Price: " << postPrice << endl;
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

int  Buyer::countRejectedOrders() {
	int rejectedOrders = 0;

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
		preStmt->setInt(1, userId);

		sql::ResultSet* preRes;
		preRes = preStmt->executeQuery();

		while (preRes->next()) {
			orderId = preRes->getInt("order_id");
			orderStatus = preRes->getString("order_status");
			sellerId = preRes->getInt("seller_id");
			sellerName = getUsernameById(sellerId);



			// Skips the post which does not have status of completed
			if (orderStatus != "Rejected") {
				continue;
			}
			// Then we get post id of the status that are completed
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(GET_COMPLETED_ORDERS_SELLER_BUYER);
			pstmt->setString(1, orderStatus);
			pstmt->setInt(2, userId);

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
			}
			else {
				cout << "No post to display" << endl;
			}
			delete postRes;
			delete postStmt;

			rejectedOrders++;
		}
		delete preRes;
		delete preStmt;

		return rejectedOrders;
	}
	catch (sql::SQLException& e)
	{
		cout << "Could not get active orders. Error:" << e.what() << endl;
		return 404;
	}
}

void  Buyer::displayRejectedOrders() {
	// Array size of rejected orders
	int rejOrders = countRejectedOrders();

	int* rejIds = new int[rejOrders];


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
		preStmt->setInt(1, userId);

		sql::ResultSet* preRes;
		preRes = preStmt->executeQuery();

		int count = 0;

		while (preRes->next() && !(count >= rejOrders)) {
			orderId = preRes->getInt("order_id");
			orderStatus = preRes->getString("order_status");
			sellerId = preRes->getInt("seller_id");
			sellerName = getUsernameById(sellerId);

			// Saving orderIds in the array
			rejIds[count] = orderId;

			// Skips the post which does not have status of completed
			if (orderStatus != "Rejected") {
				continue;
			}
			// Then we get post id of the status that are completed
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(GET_COMPLETED_ORDERS_SELLER_BUYER);
			pstmt->setString(1, orderStatus);
			pstmt->setInt(2, userId);

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
				float postPrice = res->getDouble("price");

				cout << "\n\nOrder ID: " << orderId << endl;
				cout << "Seller: " << sellerName << endl;
				cout << "Post ID: " << postId << endl;
				cout << "Price: " << postPrice << endl;
				cout << "Category: " << category << endl;
				cout << "Post Title: \n" << postTitle << endl;
				cout << "Post Description: \n" << postDescription << endl;
				cout << "Order Status: " << orderStatus << endl;
				cout << "-----------------------------------------" << endl;

				++count;
			}
			else {
				cout << "No post to display" << endl;
			}
			delete postRes;
			delete postStmt;
		}
		if (rejOrders == 0) {
			cout << "No Rejected Post To Show" << endl;
			system("pause");
			system("cls");
		}
		else {
			char choice;
			cout << "1. Do you want to clear rejected orders? \n"
				"2. Go back\n"
				"Enter you choice: ";
			cin >> choice;

			if (choice == '1') {
				for (int i = 0; i < rejOrders; i++) {
					cout << rejIds[i] << endl;
					int deleteOrderId = rejIds[i];
					sql::PreparedStatement* delStmt;
					delStmt = database.prepareStatement(DELETE_REJECTED_ORDERS);
					delStmt->setInt(1, deleteOrderId);

					delStmt->executeUpdate();


					//i++;
					cout << i << endl;

					delete delStmt;
				}
				cout << "List Cleared Successfully" << endl;

				system("pause");
				system("cls");
			}
			else if (choice == '2') {
				system("cls");
				return;
			}
			else {
				cout << "Invalid Operation" << endl;
				//system("pause");
				system("cls");
				return;
			}
			delete preRes;
			delete preStmt;

		}

	}
	catch (sql::SQLException& e)
	{
		cout << "Could not get active orders. Error:" << e.what() << endl;
	}
}

string  Buyer::getUsernameById(int userId) {
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
int  Buyer::getIdByUsername(string& username) {
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

bool  Buyer::isPostInActiveOrders(int postId) {
	try
	{
		sql::PreparedStatement* pstmt = nullptr;
		pstmt = database.prepareStatement(CHECK_POST_IN_ACTIVE_ORDERS);
		pstmt->setInt(1, userId);
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
void  Buyer::reset() {
	int userId = 0;
	string role = "";
	string username = "";
	isLoggedIn = false;
}

void Buyer::logout() {
	isLoggedIn = false;
}


#endif // !BUYER_H

