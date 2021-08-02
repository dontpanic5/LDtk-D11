#include "EntityWrapper.h"
#include "LDtkLoader/Entity.hpp"
#include "Utilities.h"

#include <cmath>

EntityWrapper::EntityWrapper(const ldtk::Entity& e, int cell_size) :m_entity(e), m_pos(static_cast<float>(e.getPosition().x), static_cast<float>(e.getPosition().y)), m_cell_size(cell_size)
{
	try {
		auto& patrol = m_entity.getArrayField<ldtk::IntPoint>("patrol");

		if (patrol.size() == 0)
		{
			m_moves = false;
			return;
		}

		SetNextDest(patrol);

		m_moves = true;
	}
	// will throw if there's no patrol
	catch (std::invalid_argument& err)
	{
		err; // do nothing
		m_moves = false;
	}
}

auto EntityWrapper::GetEntity() const -> const ldtk::Entity&
{
	return m_entity;
}

const ldtk::FloatPoint& EntityWrapper::GetPos() const
{
	return m_pos;
}

void EntityWrapper::Move(float elapsedTime)
{
	if (!m_moves) return;

	auto& patrol = m_entity.getArrayField<ldtk::IntPoint>("patrol");

	// I travel 32 pixels in one second
	float travel_distance = elapsedTime * 32;

	auto leg_dist = GetLegDist();

	do
	{
		if (leg_dist > travel_distance)
		{
			// travel to intermediary distance

			m_pos.x = m_pos.x + travel_distance / leg_dist * (m_dest.x - m_pos.x);
			m_pos.y = m_pos.y + travel_distance / leg_dist * (m_dest.y - m_pos.y);

			travel_distance = 0; // set to zero to symbolize there's no need to travel more
		}
		// set up to go to next point
		else {
			m_pos = ldtk::FloatPoint(static_cast<float>(m_dest.x), static_cast<float>(m_dest.y));

			// progress to next point

			if (m_patrolling_forward)
			{
				if (m_patrol_idx < patrol.size() - 1)
				{
					m_patrol_idx++;
				}
				else
				{
					m_patrolling_forward = false;
					m_patrol_idx--;
				}
			}
			else if (!m_patrolling_forward)
			{
				if (m_patrol_idx > -1)
				{
					m_patrol_idx--;
				}
				else
				{
					m_patrolling_forward = true;
					m_patrol_idx++;
				}
			}

			if (m_patrol_idx > -1)
			{
				SetNextDest(patrol);
			}
			else
			{
				m_dest = m_entity.getPosition();
			}

			travel_distance -= leg_dist;
			leg_dist = GetLegDist();
		}
	} while (travel_distance > 0);
}

float EntityWrapper::GetLegDist()
{
	auto x_dist = m_pos.x - m_dest.x;
	auto y_dist = m_pos.y - m_dest.y;
	return sqrt(x_dist * x_dist + y_dist * y_dist);
}

void EntityWrapper::SetNextDest(ldtk::ArrayField<ldtk::IntPoint> patrol)
{
	m_dest = patrol[m_patrol_idx].value();
	m_dest.x *= m_cell_size;
	m_dest.y *= m_cell_size;
	m_dest = TransformByPivot(m_entity.getPivot(), m_dest, m_cell_size, m_cell_size);
}