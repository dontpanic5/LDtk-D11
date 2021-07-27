#include "Utilities.h"
#include "pch.h"

using namespace DirectX;

XMFLOAT2 GetPos(const ldtk::Entity& e, const ldtk::IntPoint& cur_pos)
{
    auto& pivot = e.getPivot();
    return XMFLOAT2(
        static_cast<float>(cur_pos.x) - pivot.x * static_cast<float>(e.getSize().x),
        static_cast<float>(cur_pos.y) - pivot.y * static_cast<float>(e.getSize().y)
    );
}