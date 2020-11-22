#include "Physics.h"

TTN_PhysicsB::TTN_PhysicsB()
{
	btCollisionShape* boxCol = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	m_mass = 2.0f;

}

TTN_PhysicsB::TTN_PhysicsB(glm::vec3 pos)
{
	static btDiscreteDynamicsWorld* m_world;
	static btDefaultMotionState* m_MotionState;

	btCollisionShape* boxCol = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f)); //size

	m_trans.setIdentity();
	m_trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
	m_mass = 2.0f;

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	m_MotionState = new btDefaultMotionState(m_trans);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(m_mass, m_MotionState, boxCol, btVector3(0, 0, 0));// first: mass in kg, last:local interia in btvec3
	btRigidBody* body = new btRigidBody(rbInfo);

	//add the body to the world
	m_world->addRigidBody(body);

}

void TTN_PhysicsB::InitCollision()
{

	static btBroadphaseInterface* m_broadphase;
	static btDefaultCollisionConfiguration* m_config;
	static btCollisionDispatcher* m_dispatcher;

	static btSequentialImpulseConstraintSolver* m_solver;
	static btDiscreteDynamicsWorld* m_world;
	static btDefaultMotionState* m_MotionState;

	// Build the broadphase which essentially detects overlapping aabb pairs
	m_broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	m_config = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_config);

	// The actual physics solver
	m_solver = new btSequentialImpulseConstraintSolver;

	// The world.
	m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_config);
	m_world->setGravity(btVector3(0, -9.80f, 0));

}
