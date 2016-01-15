#ifndef LAB7_H
#define LAB7_H

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes\OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes\OgreBulletCollisionsBoxShape.h>
#include <Shapes\OgreBulletCollisionsCapsuleShape.h>

#include <SdkCameraMan.h>

using namespace Ogre;

class Lab7
	:	public WindowEventListener,
		public FrameListener,
		public OIS::KeyListener,
		public OIS::MouseListener
{
public:
	Lab7();
	virtual ~Lab7();

	bool start();
private:
	virtual bool frameRenderingQueued(const FrameEvent& e);

	virtual bool keyPressed(const OIS::KeyEvent& e);
	virtual bool keyReleased(const OIS::KeyEvent& e);
	virtual bool mouseMoved(const OIS::MouseEvent& e);
	virtual bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id);

	void SetupBullet();
	void SetupCamera();
	void SetupScene();
	void SetupOIS();

	Root* mRoot;
	String mPlugins;
	String mResources;
	RenderWindow* mWindow;
	SceneManager* mScene;
	Camera* mCamera;

	OgreBites::SdkCameraMan* mCameraMan;

	OIS::InputManager* mInput;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;

	OgreBulletDynamics::DynamicsWorld* mWorld;
	OgreBulletCollisions::DebugDrawer* mDebugDraw;
	int mNumEnity;
	
	std::deque<OgreBulletDynamics::RigidBody*> mRigidBody;
	std::deque<OgreBulletCollisions::CollisionShape*> mShape;
};

#endif