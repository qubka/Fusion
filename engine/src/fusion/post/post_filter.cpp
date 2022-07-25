#include "post_filter.hpp"

using namespace fe;

PostFilter::PostFilter(const Pipeline::Stage& pipelineStage, const std::vector<fs::path>& shaderStages, const std::vector<Shader::Define>& defines)
        : Subrender{pipelineStage}
        , pipeline{pipelineStage, shaderStages, {}, defines, PipelineGraphics::Mode::Polygon, PipelineGraphics::Depth::None} {
}

const Descriptor* PostFilter::getAttachment(const std::string& descriptorName, const Descriptor* descriptor) const {
    if (auto it = attachments.find(descriptorName); it != attachments.end())
        return it->second;
    return descriptor;
}

const Descriptor* PostFilter::getAttachment(const std::string& descriptorName, const std::string& rendererAttachment) const {
    if (auto it = attachments.find(descriptorName); it != attachments.end())
        return it->second;
    return Graphics::Get()->getAttachment(rendererAttachment);
}

void PostFilter::setAttachment(const std::string& descriptorName, const Descriptor* descriptor) {
    if (auto it = attachments.find(descriptorName); it != attachments.end())
        it->second = descriptor;
    else
        attachments.emplace(descriptorName, descriptor);
}

bool PostFilter::removeAttachment(const std::string& name) {
    if (auto it = attachments.find(name); it != attachments.end()) {
        attachments.erase(it);
        return true;
    }
    return false;
}

void PostFilter::pushConditional(const std::string& descriptorName1, const std::string& descriptorName2, const std::string& rendererAttachment1, const std::string& rendererAttachment2) {
    // TODO: Clean up this state machine mess, this logic may also be incorrect.
    auto it1 = attachments.find(descriptorName1);
    auto it2 = attachments.find(descriptorName2);

    if (it1 != attachments.end() && it2 != attachments.end()) {
        descriptorSet.push(descriptorName1, getAttachment(descriptorName1, rendererAttachment1));
        descriptorSet.push(descriptorName2, getAttachment(descriptorName2, rendererAttachment1));
        return;
    }
    if (it1 == attachments.end() && it2 != attachments.end()) {
        descriptorSet.push(descriptorName1, Graphics::Get()->getAttachment(GlobalSwitching % 2 == 1 ? rendererAttachment1 : rendererAttachment2));
        descriptorSet.push(descriptorName2, getAttachment(descriptorName2, rendererAttachment1));
        return;
    }
    if (it1 != attachments.end() && it2 == attachments.end()) {
        descriptorSet.push(descriptorName1, getAttachment(descriptorName1, rendererAttachment1));
        descriptorSet.push(descriptorName2, Graphics::Get()->getAttachment(GlobalSwitching % 2 == 1 ? rendererAttachment1 : rendererAttachment2));
        return;
    }

    if (GlobalSwitching % 2 == 1) {
        descriptorSet.push(descriptorName1, Graphics::Get()->getAttachment(rendererAttachment1));
        descriptorSet.push(descriptorName2, Graphics::Get()->getAttachment(rendererAttachment2));
    } else {
        descriptorSet.push(descriptorName1, Graphics::Get()->getAttachment(rendererAttachment2));
        descriptorSet.push(descriptorName2, Graphics::Get()->getAttachment(rendererAttachment1));
    }

    GlobalSwitching++;
}