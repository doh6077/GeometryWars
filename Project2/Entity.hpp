#pragma once
#include "Components.hpp"
#include <string>
#include <tuple>


class EntityManager;

using ComponentTuple = std::tuple<
    CTransform,
    CShape,
    CCollision,
    CInput,
    CScore,
    CLifespan


>;

class Entity
{
    friend class EntityManager; // allows friend access to private, only thing that can construct Entity is EntityManager as the attributes are private 

    ComponentTuple m_components;
    bool m_active = true; // if false, will delete entity
    std::string m_tag = "default"; // type of entity
    size_t m_id = 0; // unique integer id

    Entity(const size_t& id, const std::string& tag)
        : m_tag(tag)
        , m_id(id)
    {
    }
public:

    bool isActive() const
    {
        return m_active;
    }
    void destroy() {

        m_active = false;
    }

    size_t id() const
    {
        return m_id;
    }

    const std::string& tag() const
    {
        return m_tag;
    }

    template <typename T>
    bool has() const
    {
        return get<T>().exists;

    }

    template <typename T, typename... TArgs>
    T& add(TArgs&&...mArgs)
    {
        auto& component = get<T>();
        component = T(std::forward<TArgs>(mArgs)...);
        component.exists = true;
        return component;
    }

    template<typename T>
    T& get()
    {
        return std::get<t>(m_components);
    }

    template<typename T>
    const T& get() const
    {
        return std::get<T>(m_components);
    }

    template<typename T>
    void remove()
    {
        get<T>() = T();
    }

};