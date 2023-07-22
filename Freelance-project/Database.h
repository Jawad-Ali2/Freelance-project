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



