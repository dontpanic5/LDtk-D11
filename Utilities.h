#pragma once

#include "pch.h"
#include "LDtkLoader/Entity.hpp"

template<typename T>
ldtk::Point<T> TransformByPivot(const ldtk::FloatPoint& pivot, const ldtk::Point<T>& cur_pos, const int size_x, const int size_y)
{
	return ldtk::Point<T>(
		static_cast<T>(cur_pos.x + pivot.x * size_x),
		static_cast<T>(cur_pos.y + pivot.y * size_y)
	);
}