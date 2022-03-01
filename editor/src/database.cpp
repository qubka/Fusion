#include "database.hpp"

using namespace fe;

void Database::connect(const std::string& path) {
    // Save the connection result
    int exit = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

    // Test if there was an error
    if (exit) {
        LOG_ERROR << "DB Open Error: " << sqlite3_errmsg(db);
        sqlite3_close(db);
    } else {
        LOG_INFO << "Opened DB: \"" << path << "\" Successfully!";
    }
}

void Database::close() {

}

void Database::create(const std::string& request) {

}
