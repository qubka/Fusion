#pragma once

#include <sqlite3.h>

namespace fe {
    class Database {
    public:
        Database() = default;
        ~Database() = default;

        void connect(const std::string& path);
        void close();


    private:
        sqlite3* db;
    };
}
