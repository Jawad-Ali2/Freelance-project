#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include<string>
using namespace std;

// database details
const string DB_HOST = "tcp://127.0.0.1:3306";
const string DB_USERNAME = "root";
const string DB_PASSWORD = "jawadali123";
const string DB_NAME = "freelance";

// SQL queries
const string LOGIN_QUERY = "SELECT * FROM users WHERE username = ? AND password = ?";
const string REGISTER_QUERY = "INSERT INTO users (username, email, password, role) VALUES (?, ?, ?, ?)";
const string GET_USER_ROLE = "SELECT role, id FROM users WHERE username = ?";
const string CREATE_SELLER_POST = "INSERT INTO posts (seller_id, post_title, post_description) VALUES (?, ?, ?)";
const string GET_ALL_POSTS = "SELECT post_id, post_title, post_description FROM posts WHERE seller_id = ?";
const string GET_ACTIVE_ORDERS = "SELECT * FROM orders WHERE seller_id = ?";
const string GET_ORDER_POST_DETAILS = "SELECT * FROM posts WHERE id = ?";

// Error Messages
const string ERROR_DB_CONNECTION = "Failed to connect to database";


#endif // !1
