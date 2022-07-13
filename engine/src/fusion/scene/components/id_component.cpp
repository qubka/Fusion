#include "id_component.hpp"

namespace fe {
    static std::random_device RandomDevice;
    static std::mt19937 Engine(RandomDevice());
    static uuids::uuid_random_generator Generator{Engine};

    IdComponent::IdComponent() : uuid{Generator()} {
    }
}
