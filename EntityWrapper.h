#pragma once
#include "LDtkLoader/Entity.hpp"

class EntityWrapper
{
public:
	EntityWrapper(const ldtk::Entity& e, int cell_size);

	auto GetEntity() const -> const ldtk::Entity&;
	const ldtk::FloatPoint& GetPos() const;
	void Move(float elapsedTime);

private:
	const ldtk::Entity& m_entity;
	ldtk::FloatPoint m_pos;
	ldtk::IntPoint m_dest;

	int m_patrol_idx = 0;
	bool m_patrolling_forward = true;

	bool m_moves;

	const int m_cell_size;

	float GetLegDist();
	void SetNextDest(ldtk::ArrayField<ldtk::IntPoint> patrol);
};

