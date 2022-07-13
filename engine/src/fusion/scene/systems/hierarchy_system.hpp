#pragma once

namespace fe {
    class hierarchy_system {
    public:
        hierarchy_system();
        ~hierarchy_system();
        hierarchy_system(const hierarchy_system&) = delete;
        hierarchy_system(hierarchy_system&&) = delete;
        hierarchy_system& operator=(const hierarchy_system&) = delete;
        hierarchy_system& operator=(hierarchy_system&&) = delete;
    private:
    };
}
