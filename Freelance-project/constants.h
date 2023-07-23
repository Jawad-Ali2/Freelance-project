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
const string GET_USERNAME_BY_ID = "SELECT username FROM users WHERE id = ?";
const string GET_ID_BY_USERNAME = "SELECT id FROM users WHERE username = ?";
const string GET_ORDER_ID = "SELECT order_id FROM orders WHERE post_id = ?";
const string CURRENT_ORDER_STATUS = "SELECT order_status FROM orders WHERE post_id = ?";

const string LOGIN_QUERY = "SELECT * FROM users WHERE username = ? AND password = ?";
const string REGISTER_QUERY = "INSERT INTO users (username, email, password, credits, role) VALUES (?, ?, ?, ?, ?)";
const string GET_USER_ROLE = "SELECT role, id, credits FROM users WHERE username = ?";
const string CREATE_SELLER_POST = "INSERT INTO posts (seller_id, post_title, post_description, category, price) VALUES (?, ?, ?, ?, ?)";
const string GET_ALL_POSTS = "SELECT post_id, post_title, post_description, category, price FROM posts WHERE seller_id = ?";
const string GET_ACTIVE_ORDERS_SELLER = "SELECT * FROM orders WHERE seller_id = ?";
const string GET_POST_DETAILS = "SELECT * FROM posts WHERE post_id = ?";
const string GET_SELLER_POSTS = "SELECT * FROM posts WHERE category = ?";
const string INSERT_ORDER = "INSERT INTO orders (buyer_id, seller_id, post_id, order_status) VALUES (?, ?, ?, ?)";
const string GET_ACTIVE_ORDERS_BUYER = "SELECT * FROM orders WHERE buyer_id = ?";
const string GET_COMPLETED_ORDERS_SELLER = "SELECT post_id FROM orders WHERE order_status = ? AND seller_id = ?";
const string GET_COMPLETED_ORDERS_SELLER_BUYER = "SELECT post_id FROM orders WHERE order_status = ? AND buyer_id = ?";
const string GET_REJECTED_ORDERS = "SELECT post_id FROM orders WHERE order_status = ? AND seller_id = ?";
const string GET_USER_CREDS = "SELECT credits FROM users WHERE id = ?";

const string UPDATE_ORDER_STATUS = "UPDATE orders SET order_status = ? WHERE order_id = ?";
const string UPDATE_USER_CREDS = "UPDATE users SET credits = ? WHERE id = ?";

const string DELETE_REJECTED_ORDERS = "DELETE FROM orders WHERE order_id = ? AND order_status = 'Rejected'";
const string DELETE_ALL_POSTS = "DELETE FROM posts where seller_id = ?";
const string DELETE_ONE_POST = "DELETE FROM posts WHERE seller_id=? AND post_id=?";
const string DELETE_POST_FROM_ORDERS = "DELETE FROM orders WHERE seller_id=? AND post_id=?";

// Validation for inputs
const string VALIDATE_POST_ID = "SELECT post_id FROM posts WHERE post_id = ?";
const string CHECK_POST_IN_ACTIVE_ORDERS = "SELECT * FROM orders WHERE buyer_id = ? AND post_id = ? AND order_status IN ('Placed', 'In Progress')";

// Error Messages
const string ERROR_DB_CONNECTION = "Failed to connect to database";


#endif // !1
