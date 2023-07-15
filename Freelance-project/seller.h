#ifndef SELLER_H
#define SELLER_H

#include <iostream>
#include <string>
#include "constants.h"
#include "Database.h"

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
};

#endif

