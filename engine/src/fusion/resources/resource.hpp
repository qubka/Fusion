#pragma once

namespace fe {
    /**
     * @brief A managed resource object.
     */
    class Resource {
    public:
        Resource() = default;
        virtual ~Resource() = default;
        NONCOPYBLE(Resource);

        virtual type_index getTypeIndex() const = 0;
    };
}