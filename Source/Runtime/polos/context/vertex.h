#ifndef POLOS_VERTEX_H_
#define POLOS_VERTEX_H_

namespace polos
{
    struct vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textureCoord;
        glm::vec3 color;
    };
}

#endif //POLOS_VERTEX_H_
