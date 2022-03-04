#include "database.hpp"

using namespace fe;

/*void Database::connect(const std::string& path) {
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

static int callback(void* context, int columnCount, char** columnValues, char** columnNames) {
    auto obj = reinterpret_cast<Database*>(context);

    std::vector<std::string> values;
    values.reserve(columnCount);
    std::vector<std::string> names;
    names.reserve(columnCount);

    for (int i = 0; i < columnCount; i++) {
        values.emplace_back(columnValues[i]);
        names.emplace_back(columnNames[i]);
    }

    return obj->onExec(values, names);
}

void Database::exec(const std::string& request) {
    int result = sqlite3_exec(db, request.c_str(), callback, this, nullptr);
    if (result) {
        LOG_ERROR << "DB Exec Error: " << sqlite3_errmsg(db);
    }
}*/

