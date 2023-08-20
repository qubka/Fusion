#include "text_subrender.h"
#include "msdf.h"
#include "font.h"

#include "fusion/graphics/commands/command_buffer.h"
#include "fusion/scene/scene_manager.h"
#include "fusion/scene/components.h"

using namespace fe;

TextSubrender::TextSubrender(Pipeline::Stage pipelineStage)
    : Subrender{pipelineStage}
    , pipeline{pipelineStage,
               {FUSION_ASSET_PATH "shaders/text/text.vert", FUSION_ASSET_PATH "shaders/text/text.frag"},
               {{{Vertex::Component::Position, Vertex::Component::RGBA, Vertex::Component::Normal}}},
               {},
               PipelineGraphics::Mode::Polygon,
               PipelineGraphics::Depth::ReadWrite,
               VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
               VK_POLYGON_MODE_FILL,
               VK_CULL_MODE_FRONT_BIT,
               VK_FRONT_FACE_CLOCKWISE}
    , descriptorSet{pipeline} {
}

void TextSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    auto camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    auto& registry = scene->getRegistry();

    // Update uniforms
    pushObject.push("projection", camera->getProjectionMatrix());
    pushObject.push("view", camera->getViewMatrix());

    // Updates descriptors
    descriptorSet.push("PushObject", pushObject);
    //descriptorSet.push("samplerFont", fontAtlas.get());

    bindlessDescriptors.clear();

    auto view = registry.view<TextComponent>();

    float id = 0.0f;
    for (const auto& [entity, text]: view.each()) {
        if (text.font) if (bindlessDescriptors.emplace(text.font->getAtlasTexture(), id).second) ++id;
    }

    descriptorSet.push("textures", bindlessDescriptors.keys());

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushObject.bindPush(commandBuffer, pipeline);

    vertexBuffer.clear();
    indexBuffer.clear();

    auto group = registry.group<TextComponent>(entt::get<TransformComponent>);

    group.sort([&registry](const entt::entity a, const entt::entity b) {
        return registry.get<TextComponent>(a).font < registry.get<TextComponent>(b).font;
    });

    //const auto& frustum = camera->getFrustum();

    for (const auto& [entity, text, transform] : group.each()) {
        const auto& font = text.font;
        if (!font)
            continue;

        const auto& fontGeometry = font->getMSDFData()->fontGeometry;
        const auto& metrics = fontGeometry.getMetrics();
        auto fontAtlas = font->getAtlasTexture();
        auto& extent = fontAtlas->getExtent();

        const auto& string = text.text;

        glm::dvec2 o{0, 0};
        glm::vec2 texSize{ 1.0f / static_cast<float>(extent.width), 1.0f / static_cast<float>(extent.height) };

        double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
        const double spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

        for (size_t i = 0; i < string.size(); ++i) {
            char character = string[i];
            if (character == '\r')
                continue;

            if (character == '\n') {
                o.x = 0;
                o.y -= fsScale * metrics.lineHeight + text.lineSpacing;
                continue;
            }

            if (character == ' ') {
                double advance = spaceGlyphAdvance;
                if (i < string.size() - 1) {
                    char nextCharacter = string[i + 1];
                    fontGeometry.getAdvance(advance, character, nextCharacter);
                }
                o.x += fsScale * advance + text.kerning;
                continue;
            }

            if (character == '\t') {
                o.x += 4.0 * (fsScale * spaceGlyphAdvance + text.kerning);
                continue;
            }

            auto glyph = fontGeometry.getGlyph(character);
            if (!glyph) glyph = fontGeometry.getGlyph('?');
            if (!glyph)
                continue;

            double al, ab, ar, at;
            glyph->getQuadAtlasBounds(al, ab, ar, at);
            glm::vec2 texCoordMin{al, ab};
            glm::vec2 texCoordMax{ar, at};

            double pl, pb, pr, pt;
            glyph->getQuadPlaneBounds(pl, pb, pr, pt);
            glm::vec2 quadMin{pl, pb};
            glm::vec2 quadMax{pr, pt};

            quadMin *= fsScale, quadMax *= fsScale;
            quadMin += o, quadMax += o;

            texCoordMin *= texSize;
            texCoordMax *= texSize;

            // render here

            size_t firstIndex = vertexBuffer.size();

            indexBuffer.push_back(firstIndex);
            indexBuffer.push_back(firstIndex + 1);
            indexBuffer.push_back(firstIndex + 2);
            indexBuffer.push_back(firstIndex);
            indexBuffer.push_back(firstIndex + 2);
            indexBuffer.push_back(firstIndex + 3);

            uint32_t color = glm::rgbaColor(text.color);
            float texture = bindlessDescriptors[fontAtlas];

            const auto& worldMatrix = transform.getWorldMatrix();
            vertexBuffer.emplace_back(worldMatrix * glm::vec4{quadMin, 0.0f, 1.0f}, color, glm::vec3{texCoordMin, texture});
            vertexBuffer.emplace_back(worldMatrix * glm::vec4{quadMin.x, quadMax.y, 0.0f, 1.0f}, color, glm::vec3{texCoordMin.x, texCoordMax.y, texture});
            vertexBuffer.emplace_back(worldMatrix * glm::vec4{quadMax, 0.0f, 1.0f}, color, glm::vec3{texCoordMax, texture});
            vertexBuffer.emplace_back(worldMatrix * glm::vec4{quadMax.x, quadMin.y, 0.0f, 1.0f}, color, glm::vec3{texCoordMax.x, texCoordMin.y, texture});

            if (i < string.size() - 1) {
                double advance = glyph->getAdvance();
                char nextCharacter = string[i + 1];
                fontGeometry.getAdvance(advance, character, nextCharacter);

                o.x += fsScale * advance + text.kerning;
            }
        }
    }

    drawObject.cmdRender(commandBuffer, vertexBuffer, indexBuffer);
}
