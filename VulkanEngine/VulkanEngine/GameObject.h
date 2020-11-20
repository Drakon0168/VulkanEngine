#pragma once
#include "pch.h"

#include "Transform.h"
#include "Mesh.h"
#include "PhysicsObject.h"

class GameObject
{
private:
	std::vector<std::shared_ptr<Component>> components;
	std::shared_ptr<Transform> transform;
	std::shared_ptr<PhysicsObject> physicsObject;

	std::shared_ptr<Mesh> mesh;
	int instanceId;

	std::string name;

	bool active;

public:
#pragma region Constructor

	GameObject(std::shared_ptr<Mesh> mesh);

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the transform that is being used by this game object
	/// </summary>
	/// <returns>The game object's transform</returns>
	std::shared_ptr<Transform> GetTransform();

	/// <summary>
	/// Returns the physics object that is being used by this game object
	/// </summary>
	/// <returns>The game object's physics object</returns>
	std::shared_ptr<PhysicsObject> GetPhysicsObject();

	/// <summary>
	/// Creates a new physics object to set this gameObject's physicsObject to
	/// </summary>
	/// <param name="layer">The physics layer to set the object to, Dynamic by default</param>
	/// <param name="colliderType">The type of collider to use for this object, ARBB by default</param>
	void SetPhysicsObject(PhysicsLayers layer = PhysicsLayers::Dynamic, ColliderTypes::ColliderTypes colliderType = ColliderTypes::ARBB, float mass = 1.0f, bool affectedByGravity = true, bool alive = true);

	/// <summary>
	/// Returns the mesh that is being used by this game object
	/// </summary>
	/// <returns>The gameobject's mesh</returns>
	std::shared_ptr<Mesh> GetMesh();

	/// <summary>
	/// Returns the name of this gameobject
	/// </summary>
	/// <returns>The gameobject's name</returns>
	std::string GetName();

	/// <summary>
	/// Sets the name of this gameobject used to display the object and access it in game manager
	/// </summary>
	/// <param name="value">The string to set the name to</param>
	void SetName(std::string value);

	/// <summary>
	/// Returns whether this object is active (visible and interactable)
	/// </summary>
	/// <returns>True if the object is active</returns>
	bool GetActive();

	/// <summary>
	/// Adds a component of the specified type to the gameobject
	/// </summary>
	/// <typeparam name="T">The type of component to add</typeparam>
	/// <returns>The component that was added</returns>
	template <class T>
	std::shared_ptr<T> AddComponent();

	/// <summary>
	/// Adds the specified component to the gameobject
	/// </summary>
	/// <typeparam name="T">The type of component to add</typeparam>
	/// <param name="value">The component to add</param>
	/// <returns>The ID of the component in the gameobject's component list</returns>
	template <class T>
	int AddComponent(std::shared_ptr<T> value);

	/// <summary>
	/// Removes the specified component from the list of components
	/// </summary>
	/// <param name="ID">The ID of the component to remove</param>
	void RemoveComponent(int ID);

	/// <summary>
	/// Returns the first attached component of the specified type
	/// </summary>
	/// <typeparam name="T">The type of component to find, type must inherit from Component</typeparam>
	/// <returns>The component of the specified type</returns>
	template<class T>
	std::shared_ptr<T> GetComponent();

	/// <summary>
	/// Returns a list of all attached components of the specified type
	/// </summary>
	/// <typeparam name="T">The type of component to search for, type must inherit from Component</typeparam>
	/// <returns>A list of all attached components of the specified type</returns>
	template<class T>
	std::vector<std::shared_ptr<T>> GetComponents();

#pragma endregion

#pragma region Spawning

	/// <summary>
	/// Initializes the gameobjects variables
	/// </summary>
	virtual void Init();

	/// <summary>
	/// Sets the object to active and spawns it into the entity manager
	/// </summary>
	virtual void Spawn();

	/// <summary>
	/// Sets the object as inactive and despawns it with the entity manager
	/// </summary>
	virtual void Despawn();

	/// <summary>
	/// Called by the physics system when this object collides with another
	/// </summary>
	/// <param name="other">The object that was collided with</param>
	virtual void OnCollision(GameObject* other);

#pragma endregion

#pragma region Update

	/// <summary>
	/// Updates this objects variables once per frame
	/// </summary>
	virtual void Update();

#pragma endregion
};

template<class T>
inline std::shared_ptr<T> GameObject::AddComponent()
{
	std::shared_ptr<T> c = std::make_shared<T>();
	AddComponent(c);

	return c;
}

template<class T>
inline int GameObject::AddComponent(std::shared_ptr<T> value)
{
	std::shared_ptr<Component> component = std::dynamic_pointer_cast<Component, T>(value);

	if (component != nullptr) {
		int ID = -1;

		for (int i = 0; i < components.size(); i++) {
			if (components[i] == nullptr) {
				ID = i;
				components[i] = component;
				break;
			}
		}

		if (ID == -1) {
			ID = components.size();
			components.push_back(component);
		}

		component->SetID(ID);
		component->SetGameObject(this);

		return ID;
	}

	std::cout << "Failed to add component invalid type." << std::endl;
	return -1;
}

template<class T>
inline std::shared_ptr<T> GameObject::GetComponent()
{
	for (int i = 0; i < components.size(); i++) {
		std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(components[i]);
		if (p != nullptr) {
			return p;
		}
	}

	return nullptr;
}

template<class T>
inline std::vector<std::shared_ptr<T>> GameObject::GetComponents()
{
	std::vector<std::shared_ptr<T>> foundComponents = std::vector<std::shared_ptr<T>>();

	for (int i = 0; i < components.size(); i++) {
		std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(components[i]);
		if (p != nullptr) {
			foundComponents.push_back(p);
		}
	}

	return foundComponents;
}