#pragma once
#include "LDtkLoader/Entity.hpp"

class EntityWrapper
{
public:
	EntityWrapper(const ldtk::Entity& e);

private:
	const ldtk::Entity& m_entity;
	float m_pos[2];
	ldtk::IntPoint m_dest;
};

