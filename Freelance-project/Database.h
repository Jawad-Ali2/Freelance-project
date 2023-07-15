#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <mysql_driver.h>
#include <mysql_connection.h>

using namespace std;

class Database {
private:
	sql::mysql::MySQL_Driver* driver;
	sql::Connection* connection;
public:
	Database(const string& host, const string& user, const string& password, const string& database);
	~Database();

	bool connect(const string& host, const string& user, const string& password, const string& database);
	void setSchema(const std::string& schema);
	sql::PreparedStatement* prepareStatement(const std::string& query);
	void close();
};

#endif



