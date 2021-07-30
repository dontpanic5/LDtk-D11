#include "Utilities.h"
#include "pch.h"

using namespace DirectX;

//template<typename T>
ldtk::Point<float> TransformByPivot(const ldtk::FloatPoint& pivot, const ldtk::Point<float>& cur_pos, const int size_x, const int size_y)
{
	return ldtk::Point<float>(
		static_cast<float>(cur_pos.x - pivot.x * size_x),
		static_cast<float>(cur_pos.y - pivot.y * size_y)
	);
}