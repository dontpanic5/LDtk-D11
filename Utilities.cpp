#include "Utilities.h"
#include "pch.h"

using namespace DirectX;

XMFLOAT2 GetPos(const ldtk::Entity& e)
{
    auto& pivot = e.getPivot();
    return XMFLOAT2(
        static_cast<float>(e.getPosition().x) - pivot.x * static_cast<float>(e.getSize().x),
        static_cast<float>(e.getPosition().y) - pivot.y * static_cast<float>(e.getSize().y)
    );
}