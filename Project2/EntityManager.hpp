#pragma once

#include "Entity.hpp"

using EntityVec = std::vector<std::shared_ptr<Entity>>;
class EntityManager
{
	EntityVec m_entities;
	EntityVec m_entitiesToAdd;
	std::map<std::string, EntityVec> m_entityMap;
	size_t    m_totalEntities = 0;

	void removeDeadEntities(EntityVec& vec) {

		// TODO: remove all dead entities from the input vector 
		// This is called by the update() function 
		for (std::size_t i = 0; i < vec.size(); ++i) {
			if (!vec[i]->isActive()) {
				vec.erase(vec.begin() + i); // Erase shifts elements left
			}
			else {
				++i; // Increment only if no entity was removed
			}
		}


	}
public:
	EntityManager() = default; 
	void update() {
		// Add entities from m_entitiesToAdd to the proper locations
		for (const auto& entity : m_entitiesToAdd) {
			m_entities.push_back(entity);
			m_entityMap[entity->m_tag].push_back(entity);
		}
		m_entitiesToAdd.clear();

		// Remove dead entities
		removeDeadEntities(m_entities);

		for (auto& pair : m_entityMap) {
			removeDeadEntities(pair.second);
		}
	}
	size_t getTotalEntities() const {
		return m_totalEntities;
	}

	std::shared_ptr<Entity> addEntity(const std::string& tag) {

		// create the entity shared pointer 
		auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

		// add it to the vec of all entities 
		m_entitiesToAdd.push_back(entity);

		// add it to the entity map 
		if (m_entityMap.find(tag) == m_entityMap.end()) {
			m_entityMap[tag] = EntityVec();
		}
		m_entityMap[tag].push_back(entity);


		return entity; 


	}

	const EntityVec& getEntities()
	{
		return m_entities; 
	}

	const EntityVec& getEntities(const std::string& tag) {
		
		if (m_entityMap.find(tag) == m_entityMap.end()) {
			m_entityMap[tag] = EntityVec();

		}

		return m_entityMap[tag];
	
	}

};