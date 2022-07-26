#include "mesh_factory.hpp"
#include "mesh.hpp"

using namespace fe;
/*
void appendVertex(std::vector<uint8_t>& outputBuffer, const Vertex::Layout& layout, const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv) {
    
}

std::shared_ptr<Mesh> MeshFactory::CreateCuboid(const Vertex::Layout& layout, const glm::vec3& halfExtents, bool inwards) {
    float orientation = 1;
    if (inwards)
        orientation = -1;

    std::vector<uint8_t> vertices;
    vertices.reserve(layout.getStride() * 24);
    {
         //front
         appendVertex(vertices, layout, {-1.0f * halfExtents.x, -1.0f * halfExtents.y,  1.0f * halfExtents.z }, { 0.0f, 0.0f,  1.0f * orientation}, { 0.0f, 0.0f });
         appendVertex(vertices, layout, { 1.0f * halfExtents.x, -1.0f * halfExtents.y,  1.0f * halfExtents.z }, { 0.0f, 0.0f,  1.0f * orientation}, { 1.0f, 0.0f });
         appendVertex(vertices, layout, { 1.0f * halfExtents.x,  1.0f * halfExtents.y,  1.0f * halfExtents.z }, { 0.0f, 0.0f,  1.0f * orientation}, { 1.0f, 1.0f });
         appendVertex(vertices, layout, {-1.0f * halfExtents.x,  1.0f * halfExtents.y,  1.0f * halfExtents.z }, { 0.0f, 0.0f,  1.0f * orientation}, { 0.0f, 1.0f });

         // right
         appendVertex(vertices, layout, { 1.0f * halfExtents.x, -1.0f * halfExtents.y,  1.0f * halfExtents.z }, { 1.0f * orientation, 0.0f, 0.0f }, { 0.0f, 0.0f });
         appendVertex(vertices, layout, { 1.0f * halfExtents.x, -1.0f * halfExtents.y, -1.0f * halfExtents.z }, { 1.0f * orientation, 0.0f, 0.0f }, { 1.0f, 0.0f });
         appendVertex(vertices, layout, { 1.0f * halfExtents.x,  1.0f * halfExtents.y, -1.0f * halfExtents.z }, { 1.0f * orientation, 0.0f, 0.0f }, { 1.0f, 1.0f });
         appendVertex(vertices, layout, { 1.0f * halfExtents.x,  1.0f * halfExtents.y,  1.0f * halfExtents.z }, { 1.0f * orientation, 0.0f, 0.0f }, { 0.0f, 1.0f });

         // back
         appendVertex(vertices, layout, { 1.0f * halfExtents.x, -1.0f * halfExtents.y, -1.0f * halfExtents.z }, { 0.0f, 0.0f, -1.0f * orientation}, { 0.0f, 0.0f });
         appendVertex(vertices, layout, {-1.0f * halfExtents.x, -1.0f * halfExtents.y, -1.0f * halfExtents.z }, { 0.0f, 0.0f, -1.0f * orientation}, { 1.0f, 0.0f });
         appendVertex(vertices, layout, {-1.0f * halfExtents.x,  1.0f * halfExtents.y, -1.0f * halfExtents.z }, { 0.0f, 0.0f, -1.0f * orientation}, { 1.0f, 1.0f });
         appendVertex(vertices, layout, { 1.0f * halfExtents.x,  1.0f * halfExtents.y, -1.0f * halfExtents.z }, { 0.0f, 0.0f, -1.0f * orientation}, { 0.0f, 1.0f });

         // left
         appendVertex(vertices, layout, {-1.0f * halfExtents.x, -1.0f * halfExtents.y, -1.0f * halfExtents.z }, {-1.0f * orientation, 0.0f, 0.0f },	{ 0.0f, 0.0f });
         appendVertex(vertices, layout, {-1.0f * halfExtents.x, -1.0f * halfExtents.y,  1.0f * halfExtents.z }, {-1.0f * orientation, 0.0f, 0.0f },	{ 1.0f, 0.0f });
         appendVertex(vertices, layout, {-1.0f * halfExtents.x,  1.0f * halfExtents.y,  1.0f * halfExtents.z }, {-1.0f * orientation, 0.0f, 0.0f },	{ 1.0f, 1.0f });
         appendVertex(vertices, layout, {-1.0f * halfExtents.x,  1.0f * halfExtents.y, -1.0f * halfExtents.z }, {-1.0f * orientation, 0.0f, 0.0f },	{ 0.0f, 1.0f });

         // top
         appendVertex(vertices, layout, {-1.0f * halfExtents.x,  1.0f * halfExtents.y,  1.0f * halfExtents.z }, { 0.0f,  1.0f * orientation, 0.0f}, { 0.0f, 0.0f });
         appendVertex(vertices, layout, { 1.0f * halfExtents.x,  1.0f * halfExtents.y,  1.0f * halfExtents.z }, { 0.0f,  1.0f * orientation, 0.0f}, { 1.0f, 0.0f });
         appendVertex(vertices, layout, { 1.0f * halfExtents.x,  1.0f * halfExtents.y, -1.0f * halfExtents.z }, { 0.0f,  1.0f * orientation, 0.0f}, { 1.0f, 1.0f });
         appendVertex(vertices, layout, {-1.0f * halfExtents.x,  1.0f * halfExtents.y, -1.0f * halfExtents.z }, { 0.0f,  1.0f * orientation, 0.0f}, { 0.0f, 1.0f });

         // bottom
         appendVertex(vertices, layout, {-1.0f * halfExtents.x, -1.0f * halfExtents.y, -1.0f * halfExtents.z }, { 0.0f, -1.0f * orientation, 0.0f}, { 0.0f, 0.0f });
         appendVertex(vertices, layout, { 1.0f * halfExtents.x, -1.0f * halfExtents.y, -1.0f * halfExtents.z }, { 0.0f, -1.0f * orientation, 0.0f}, { 1.0f, 0.0f });
         appendVertex(vertices, layout, { 1.0f * halfExtents.x, -1.0f * halfExtents.y,  1.0f * halfExtents.z }, { 0.0f, -1.0f * orientation, 0.0f}, { 1.0f, 1.0f });
         appendVertex(vertices, layout, {-1.0f * halfExtents.x, -1.0f * halfExtents.y,  1.0f * halfExtents.z }, { 0.0f, -1.0f * orientation, 0.0f}, { 0.0f, 1.0f });
    }

    std::vector<uint32_t> indices {
            0,  1,  2,	  0,  2,  3,  //front
            4,  5,  6,	  4,  6,  7,  //right
            8,  9, 10,	  8, 10, 11,  //back
            12, 13, 14,	 12, 14, 15,  //left
            16, 17, 18,	 16, 18, 19,  //upper
            20, 21, 22,	 20, 22, 23   //bottom
    };

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> MeshFactory::CreateSphere(const Vertex::Layout& layout, uint32_t stacks, uint32_t slices, float radius) {
    std::vector<uint8_t> vertices;
    vertices.reserve(layout.getStride() * (stacks * slices + 2));

    std::vector<uint32_t> indices;
    vertices.reserve(stacks * slices);

    float sliceStep = 2 * M_PI / static_cast<float>(slices);
    float stackStep = M_PI / static_cast<float>(stacks);
    float lengthInv = 1.0f / radius;

    for (uint32_t i = 0; i <= stacks; ++i) {
        float stackAngle = M_PI_2 - static_cast<float>(i) * stackStep;  // starting from pi/2 to -pi/2
        float xy = radius * cosf(stackAngle); // r * cos(u)
        float z = radius * sinf(stackAngle);  // r * sin(u)

        // add (sliceCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (uint32_t j = 0; j <= slices; ++j) {
            float sliceAngle = static_cast<float>(j) * sliceStep; // starting from 0 to 2pi

            glm::vec3 pos {
                xy * cosf(sliceAngle),
                xy * sinf(sliceAngle),
                z
            };

            glm::vec3 norm { pos * lengthInv };

            glm::vec2 tex {
                1.0f - static_cast<float>(j) / static_cast<float>(slices),
                static_cast<float>(i) / static_cast<float>(stacks)
            };

            appendVertex(vertices, layout, pos, norm, tex);
        }
    }

    for (uint32_t i = 0; i < stacks; ++i) {
        uint32_t k1 = i * (slices + 1);      // beginning of current stack
        uint32_t k2 = k1 + slices + 1;      // beginning of next stack

        for (uint32_t j = 0; j < slices; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> MeshFactory::CreateQuad(const Vertex::Layout& layout, const glm::vec2& extent) {
    std::vector<glm::vec3> position = {
            {-extent.x, -extent.y, 0.0f},
            {extent.x, -extent.y, 0.0f},
            {-extent.x, extent.y, 0.0f},
            {extent.x, -extent.y, 0.0f},
            {extent.x, extent.y, 0.0f},
            {-extent.x, extent.y, 0.0f}
    };

    std::vector<uint8_t> vertices;
    vertices.reserve(layout.getStride() * 6);
    {
        appendVertex(vertices, layout, position[0], { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[1], { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[2], { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f });
        appendVertex(vertices, layout, position[3], { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[4], { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f });
        appendVertex(vertices, layout, position[5], { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f });
    }
    return std::make_shared<Mesh>(vertices);
}

std::shared_ptr<Mesh> MeshFactory::CreateOctahedron(const Vertex::Layout& layout, const glm::vec3& extent) {
    std::vector<glm::vec3> position = {
            {0.0f, extent.y, 0.0f},
            {extent.x, 0.0f, extent.z},
            {-extent.x, 0.0f, extent.z},
            {-extent.x, 0.0f, -extent.z},
            {extent.x, 0.0f, -extent.z},
            {0.0f, -extent.y, 0.0f}
    };

    std::vector<glm::vec3> normals = {
            glm::cross(position[0] - position[2], position[0] - position[1]),
            glm::cross(position[0] - position[3], position[0] - position[2]),
            glm::cross(position[0] - position[1], position[0] - position[4]),
            glm::cross(position[0] - position[4], position[0] - position[3]),
            glm::cross(position[5] - position[2], position[5] - position[1]),
            glm::cross(position[5] - position[3], position[5] - position[2]),
            glm::cross(position[5] - position[1], position[5] - position[4]),
            glm::cross(position[5] - position[4], position[5] - position[3])
    };

    std::vector<uint8_t> vertices;
    vertices.reserve(layout.getStride() * 24);
    {
        appendVertex(vertices, layout, position[0], normals[0], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[2], normals[0], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[1], normals[0], { 0.5f, 1.0f });
        appendVertex(vertices, layout, position[0], normals[1], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[3], normals[1], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[2], normals[1], { 0.5f, 1.0f });
        appendVertex(vertices, layout, position[0], normals[2], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[1], normals[2], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[4], normals[2], { 0.5f, 1.0f });
        appendVertex(vertices, layout, position[0], normals[3], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[4], normals[3], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[3], normals[3], { 0.5f, 1.0f });
        appendVertex(vertices, layout, position[1], normals[4], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[2], normals[4], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[5], normals[4], { 0.5f, 1.0f });
        appendVertex(vertices, layout, position[2], normals[5], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[3], normals[5], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[5], normals[5], { 0.5f, 1.0f });
        appendVertex(vertices, layout, position[4], normals[6], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[1], normals[6], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[5], normals[6], { 0.5f, 1.0f });
        appendVertex(vertices, layout, position[3], normals[7], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[4], normals[7], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[5], normals[7], { 0.5f, 1.0f });
    }

    return std::make_shared<Mesh>(vertices);
}

std::shared_ptr<Mesh> MeshFactory::CreateTetrahedron(const Vertex::Layout& layout, const glm::vec3& extent) {
    std::vector<glm::vec3> position = {
            {0.0f, extent.y, 0.0f},
            {0.0f, 0.0f, extent.z},
            {-extent.x, 0.0f, -extent.z},
            {extent.x, 0.0f, -extent.z}
    };

    std::vector<glm::vec3> normals = {
            glm::cross(position[0] - position[2], position[0] - position[1]),
            glm::cross(position[0] - position[3], position[0] - position[2]),
            glm::cross(position[0] - position[1], position[0] - position[3]),
            glm::cross(position[1] - position[2], position[1] - position[3])
    };

    std::vector<uint8_t> vertices;
    vertices.reserve(layout.getStride() * 12);
    {
        appendVertex(vertices, layout, position[0], normals[0], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[2], normals[0], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[1], normals[0], { 0.5f, 1.0f });
        appendVertex(vertices, layout, position[0], normals[1], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[3], normals[1], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[2], normals[1], { 0.5f, 1.0f });
        appendVertex(vertices, layout, position[0], normals[2], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[1], normals[2], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[3], normals[2], { 0.5f, 1.0f });
        appendVertex(vertices, layout, position[1], normals[3], { 0.0f, 0.0f });
        appendVertex(vertices, layout, position[2], normals[3], { 1.0f, 0.0f });
        appendVertex(vertices, layout, position[3], normals[3], { 0.5f, 1.0f });
    }

    return std::make_shared<Mesh>(vertices);
}

std::shared_ptr<Mesh> MeshFactory::CreateTorus(const Vertex::Layout& layout, uint32_t sides, uint32_t innerSides, float radius, float innerRadius) {
    uint32_t numVertices = (sides + 1) * (innerSides + 1);
    uint32_t numIndices = (2 * sides + 4) * innerSides;

    std::vector<uint8_t> vertices;
    vertices.reserve(layout.getStride() * numVertices);
    std::vector<uint32_t> indices;
    indices.reserve(numIndices);

    auto angleIncs = static_cast<uint32_t>(360.0f / static_cast<float>(sides));
    auto cs_angleIncs = static_cast<uint32_t>(360.0f / static_cast<float>(innerSides));

    // iterate innerSides: inner ring
    for (uint32_t j = 0; j <= 360; j += angleIncs) {
        auto ja = static_cast<float>(j);

        float currentRadius = radius + (innerRadius * cosf(glm::radians(ja)));
        float z = innerRadius * sinf(glm::radians(ja));

        // iterate sides: outer ring
        for (uint32_t i = 0; i <= 360; i += cs_angleIncs) {
            auto ia = static_cast<float>(i);

            glm::vec3 position {
                currentRadius * cosf(glm::radians(ia)),
                currentRadius * sinf(glm::radians(ia)),
                z
            };

            glm::vec3 norm {
                position.x - radius * cosf(glm::radians(ia)),
                position.y - radius * sinf(glm::radians(ia)),
                position.z
            };

            glm::vec2 tex {
                ia / 360.0f,
                2.0f * ja / 360.0f - 1
            };

            if (tex.t < 0)
                tex.t = -tex.t;

            appendVertex(vertices, layout, position, glm::normalize(norm), tex);
        }
    }

    // indices grouped by GL_TRIANGLE_STRIP, face oriented clock-wise

    // inner ring
    for (uint32_t i = 0, nextrow = sides + 1; i < innerSides; i++) {
        // outer ring
        uint32_t j;
        for (j = 0; j < sides; j ++) {
            indices.push_back((i + 1) * nextrow + j);
            indices.push_back(i * nextrow + j);
        }

        // generate dummy triangle to avoid messing next ring
        uint32_t dummy = i * nextrow + j;
        indices.push_back(dummy + nextrow);
        indices.push_back(dummy);
        indices.push_back(dummy + nextrow);
        indices.push_back(dummy + nextrow);
    }

    return std::make_shared<Mesh>(vertices, indices); // GL_TRIANGLE_STRIP
}*/