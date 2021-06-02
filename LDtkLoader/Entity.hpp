// Created by Modar Nasser on 22/11/2020.

#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <unordered_map>
#include "thirdparty/json.hpp"
#include "DataTypes.hpp"
#include "Enum.hpp"
#include "FieldsContainer.hpp"
#include "EntityDef.hpp"
#include "Utils.hpp"

namespace ldtk {

    class World;
    class Layer;

    class Entity : public FieldsContainer {
        friend Layer;
    public:

        auto getName() const -> const std::string&;
        auto getSize() const -> const IntPoint&;
        auto getPosition() const -> const IntPoint&;
        auto getGridPosition() const -> const IntPoint&;
        auto getColor() const -> const Color&;
        auto getPivot() const -> const FloatPoint&;

        auto hasIcon() const -> bool;
        auto getIconTileset() const -> const Tileset&;
        auto getIconTexturePos() const -> IntPoint;

    private:
        explicit Entity(const nlohmann::json& j, const World* w);

        const EntityDef* m_definition = nullptr;

        const IntPoint m_size;
        const IntPoint m_position;
        const IntPoint m_grid_pos;

    };

}
