#pragma once

namespace fe {
    /**
     * @brief A managed resource object.
     */
    class Resource {
    public:
        Resource() = default;
        virtual ~Resource() = default;
        NONCOPYABLE(Resource);

        virtual type_index getTypeIndex() const = 0;
    };
}