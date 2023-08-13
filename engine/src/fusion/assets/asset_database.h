#pragma once

extern "C" {
    typedef struct MDB_env MDB_env;
    typedef struct MDB_txn MDB_txn;
    typedef struct MDB_cursor MDB_cursor;
    typedef unsigned int MDB_dbi;
}

namespace fe {
    // Define available file changes
    enum class FileStatus : unsigned char { Created, Modified, Erased };

    class FUSION_API AssetDatabase {
    public:
        explicit AssetDatabase(fs::path path);
        ~AssetDatabase();

        std::optional<uuids::uuid> getKey(const fs::path& value);
        std::optional<fs::path> getValue(uuids::uuid key);

        bool put(uuids::uuid key, const fs::path& value, bool overwrite = false);

    private:
        MDB_env* env{ nullptr };
        MDB_dbi dbi{ 0 };
        fs::path databaseDirectory;
    };

    namespace mdb {
        class Transaction {
        public:
            Transaction(MDB_env* env, uint32_t flags = 0, MDB_txn* parent = nullptr);
            ~Transaction();

            operator bool() const { return txn != nullptr; }
            operator MDB_txn*() const { return txn; }

            MDB_txn* get() const { return txn; }

            void clear() { txn = nullptr; }

        private:
            MDB_txn* txn{ nullptr };
        };

        class Cursor {
        public:
            Cursor(MDB_txn* txn, MDB_dbi dbi);
            ~Cursor();

            operator bool() const { return cursor != nullptr; }
            operator MDB_cursor*() const { return cursor; }

            MDB_cursor* get() const { return cursor; }

            void clear() { cursor = nullptr; }

        private:
            MDB_cursor* cursor{ nullptr };
        };
    }
}