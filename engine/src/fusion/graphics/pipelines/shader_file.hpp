#pragma once

#include "fusion/assets/asset.hpp"

namespace fe {
    class ShaderFile : public Asset {
    public:
        explicit ShaderFile(const fs::path& filepath);
        ~ShaderFile() override = default;

        type_index getType() const override { return type_id<ShaderFile>; }

        const std::string& getCode() const { return code; }
        VkShaderStageFlagBits getStage() const { return stage; }

    private:
        std::string code;
        VkShaderStageFlagBits stage{ VK_SHADER_STAGE_ALL };
    };
}
