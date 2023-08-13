#include "asset_database.h"

#include <lmdb/libraries/liblmdb/lmdb.h>

using namespace fe;

#define MDB_RESULT(x) if (const int err = x) { char* error = mdb_strerror(err); FE_LOG_ERROR(error); throw std::runtime_error(error); }

AssetDatabase::AssetDatabase(fs::path path) : databaseDirectory{std::move(path)} {
    if (mdb_env_create(&env))
        throw std::runtime_error("Cant instantiate LMDB environment!");

    constexpr size_t size = 1048576UL * 100000UL; // 1MB * 100000
    mdb_env_set_maxdbs(env, 1);
    mdb_env_set_mapsize(env, size);

    if (const int rc = mdb_env_open(env, databaseDirectory.string().c_str(), MDB_NOTLS, 0664)) {
        mdb_env_close(env);
        throw std::runtime_error(fmt::format("Could not load '{}' database. Error: {}", databaseDirectory, mdb_strerror(rc)));
    }
}

AssetDatabase::~AssetDatabase() {
    if (dbi != 0)
        mdb_dbi_close(env, dbi);
    if (env)
        mdb_env_close(env);
}

std::optional<uuids::uuid> AssetDatabase::getKey(const fs::path& filepath) {
    if (filepath.empty()) {
        FE_LOG_ERROR("AssetDatabase: value is empty!");
        return std::nullopt;
    }

    mdb::Transaction txn{env, MDB_RDONLY};
    mdb_dbi_open(txn, nullptr, 0, &dbi);

    mdb::Cursor cursor{txn, dbi};

    MDB_val key{ 0, nullptr };
    MDB_val value{ 0, nullptr };

    std::string pathStr{ filepath.generic_string() };

    while (mdb_cursor_get(cursor, &key, &value, MDB_NEXT) == 0) {
        std::string_view path{static_cast<char*>(value.mv_data), value.mv_size};

        if (pathStr == path) {
            gsl::span<uint8_t, 16> bytes{ static_cast<uint8_t*>(key.mv_data), key.mv_size };
            return { uuids::uuid{ bytes } };
        }
    }

    return std::nullopt;
}

std::optional<fs::path> AssetDatabase::getValue(uuids::uuid uuid) {
    if (uuid.is_nil()) {
        FE_LOG_ERROR("AssetDatabase: key is empty!");
        return std::nullopt;
    }

    mdb::Transaction txn{env, MDB_RDONLY};
    mdb_dbi_open(txn, nullptr, 0, &dbi);

    auto bytes = uuid.as_bytes();
    MDB_val key{bytes.size(), (void*)bytes.data()};
    MDB_val value;

    const int rc = mdb_get(txn, dbi, &key, &value);
    if (rc != 0)
        return std::nullopt;

    fs::path path{ std::string_view{static_cast<char*>(value.mv_data), value.mv_size} };
    path.make_preferred();
    return { std::move(path) };
}

bool AssetDatabase::put(uuids::uuid uuid, const fs::path& filepath, bool overwrite) {
    if (uuid.is_nil() || filepath.empty()) {
        FE_LOG_ERROR("AssetDatabase: key/value is empty!");
        return false;
    }

    mdb::Transaction txn{env};
    mdb_dbi_open(txn, nullptr, 0, &dbi);

    auto bytes = uuid.as_bytes();
    MDB_val key{bytes.size(), (void*)bytes.data()};

    std::string pathStr{ filepath.generic_string() };
    MDB_val value{ pathStr.size(), (void*) pathStr.c_str() };

    MDB_RESULT(mdb_put(txn, dbi, &key, &value, overwrite ? 0 : MDB_NOOVERWRITE));

    mdb_txn_commit(txn);

    txn.clear();

    return true;
}

namespace fe::mdb {
    Transaction::Transaction(MDB_env* env, uint32_t flags, MDB_txn* parent) {
        MDB_RESULT(mdb_txn_begin(env, nullptr, flags, &txn));
    }
    Transaction::~Transaction() {
        if (txn)
            mdb_txn_abort(txn);
    }

    Cursor::Cursor(MDB_txn* txn, MDB_dbi dbi) {
        MDB_RESULT(mdb_cursor_open(txn, dbi, &cursor));
    }
    Cursor::~Cursor() {
        if (cursor)
            mdb_cursor_close(cursor);
    }
}