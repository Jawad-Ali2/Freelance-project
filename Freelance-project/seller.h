#ifndef SELLER_H
#define SELLER_H

#include <iostream>
#include <string>
#include "login.h"
#include "constants.h"
#include "Database.h"
#include "User.h"
#include <cppconn/prepared_statement.h>

using namespace std;

class Seller : public User {
public:
	Seller(Database& db, const string& username, const string& role, const int& userId, const float& userCred);
	void displayDashboard() override;
	void displayPosts() override;

	void addPost(const int& userId);
	int countPosts();
	int countActiveOrders();
	void displayActiveOrders();
	void rejectOrders();
	int countCompletedOrders();
	void displayCompletedOrders();
	int countRejectedOrders();
	void displayRejectedOrders();
	string getUsernameById(int userId);
	void changeOrderStatus(int postId);
	void deletePosts(const int& userId);

	void reset() override;
	void logout() override;
};


Seller::Seller(Database& db, const string& username, const string& role, const int& userId, const float& userCred) : User(db, username, role, userId, userCred) {}

void Seller::displayDashboard() {
	cout << "Seller Dashboard\n" << endl;
	cout << "Welcome, " << username << "!\t\t\t\tCredits: " << creds << "\n\n";

	while (true) {
		int choice;

		cout << "1. Add Post" << endl;
		cout << "2. Display all your post (" << countPosts() << ")" << endl;
		cout << "3. Show active orders (" << countActiveOrders() << ")" << endl;
		cout << "4. Show completed orders (" << countCompletedOrders() << ")" << endl;
		cout << "5. Show rejected orders (" << countRejectedOrders() << ")" << endl;
		cout << "6. Log out" << endl;
		cin >> choice;

		if (choice == 1) {
			system("cls");
			addPost(userId);
		}
		else if (choice == 2) {
			system("cls");
			displayPosts();
		}
		else if (choice == 3) {
			system("cls");
			displayActiveOrders();
		}
		else if (choice == 4) {
			system("cls");
			displayCompletedOrders();
		}
		else if (choice == 5) {
			system("cls");
			displayRejectedOrders();
		}
		else if (choice == 6) {
			reset();
			logout();
			system("cls");
			cout << "\n\n\n\t\t\t\t\tLogged Out Successfully!" << endl;

			cout << "\n\n\n\t\t\t\tGoing back to the main screen" << endl;
			system("pause");
			system("cls");
			return;
		}
		else {
			cout << "Invalid input!" << endl;
		}
	}

}

void Seller::addPost(const int& userId) {

	string postTitle;
	string postDescription;
	string category;
	float price = 0.00;

	cin.ignore();
	cout << "Add Post" << endl;
	cout << "Enter Post Category: (i.e. Website, Content-Writing etc.): ";
	getline(cin, category);
	cout << "Price: ";cin >> price;
	cin.ignore();
	cout << "Post Title: ";
	getline(cin, postTitle);
	cout << "Post Description: ";
	getline(cin, postDescription);

	try
	{
		sql::PreparedStatement* pstmt;
		pstmt = database.prepareStatement(CREATE_SELLER_POST);
		pstmt->setInt(1, userId);
		pstmt->setString(2, postTitle);
		pstmt->setString(3, postDescription);
		pstmt->setString(4, category);
		pstmt->setDouble(5, price);

		pstmt->execute();

		cout << "Post added successfully" << endl;
	}
	catch (sql::SQLException& e)
	{
		cout << "Failed to create post. Error: " << e.what() << endl;
	}

}

int Seller::countPosts() {
	int postsCount = 0;
	try
	{
		sql::PreparedStatement* pstmt;
		// Get all the posts
		pstmt = database.prepareStatement(GET_ALL_POSTS);
		pstmt->setInt(1, userId); // of the seller with id

		sql::ResultSet* res;
		res = pstmt->executeQuery();
		if (res->next()) {
			do {
				int postId = res->getInt("post_id");
				string postTitle = res->getString("post_title");
				string postDesc = res->getString("post_description");

				postsCount++;
			} while (res->next());

			delete res;
			delete pstmt;
			return postsCount;
		}
	}
	catch (sql::SQLException& e)
	{
		cout << "Failed to fetch posts. Error: " << e.what() << endl;
	}
}

void Seller::displayPosts() {
	try
	{
		sql::PreparedStatement* pstmt;
		// Get all the posts
		pstmt = database.prepareStatement(GET_ALL_POSTS);
		pstmt->setInt(1, userId); // of the seller with id

		sql::ResultSet* res;
		res = pstmt->executeQuery();
		if (res->next()) {
			cout << "Your Posts: \n" << endl;

			do {
				int postId = res->getInt("post_id");
				string postTitle = res->getString("post_title");
				string postDesc = res->getString("post_description");
				string postCat = res->getString("category");
				float postPrice = res->getDouble("price");


				cout << "Post ID: " << postId << endl;
				cout << "Price: " << postPrice << endl;
				cout << "Category: " << postCat << endl;
				cout << "Title: " << postTitle << endl;
				cout << "Description: " << postDesc << endl;
				cout << "-----------------------------------------" << endl;
				cout << endl;
			} while (res->next());

			delete res;
			delete pstmt;

			char choice;
			cout << "1. Do you want to delete post? (y/n)\n"
				"2. Go Back\n"
				"Enter your choice: ";
			cin >> choice;

			if (choice == '1') {
				deletePosts(userId);
			}
			else if (choice == '2') {
				system("cls");
				return;
			}
			else {
				cout << "Invalid input! Returning to main screen" << endl;
				system("pause");
				return;
			}

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

int Seller::countActiveOrders() {

	// Get the count of active orders
	int activeOrders = 0;
	try
	{
		sql::PreparedStatement* pstmt;
		// Get Active_orders from the database based on the id
		pstmt = database.prepareStatement(GET_ACTIVE_ORDERS_SELLER);
		pstmt->setInt(1, userId); // Provides the id of the user

		sql::ResultSet* res;
		res = pstmt->executeQuery();

		if (res->next()) {

			do {
				int orderId = res->getInt("order_id");
				int buyerId = res->getInt("buyer_id");
				int postId = res->getInt("post_id");
				// If the order status is 'Completed' we move that post to completed orders
				string orderStatus = res->getString("order_status");
				string buyerName = getUsernameById(buyerId);


				// Skips the post which has status completed
				if (orderStatus == "Completed" || orderStatus == "Rejected") {
					//displayCompletedOrders();
					continue;
				}

				// Fetch  Post details
				sql::PreparedStatement* postStmt = database.prepareStatement(GET_POST_DETAILS);
				postStmt->setInt(1, postId);
				sql::ResultSet* postRes = postStmt->executeQuery();

				if (postRes->next()) {
					string postTitle = postRes->getString("post_title");
					string postDescription = postRes->getString("post_description");
					string postCat = postRes->getString("category");

					activeOrders++;
				}
				delete postRes;
				delete postStmt;

			} while (res->next());;
		}
		delete res;
		delete pstmt;

		return activeOrders;
	}
	catch (sql::SQLException& e)
	{
		cout << "Couldn't get active orders. Error: " << e.what() << endl;
		return 404;
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
		pstmt->setInt(1, userId); // Provides the id of the user

		sql::ResultSet* res;
		res = pstmt->executeQuery();

		if (res->next()) {
			cout << "Active Orders" << endl;

			do {
				int orderId = res->getInt("order_id");
				int buyerId = res->getInt("buyer_id");
				int postId = res->getInt("post_id");

				// If the order status is 'Completed' we move that post to completed orders
				string orderStatus = res->getString("order_status");
				string buyerName = getUsernameById(buyerId);


				// Skips the post which has status completed
				if (orderStatus == "Completed" || orderStatus == "Rejected") {
					//displayCompletedOrders();
					continue;
				}

				// Fetch  Post details
				sql::PreparedStatement* postStmt = database.prepareStatement(GET_POST_DETAILS);
				postStmt->setInt(1, postId);
				sql::ResultSet* postRes = postStmt->executeQuery();

				if (postRes->next()) {
					string postTitle = postRes->getString("post_title");
					string postDescription = postRes->getString("post_description");
					string postCat = postRes->getString("category");
					float postPrice = postRes->getDouble("price");

					cout << "Order ID: " << orderId << endl;
					cout << "Buyer: " << buyerName << endl;
					cout << "Post ID: " << postId << endl;
					cout << "Price: " << postPrice << endl;
					cout << "Category: " << postCat << endl;
					cout << "Post Title: \n" << postTitle << endl;
					cout << "Post Description: \n" << postDescription << endl;
					cout << "Order Status: " << orderStatus << endl;
					cout << "-----------------------------------------" << endl;
				}
				delete postRes;
				delete postStmt;

			} while (res->next());
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
				rejectOrders();
			}
			else {
				displayDashboard();
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

void Seller::rejectOrders() {
	int order_Id;
	cout << "Enter the Pending Order ID you want to reject: "; cin >> order_Id;
	try
	{
		sql::PreparedStatement* pstmt = nullptr;
		pstmt = database.prepareStatement(GET_ACTIVE_ORDERS_SELLER);
		pstmt->setInt(1, userId);

		sql::ResultSet* res;
		res = pstmt->executeQuery();

		while (res->next()) {
			string orderStatus = res->getString("order_status");
			int orderId = res->getInt("order_id");
			int postId = res->getInt("post_id");
			int buyerId = res->getInt("buyer_id");

			string buyerName = getUsernameById(buyerId);


			if (order_Id == orderId && orderStatus == "Placed") {
				// Display that post here if order status is placed:
				if (orderStatus != "Placed") {
					cout << "You can only reject Pending orders!" << endl;
					system("pause");
					displayDashboard();
				}

				// Fetch  Post details
				sql::PreparedStatement* postStmt = database.prepareStatement(GET_POST_DETAILS);
				postStmt->setInt(1, postId);
				sql::ResultSet* postRes = postStmt->executeQuery();

				if (postRes->next()) {
					string postTitle = postRes->getString("post_title");
					string postDescription = postRes->getString("post_description");
					string postCat = postRes->getString("category");
					float postPrice = postRes->getDouble("price");


					//cout << "Buyer ID: " << buyerId << endl;
					cout << "\n\nOrder ID: " << orderId << endl;
					cout << "Buyer: " << buyerName << endl;
					cout << "Post ID: " << postId << endl;
					cout << "Price: " << postPrice << endl;
					cout << "Category: " << postCat << endl;
					cout << "Post Title: \n" << postTitle << endl;
					cout << "Post Description: \n" << postDescription << endl;
					cout << "Order Status: " << orderStatus << endl;
					cout << "-----------------------------------------" << endl;

				}
				delete postRes;
				delete postStmt;

				char choice;
				cout << "Do you wish to reject this order? (y/n) "; cin >> choice;

				if (choice == 'y') {
					string updateStatus = "Rejected";
					sql::PreparedStatement* rejStmt = nullptr;
					rejStmt = database.prepareStatement(UPDATE_ORDER_STATUS);
					rejStmt->setString(1, updateStatus);
					rejStmt->setInt(2, orderId);
					rejStmt->executeUpdate();

					delete rejStmt;
					cout << "Order Successfully Rejected." << endl;

					break;
				}
			}
		}

		delete res;
		delete pstmt;

	}
	catch (sql::SQLException& e)
	{
		cout << "Could not reject the order. Error: " << e.what() << endl;
	}
}

int Seller::countCompletedOrders() {
	int completedOrders = 0;

	string orderStatus;
	int orderId;
	int buyerId;
	string buyerName;
	int postId;
	try
	{
		// First we get order status and buyer name from this query
		sql::PreparedStatement* preStmt = nullptr;
		preStmt = database.prepareStatement(GET_ACTIVE_ORDERS_SELLER);
		preStmt->setInt(1, userId);

		sql::ResultSet* preRes;
		preRes = preStmt->executeQuery();

		while (preRes->next()) {
			orderId = preRes->getInt("order_id");
			orderStatus = preRes->getString("order_status");
			buyerId = preRes->getInt("buyer_id");
			buyerName = getUsernameById(buyerId);

			// Skips the post which does not have status of completed
			if (orderStatus != "Completed") {
				continue;
			}
			// Then we get post id of the status that are completed
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(GET_COMPLETED_ORDERS_SELLER);
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
			sql::PreparedStatement* postStmt = database.prepareStatement(GET_POST_DETAILS);
			postStmt->setInt(1, postId);
			sql::ResultSet* postRes = postStmt->executeQuery();


			if (postRes->next()) {
				string postTitle = postRes->getString("post_title");
				string postDescription = postRes->getString("post_description");
				string postCat = postRes->getString("category");

				completedOrders++;

			}
			delete postRes;
			delete postStmt;
		}
		delete preRes;
		delete preStmt;

		return completedOrders;
	}
	catch (sql::SQLException& e)
	{
		cout << "Could not get completed orders. Error:" << e.what() << endl;
		return 404;
	}
}

void Seller::displayCompletedOrders() {
	string orderStatus;
	int orderId;
	int buyerId;
	string buyerName;
	int postId;
	try
	{
		// First we get order status and buyer name from this query
		sql::PreparedStatement* preStmt = nullptr;
		preStmt = database.prepareStatement(GET_ACTIVE_ORDERS_SELLER);
		preStmt->setInt(1, userId);

		sql::ResultSet* preRes;
		preRes = preStmt->executeQuery();

		while (preRes->next()) {
			orderId = preRes->getInt("order_id");
			orderStatus = preRes->getString("order_status");
			buyerId = preRes->getInt("buyer_id");
			buyerName = getUsernameById(buyerId);

			// Skips the post which does not have status of completed
			if (orderStatus != "Completed") {
				continue;
			}
			// Then we get post id of the status that are completed
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(GET_COMPLETED_ORDERS_SELLER);
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
			sql::PreparedStatement* postStmt = database.prepareStatement(GET_POST_DETAILS);
			postStmt->setInt(1, postId);
			sql::ResultSet* postRes = postStmt->executeQuery();


			if (postRes->next()) {
				string postTitle = postRes->getString("post_title");
				string postDescription = postRes->getString("post_description");
				string postCat = postRes->getString("category");
				float postPrice = postRes->getDouble("price");

				cout << "\n\nOrder ID: " << orderId << endl;
				cout << "Buyer: " << buyerName << endl;
				cout << "Post ID: " << postId << endl;
				cout << "Price: " << postPrice << endl;
				cout << "Category: " << postCat << endl;
				cout << "Post Title: \n" << postTitle << endl;
				cout << "Post Description: \n" << postDescription << endl;
				cout << "Order Status: " << orderStatus << endl;
				cout << "-----------------------------------------" << endl;

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



int Seller::countRejectedOrders() {
	int rejectedOrders = 0;

	string orderStatus;
	int orderId;
	int buyerId;
	string buyerName;
	int postId;
	try
	{
		// First we get order status and buyer name from this query
		sql::PreparedStatement* preStmt = nullptr;
		preStmt = database.prepareStatement(GET_ACTIVE_ORDERS_SELLER);
		preStmt->setInt(1, userId);

		sql::ResultSet* preRes;
		preRes = preStmt->executeQuery();

		while (preRes->next()) {
			orderId = preRes->getInt("order_id");
			orderStatus = preRes->getString("order_status");
			buyerId = preRes->getInt("buyer_id");
			buyerName = getUsernameById(buyerId);

			// Skips the post which does not have status of rejected
			if (orderStatus != "Rejected") {
				continue;
			}
			// Then we get post id of the status that are completed
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(GET_COMPLETED_ORDERS_SELLER);
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
			sql::PreparedStatement* postStmt = database.prepareStatement(GET_POST_DETAILS);
			postStmt->setInt(1, postId);
			sql::ResultSet* postRes = postStmt->executeQuery();


			if (postRes->next()) {
				string postTitle = postRes->getString("post_title");
				string postDescription = postRes->getString("post_description");

				rejectedOrders++;
			}
			delete postRes;
			delete postStmt;
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

void Seller::displayRejectedOrders() {
	string orderStatus;
	int orderId;
	int buyerId;
	string buyerName;
	int postId;
	try
	{
		// First we get order status and buyer name from this query
		sql::PreparedStatement* preStmt = nullptr;
		preStmt = database.prepareStatement(GET_ACTIVE_ORDERS_SELLER);
		preStmt->setInt(1, userId);

		sql::ResultSet* preRes;
		preRes = preStmt->executeQuery();

		while (preRes->next()) {
			orderId = preRes->getInt("order_id");
			orderStatus = preRes->getString("order_status");
			buyerId = preRes->getInt("buyer_id");
			buyerName = getUsernameById(buyerId);

			// Skips the post which does not have status of rejected
			if (orderStatus != "Rejected") {
				continue;
			}
			// Then we get post id of the status that are completed
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(GET_COMPLETED_ORDERS_SELLER);
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
			sql::PreparedStatement* postStmt = database.prepareStatement(GET_POST_DETAILS);
			postStmt->setInt(1, postId);
			sql::ResultSet* postRes = postStmt->executeQuery();


			if (postRes->next()) {
				string postTitle = postRes->getString("post_title");
				string postDescription = postRes->getString("post_description");
				string postCat = postRes->getString("category");
				float postPrice = postRes->getDouble("price");

				cout << "\n\nOrder ID: " << orderId << endl;
				cout << "Buyer: " << buyerName << endl;
				cout << "Post ID: " << postId << endl;
				cout << "Price: " << postPrice << endl;
				cout << "Category: " << postCat << endl;
				cout << "Post Title: \n" << postTitle << endl;
				cout << "Post Description: \n" << postDescription << endl;
				cout << "Order Status: " << orderStatus << endl;
				cout << "-----------------------------------------" << endl;

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
			displayDashboard();
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
			displayDashboard();
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

			cout << "Order Status Updated Successfully." << endl;
		}
		else {
			cout << "Action altered, returning back to the main menu" << endl;
			system("pause");
			system("cls");
			displayDashboard();
		}
	}
	catch (sql::SQLException& e)
	{
		cout << "Could not update order status. Error: " << e.what() << endl;
	}

}

// Done by Noraiz

void Seller::deletePosts(const int& userId) {
	char choice;

	cout << "1. Delete one post\n"
		"2. Return To main menu\n"
		"Enter you choice: ";
	cin >> choice;

	if (choice == '1') {
		try {
			string postId;
			cout << "NOTE: You may lose the orders if any on current post you may select." << endl;
			cout << "Enter the Post ID Which you want to delete: ";
			cin >> postId;

			// First delete that post from posts table
			sql::PreparedStatement* pstmt = nullptr;
			pstmt = database.prepareStatement(DELETE_POST_FROM_ORDERS);
			pstmt->setInt(1, userId);
			pstmt->setString(2, postId);

			pstmt->executeUpdate();
			delete pstmt;

			// Then delete that post from orders table too
			pstmt = database.prepareStatement(DELETE_ONE_POST);
			pstmt->setInt(1, userId);
			pstmt->setString(2, postId);

			pstmt->executeUpdate();
			delete pstmt;


			cout << "Post deleted successfully" << endl;
		}
		catch (sql::SQLException& e)
		{
			cout << "Failed to delete post. Error: " << e.what() << endl;
		}

	}
	else {
		cout << "Invalid Input. Returning to main menu";
		system("cls");
		deletePosts(userId);
	}
}

void Seller::reset() {
	int userId = 0;
	string role = "";
	string username = "";
}

void Seller::logout() {
	isLoggedIn = false;
}


#endif

