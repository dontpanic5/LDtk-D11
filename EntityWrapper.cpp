#include "EntityWrapper.h"
#include "LDtkLoader/Entity.hpp"

EntityWrapper::EntityWrapper(const ldtk::Entity& e) :m_entity(e)
{
	try {
		auto& patrol = m_entity.getArrayField<ldtk::IntPoint>("patrol");
		m_dest = patrol[0].value();
	}
	// will throw if there's no patrol
	catch (std::invalid_argument& err)
	{
	}

	m_pos[0] = static_cast<float>(e.getPosition().x);
	m_pos[1] = static_cast<float>(e.getPosition().y);
}