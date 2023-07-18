#ifndef SELLER_H
#define SELLER_H

#include <iostream>
#include <string>
#include "login.h"
#include "constants.h"
#include "Database.h"
#include <cppconn/prepared_statement.h>

using namespace std;

class Seller {
private:
	int sellerId;
	string role;
	Database& database;
	string username;
public:
	Seller(Database& db, const string& username, const string& role, const int& sellerId);
	void displaySellerDashboard();
	void addPost(const int& sellerId);
	void displayPosts();
	void displayActiveOrders();
	void displayCompletedOrders();
	string getUsernameById(int userId);
	void changeOrderStatus(int postId);
};


Seller::Seller(Database& db, const string& username, const string& role, const int& sellerId) : database(db) {
	this->username = username;
	this->role = role;
	this->sellerId = sellerId;
}

void Seller::displaySellerDashboard() {
	cout << "Seller Dashboard\n" << endl;
	cout << "Welcome, " << username << "!\n\n";

	while (true) {
		int choice;

		cout << "1. Add Post" << endl;
		cout << "2. Display all your post" << endl;
		cout << "3. Show active orders" << endl;
		cout << "4. Show completed orders" << endl;
		cin >> choice;

		if (choice == 1) {
			addPost(sellerId);
		}
		else if (choice == 2) {
			displayPosts();
		}
		else if (choice == 3) {
			displayActiveOrders();
		}
		else if (choice == 4) {
			displayCompletedOrders();
		}
		else {
			cout << "Invalid input!" << endl;
		}
	}

}

void Seller::addPost(const int& sellerId) {

	string postTitle;
	string postDescription;

	cin.ignore();
	cout << "Add Post" << endl;
	cout << "Post Title: ";
	getline(cin, postTitle);
	cout << "Post Description: ";
	getline(cin, postDescription);

	try
	{
		sql::PreparedStatement* pstmt;
		pstmt = database.prepareStatement(CREATE_SELLER_POST);
		pstmt->setInt(1, sellerId);
		pstmt->setString(2, postTitle);
		pstmt->setString(3, postDescription);

		pstmt->execute();

		cout << "Post added successfully" << endl;
	}
	catch (sql::SQLException& e)
	{
		cout << "Failed to create post. Error: " << e.what() << endl;
	}

}

void Seller::displayPosts() {
	try
	{
		sql::PreparedStatement* pstmt;
		// Get all the posts
		pstmt = database.prepareStatement(GET_ALL_POSTS);
		pstmt->setInt(1, sellerId); // of the seller with id

		sql::ResultSet* res;
		res = pstmt->executeQuery();
		if (res->next()) {
			cout << "Your Posts: \n" << endl;

			while (res->next()) {
				int postId = res->getInt("post_id");
				string postTitle = res->getString("post_title");
				string postDesc = res->getString("post_description");


				cout << "Post ID: " << postId << endl;
				cout << "Title: " << postTitle << endl;
				cout << "Description: " << postDesc << endl;
				cout << "-----------------------------------------" << endl;
				cout << endl;
			}

			delete res;
			delete pstmt;

		}
		else
		{
			cout << "You haven't posted yet. Make some posts and show your audience what you can do!" << endl;
		}
	}
	catch (sql::SQLException& e)
	{
		cout << "Failed to fetch posts. Error: " << e.what() << endl;
	}
}

void Seller::displayActiveOrders() {
	char choice;
	int postId;
	try
	{
		sql::PreparedStatement* pstmt;
		// Get Active_orders from the database based on the id
		pstmt = database.prepareStatement(GET_ACTIVE_ORDERS_SELLER);
		pstmt->setInt(1, sellerId); // Provides the id of the user

		sql::ResultSet* res;
		res = pstmt->executeQuery();

		if (res->next()) {
			cout << "Active Orders" << endl;

			while (res->next()) {
				int orderId = res->getInt("order_id");
				int buyerId = res->getInt("buyer_id");
				int postId = res->getInt("post_id");
				// If the order status is 'Completed' we move that post to completed orders
				string orderStatus = res->getString("order_status");
				string buyerName = getUsernameById(buyerId);


				// Skips the post which has status completed
				if (orderStatus == "Completed") {
					displayCompletedOrders();
					continue;
				}

				// Fetch  Post details
				sql::PreparedStatement* postStmt = database.prepareStatement(GET_ORDER_POST_DETAILS);
				postStmt->setInt(1, postId);
				sql::ResultSet* postRes = postStmt->executeQuery();

				if (postRes->next()) {
					string postTitle = postRes->getString("post_title");
					string postDescription = postRes->getString("post_description");

					/*cout << "Order ID: " << orderId << endl;
					cout << "Buyer ID: " << buyerId << endl;*/
					cout << "Buyer: " << buyerName << endl;
					cout << "Post ID: " << postId << endl;
					cout << "Post Title: \n" << postTitle << endl;
					cout << "Post Description: \n" << postDescription << endl;
					cout << "Order Status: " << orderStatus << endl;
					cout << "-----------------------------------------" << endl;

				}
				delete postRes;
				delete postStmt;

			}
			// If user wants to change the status of the active order
			cout << "1. Do you want to change the status of Active Order?\n"
				"2. Do you want to reject pending order?\n"
				"3. Go back to main menu\n";
			cout << "Enter your choice: "; cin >> choice;
			if (choice == '1') {
				cout << "Enter the Post Id: "; cin >> postId;
				changeOrderStatus(postId);
			}
			else if (choice == '2') {
				cout << "Enter the Post Id: "; cin >> postId;

			}
			else {
				displaySellerDashboard();
			}
		}
		else {
			cout << "No active order found. Engage your client by showing them your skills" << endl;
		}
		delete res;
		delete pstmt;
	}
	catch (sql::SQLException& e)
	{
		cout << "Couldn't get active orders. Error: " << e.what() << endl;
	}
}

void Seller::displayCompletedOrders() {
	string orderStatus;
	int buyerId;
	string buyerName;
	int postId;
	try
	{
		// First we get order status and buyer name from this query
		sql::PreparedStatement* preStmt = nullptr;
		preStmt = database.prepareStatement(GET_ACTIVE_ORDERS_SELLER);
		preStmt->setInt(1, sellerId);

		sql::ResultSet* preRes;
		preRes = preStmt->executeQuery();

		if (preRes->next()) {
			orderStatus = preRes->getString("order_status");
			buyerId = preRes->getInt("buyer_id");
			buyerName = getUsernameById(buyerId);

			delete preRes;
			delete preStmt;
		}

		// Then we get post id of the status that are completed
		sql::PreparedStatement* pstmt = nullptr;
		pstmt = database.prepareStatement(GET_COMPLETED_ORDERS);
		pstmt->setString(1, orderStatus);
		pstmt->setInt(2, sellerId);

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

			cout << "Buyer: " << buyerName << endl;
			cout << "Post ID: " << postId << endl;
			cout << "Post Title: \n" << postTitle << endl;
			cout << "Post Description: \n" << postDescription << endl;
			cout << "Order Status: " << orderStatus << endl;
			cout << "-----------------------------------------" << endl;

		}
		delete postRes;
		delete postStmt;

	}
	catch (sql::SQLException& e)
	{
		cout << "Could not get active orders. Error:" << e.what() << endl;
	}

}

string Seller::getUsernameById(int userId) {
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

void Seller::changeOrderStatus(int postId) {
	char choice;
	bool isOrderAvail;
	string currentOrderStatus;
	string nextStatus;
	try
	{
		sql::PreparedStatement* orderStmt = nullptr;
		orderStmt = database.prepareStatement(GET_ORDER_ID);
		orderStmt->setInt(1, postId);

		sql::ResultSet* orderRes;
		orderRes = orderStmt->executeQuery();

		if (orderRes->next()) {
			int order_id = orderRes->getInt("order_id");

			delete orderRes;
			delete orderStmt;
			isOrderAvail = true;
		}
		else {
			delete orderRes;
			delete orderStmt;
			isOrderAvail = false;
		}

		if (!isOrderAvail) {
			cout << "Order with post id '" << postId << "' does not exists" << endl;
			system("pause");
			system("cls");
			displaySellerDashboard();
		}

		sql::PreparedStatement* pstmt = nullptr;
		pstmt = database.prepareStatement(CURRENT_ORDER_STATUS);
		pstmt->setInt(1, postId);

		sql::ResultSet* res;
		res = pstmt->executeQuery();

		if (res->next()) {
			currentOrderStatus = res->getString("order_status");

			delete res;
			delete pstmt;
		}

		if (currentOrderStatus == "Placed") {
			nextStatus = "In Progress";
		}
		else if (currentOrderStatus == "In Progress") {
			nextStatus = "Completed";
		}
		else {
			cout << "Status is already '" << currentOrderStatus << "' cannot update it." << endl;
			system("pause");
			system("cls");
			displaySellerDashboard();
		}

		cout << "Current order status is set to: " << "'" << currentOrderStatus << "'" << endl;

		cout << "Do you want to update the status to '" << nextStatus << "'? (y/n) "; cin >> choice;
		if (choice == 'y') {
			sql::PreparedStatement* updateStmt = nullptr;
			updateStmt = database.prepareStatement(UPDATE_ORDER_STATUS);
			updateStmt->setString(1, nextStatus);
			updateStmt->setInt(2, postId);

			updateStmt->execute();

			delete updateStmt;

			cout << "Order Status Updated Successfully.";
		}
		else {
			cout << "Action altered, returning back to the main menu" << endl;
			system("pause");
			system("cls");
			displaySellerDashboard();
		}
	}
	catch (sql::SQLException& e)
	{
		cout << "Could not update order status. Error: " << e.what() << endl;
	}



}

#endif

