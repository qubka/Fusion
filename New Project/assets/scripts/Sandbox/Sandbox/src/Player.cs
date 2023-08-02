using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using Fusion;

namespace Sandbox
{
	public class Player : Entity
	{
		private TransformComponent m_Transform;
		//private Rigidbody2DComponent m_Rigidbody;

		private Entity m_Camera;

		public float Speed;
		public float Time = 0.0f;

		void OnCreate()
		{
			Console.WriteLine($"Player.OnCreate - {ID}");

			m_Transform = GetComponent<TransformComponent>();
			//m_Rigidbody = GetComponent<Rigidbody2DComponent>();
			
			m_Camera = FindEntityByName("Camera");
		}

		void OnUpdate(float ts)
		{
			Time += ts;
			// Console.WriteLine($"Player.OnUpdate: {ts}");

			float speed = Speed;
			Vector3 velocity = Vector3.Zero;

			if (Input.IsKeyDown(KeyCode.W))
				velocity.Y = 1.0f;
			else if (Input.IsKeyDown(KeyCode.S))
				velocity.Y = -1.0f;

			if (Input.IsKeyDown(KeyCode.A))
				velocity.X = -1.0f;
			else if (Input.IsKeyDown(KeyCode.D))
				velocity.X = 1.0f;

			if (m_Camera != null)
			{
				Camera camera = m_Camera.As<Camera>();

				if (Input.IsKeyDown(KeyCode.Q))
					camera.DistanceFromPlayer += speed * 2.0f * ts;
				else if (Input.IsKeyDown(KeyCode.E))
					camera.DistanceFromPlayer -= speed * 2.0f * ts;
			}

			velocity *= speed * ts;

			//m_Rigidbody.ApplyLinearImpulse(velocity.XY, true);

			Vector3 position = m_Transform.Position;
			position += velocity * ts;
			m_Transform.Position = position;
		}
	}
}