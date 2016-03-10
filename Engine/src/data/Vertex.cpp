#include "Vertex.h"

void Vertices_FBX::resize( size_t newSize )
{
    position.resize(newSize);
    color.resize(newSize);
    normal.resize(newSize);
    tangent.resize(newSize);
    binormal.resize(newSize);
    indices.resize(newSize);
    weights.resize(newSize);
    texCoord1.resize(newSize);
    texCoord2.resize(newSize);
}
