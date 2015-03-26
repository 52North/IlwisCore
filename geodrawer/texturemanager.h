#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "drawers/drawerinterface.h"

namespace Ilwis {
namespace Geodrawer{
class TextureManager
{
public:
    TextureManager();
    ~TextureManager();

private:
    std::vector<VertexColorI> _vertexData;
};
}
}

#endif // TEXTUREMANAGER_H
