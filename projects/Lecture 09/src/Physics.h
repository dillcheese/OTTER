#pragma once

#include<btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>

//include glm features
#include "GLM/glm.hpp"
//import other required features
#include <vector>


	//class for bullet3 collisions
class TTN_PhysicsB
	{
	public:
		TTN_PhysicsB();
		TTN_PhysicsB(glm::vec3 pos);

		static void InitCollision(); // initiailizes stuff required to detect and solve collisions


	protected:
		
		/*static btBroadphaseInterface* m_broadphase;
		static btDefaultCollisionConfiguration* m_config;
		static btCollisionDispatcher* m_dispatcher;

		static btSequentialImpulseConstraintSolver* m_solver;
		static btDiscreteDynamicsWorld* m_world;
		static btDefaultMotionState* m_MotionState;*/

		btTransform m_trans;
		//collision object
		btCollisionObject m_col;
		//mass of object
		btScalar m_mass;

	};