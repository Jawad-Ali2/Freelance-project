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
	string getUsernameById(int userId);
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

			while (true) {
				int orderId = res->getInt("order_id");
				int buyerId = res->getInt("buyer_id");
				int postId = res->getInt("post_id");
				string orderStatus = res->getString("order_status");
				string buyerName = getUsernameById(buyerId);


				// Fetch  Post details
				sql::PreparedStatement* postStmt = database.prepareStatement(GET_ORDER_POST_DETAILS);
				postStmt->setInt(1, postId);
				sql::ResultSet* postRes = postStmt->executeQuery();

				if (postRes->next()) {
					string postTitle = res->getString("post_title");
					string postDescription = res->getString("post_description");

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
				// Todo: Marks the order as complete (manage them etc.)
				if (!res->next()) {
					break; // Breaks out of the while loop
				}
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

#endif

