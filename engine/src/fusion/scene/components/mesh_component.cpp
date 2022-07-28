#include "mesh_component.hpp"

/*#include "fusion/models/mesh.hpp"

fe::MeshComponent::MeshComponent() {
  std::vector<glm::vec3> vertices {
           //front
           {-1.f, -1.f,  1.f},
           { 1.f, -1.f,  1.f},
           { 1.f,  1.f,  1.f},
           {-1.f,  1.f,  1.f},

           //right
           { 1.f, -1.f,  1.f},
           { 1.f, -1.f, -1.f},
           { 1.f,  1.f, -1.f},
           { 1.f,  1.f,  1.f},

           //back
           { 1.f, -1.f, -1.f},
           {-1.f, -1.f, -1.f},
           {-1.f,  1.f, -1.f},
           { 1.f,  1.f, -1.f},

           //left
           {-1.f, -1.f, -1.f},
           {-1.f, -1.f,  1.f},
           {-1.f,  1.f,  1.f},
           {-1.f,  1.f, -1.f},

           //top
           {-1.f,  1.f,  1.f},
           { 1.f,  1.f,  1.f},
           { 1.f,  1.f, -1.f},
           {-1.f,  1.f, -1.f},

           //bottom
           {-1.f, -1.f, -1.f},
           { 1.f, -1.f, -1.f},
           { 1.f, -1.f,  1.f},
           {-1.f, -1.f,  1.f},
   };

   std::vector<uint16_t> indices {
           2,  1,  0,		3,  2,  0,  //front
           6,  5,  4,		7,  6,  4,  //right
           10, 9,  8,		11, 10, 8,  //back
           14, 13, 12,		15, 14, 12, //left
           18, 17, 16,		19, 18, 16, //upper
           22, 21, 20,		23, 22, 20  //bottom
   };

   mesh = std::make_shared<Mesh>(vertices, indices);
}
*/