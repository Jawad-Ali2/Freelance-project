#include "Database.h"
#include "constants.h"
using namespace std;

Database::Database(const string& host, const string& user, const string& password, const string& database) {
	driver = sql::mysql::get_mysql_driver_instance();


	connection = driver->connect(host, user, password);
	setSchema(database);
}

Database:: ~Database() {
	// Close the connection before destroying the object
	delete connection;
}

void Database::setSchema(const std::string& schema) {
	string currentSchema = schema;
	connection->setSchema(schema);
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

