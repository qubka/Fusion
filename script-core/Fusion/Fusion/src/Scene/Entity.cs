using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace Fusion
{
	public class Entity
	{
		protected Entity() { ID = uint.MaxValue; } 

		internal Entity(uint id)
		{
			ID = id;
		}

		public readonly uint ID;

		public Vector3 Position
		{
			get
			{
				InternalCalls.TransformComponent_GetPosition(ID, out Vector3 result);
				return result;
			}
			set
			{
				InternalCalls.TransformComponent_SetPosition(ID, ref value);
			}
		}

		public Quaternion Rotation
		{
			get
			{
				InternalCalls.TransformComponent_GetRotation(ID, out Quaternion result);
				return result;
			}
			set
			{
				InternalCalls.TransformComponent_SetRotation(ID, ref value);
			}
		}

		public Vector3 Scale
		{
			get
			{
				InternalCalls.TransformComponent_GetScale(ID, out Vector3 result);
				return result;
			}
			set
			{
				InternalCalls.TransformComponent_SetScale(ID, ref value);
			}
		}

		public bool HasComponent<T>() where T : Component, new()
		{
			Type componentType = typeof(T);
			return InternalCalls.Entity_HasComponent(ID, componentType);
		}

		public T GetComponent<T>() where T : Component, new()
		{
			if (!HasComponent<T>())
				return null;

			T component = new T() { Entity = this };
			return component;
		}
		
		public Entity FindEntityByName(string name)
		{
			uint entityID = InternalCalls.Entity_FindEntityByName(name);
			if (entityID == uint.MaxValue)
				return null;

			return new Entity(entityID);
		}

		public T As<T>() where T : Entity, new()
		{
			object instance = InternalCalls.GetScriptInstance(ID);
			return instance as T;
		}

	}
}
