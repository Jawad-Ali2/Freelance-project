#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include "constants.h"

using namespace std;

class Database {
private:
	sql::mysql::MySQL_Driver* driver;
	sql::Connection* connection;
public:
	Database() = default;
	Database(const string& host, const string& user, const string& password, const string& database);
	~Database();

	bool connect(const string& host, const string& user, const string& password, const string& database);
	sql::PreparedStatement* prepareStatement(const std::string& query);
	void close();
};


Database::Database(const string& host, const string& user, const string& password, const string& database) {
	driver = sql::mysql::get_mysql_driver_instance();


	connection = driver->connect(host, user, password);
	connection->setSchema(database);
}

Database:: ~Database() {
	// Close the connection before destroying the object
	delete connection;
}

bool Database::connect(const string& host, const string& user, const string& password, const string& database) {
	try {
		// Initialize the MySQL driver
		driver = sql::mysql::get_mysql_driver_instance();

		// Connect to the database
		connection = driver->connect(host, user, password);

		// Set the current schema (database)
		connection->setSchema(database);

		// Return true if the connection is successful
		return true;
	}
	catch (sql::SQLException& e) {
		// If there is an error in the connection, catch the exception and print the error message
		cout << "Failed to connect to the database. Error: " << e.what() << endl;
		return false;
	}
}

sql::PreparedStatement* Database::prepareStatement(const string& query) {
	if (connection) {
		return connection->prepareStatement(query);

	}
	else {
		cout << ERROR_DB_CONNECTION << endl;
		return nullptr;
	}
}

void Database::close() {
	connection->close();
}


#endif



