#pragma once
#include "LDtkLoader/Entity.hpp"

class EntityWrapper
{
public:
	EntityWrapper(const ldtk::Entity& e);

	const ldtk::Entity& const GetEntity();
	const ldtk::IntPoint& const GetPos();
	void Move(float elapsedTime);

private:
	const ldtk::Entity& m_entity;
	ldtk::IntPoint m_pos;
	ldtk::IntPoint m_dest;
};

