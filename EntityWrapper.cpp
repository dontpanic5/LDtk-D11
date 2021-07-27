#include "EntityWrapper.h"
#include "LDtkLoader/Entity.hpp"

EntityWrapper::EntityWrapper(const ldtk::Entity& e) :m_entity(e), m_pos(e.getPosition())
{
	try {
		auto& patrol = m_entity.getArrayField<ldtk::IntPoint>("patrol");
		m_dest = patrol[0].value();
	}
	// will throw if there's no patrol
	catch (std::invalid_argument& err)
	{
	}
}

const ldtk::Entity& EntityWrapper::GetEntity()
{
	return m_entity;
}

const ldtk::IntPoint& EntityWrapper::GetPos()
{
	return m_pos;
}

void EntityWrapper::Move(float elapsedTime)
{
	// TODO MATH
}