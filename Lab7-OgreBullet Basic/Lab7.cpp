#include "Lab7.h"

#include <OgreEntity.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreConfigFile.h>
#include <OgreException.h>

Lab7::Lab7()
	:	mRoot(0),
		mPlugins(StringUtil::BLANK),
		mResources(StringUtil::BLANK),
		mWindow(0),
		mScene(0),
		mCamera(0),
		mCameraMan(0),
		mInput(0),
		mKeyboard(0),
		mMouse(0),
		mWorld(0),
		mDebugDraw(0),
		mNumEnity(0),
		mRigidBody(0),
		mShape(0)
{}

Lab7::~Lab7()
{
	if(mCameraMan) delete mCameraMan;

	WindowEventUtilities::removeWindowEventListener(mWindow, this);

	std::deque<OgreBulletDynamics::RigidBody*>::iterator itRigidBody = mRigidBody.begin();
	while(itRigidBody != mRigidBody.end())
	{
		delete *itRigidBody;
		itRigidBody++;
	}

	std::deque<OgreBulletCollisions::CollisionShape*>::iterator itShape = mShape.begin();
	while(itShape != mShape.end())
	{
		delete *itShape;
		itShape++;
	}

	mRigidBody.clear();
	mShape.clear();

	delete mWorld->getDebugDrawer();
	mWorld->setDebugDrawer(0);

	if(mInput)
	{
		mInput->destroyInputObject(mKeyboard);
		mInput->destroyInputObject(mMouse);

		OIS::InputManager::destroyInputSystem(mInput);
		mInput = 0;
	}

	delete mWorld;
	delete mRoot;
}

bool Lab7::start()
{
#ifdef _DEBUG
	mPlugins = "plugins_d.cfg";
	mResources = "resources_d.cfg";
#else
	mPlugins = "plugins.cfg";
	mResources = "resources.cfg";
#endif

	mRoot = new Root(mPlugins);
	
	ConfigFile cf;
	cf.load(mResources);

	String name, locType;
	ConfigFile::SectionIterator secIt = cf.getSectionIterator();

	while(secIt.hasMoreElements())
	{
		ConfigFile::SettingsMultiMap* setting = secIt.getNext();
		ConfigFile::SettingsMultiMap::iterator it;

		for(it = setting->begin(); it != setting->end(); it++)
		{
			locType = it->first;
			name = it->second;

			ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
		}
	}

	if(!(mRoot->restoreConfig() || mRoot->showConfigDialog())) return false;

	mWindow = mRoot->initialise(true, "Lab7 Window");

	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mScene = mRoot->createSceneManager(ST_GENERIC);

	SetupBullet();
	SetupCamera();
	SetupScene();
	SetupOIS();

	WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
	mRoot->startRendering();

	return true;
}

bool Lab7::frameRenderingQueued(const FrameEvent& e)
{
	if(mWindow->isClosed()) return false;

	mKeyboard->capture();
	mMouse->capture();

	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;

	mCameraMan->frameRenderingQueued(e);

	mWorld->stepSimulation(e.timeSinceLastFrame);

	return true;
}

bool Lab7::keyPressed(const OIS::KeyEvent& e)
{
	mCameraMan->injectKeyDown(e);

	return true;
}

bool Lab7::keyReleased(const OIS::KeyEvent& e)
{
	mCameraMan->injectKeyUp(e);

	return true;
}

bool Lab7::mouseMoved(const OIS::MouseEvent& e)
{
	mCameraMan->injectMouseMove(e);

	return true;
}

bool Lab7::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	mCameraMan->injectMouseDown(e, id);

	if(e.state.buttonDown(OIS::MB_Left) || e.state.buttonDown(OIS::MB_Right))
	{
		Ogre::Vector3 size = Ogre::Vector3::ZERO;
		Ogre::Vector3 position = (mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 20);

		Entity* ent = mScene->createEntity("Shape" + StringConverter::toString(mNumEnity), "ogrehead.mesh");
		ent->setCastShadows(true);
		AxisAlignedBox boundingB = ent->getBoundingBox();
		size = boundingB.getSize();
		size = size/2*0.95;

		SceneNode* node = mScene->getRootSceneNode()->createChildSceneNode();
		if(e.state.buttonDown(OIS::MB_Left))
		{
			node->scale(0.1,0.1,0.1);
			size *= 0.1;
		}
		else if(e.state.buttonDown(OIS::MB_Right))
		{
			node->scale(0.3,0.3,0.3);
			size *= 0.3;
		}

		node->attachObject(ent);

		OgreBulletCollisions::CapsuleCollisionShape* sceneShape = new OgreBulletCollisions::CapsuleCollisionShape(size.x, 0.0, Ogre::Vector3::UNIT_Y);

		OgreBulletDynamics::RigidBody* defaultBody = new OgreBulletDynamics::RigidBody("defaultBoxRigidBody"+StringConverter::toString(mNumEnity), mWorld);

		SceneNode* rotate = mScene->getRootSceneNode()->createChildSceneNode();
		rotate->setOrientation(mCamera->getDerivedOrientation());
		rotate->yaw(Degree(180.0));

		defaultBody->setShape(node, sceneShape, 0.6, 0.6, 1.0, position, rotate->getOrientation());
	
		mNumEnity++;

		if(e.state.buttonDown(OIS::MB_Left))
			defaultBody->setLinearVelocity(mCamera->getDerivedDirection().normalisedCopy() * 0.7);
		else if(e.state.buttonDown(OIS::MB_Right))
			defaultBody->setLinearVelocity(mCamera->getDerivedDirection().normalisedCopy() * 300);

		mShape.push_back(sceneShape);
		mRigidBody.push_back(defaultBody);
	}

	return true;
}

bool Lab7::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	mCameraMan->injectMouseUp(e, id);

	return true;
}

void Lab7::SetupBullet()
{
	mWorld = new OgreBulletDynamics::DynamicsWorld(mScene, AxisAlignedBox(Ogre::Vector3 (-10000, -10000, -10000), Ogre::Vector3 (10000,  10000,  10000)), Ogre::Vector3(0.0, -9.81, 0.0));

	mDebugDraw = new OgreBulletCollisions::DebugDrawer();
	mDebugDraw->setDrawWireframe(true);

	mWorld->setDebugDrawer(mDebugDraw);
	mWorld->setShowDebugShapes(true);

	SceneNode* node = mScene->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
	node->attachObject(static_cast<SimpleRenderable*>(mDebugDraw));
}

void Lab7::SetupCamera()
{
	mCamera = mScene->createCamera("MainCam");
	mCamera->setPosition(0,10,50);
	mCamera->lookAt(0,0,-300);
	mCamera->setNearClipDistance(0.01);
	mCamera->setFarClipDistance(1000.0);

	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0,0,0));

	mCamera->setAspectRatio(Real(vp->getActualWidth())/Real(vp->getActualHeight()));

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}

void Lab7::SetupScene()
{
	mScene->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
	mScene->setFog(FOG_EXP2, ColourValue(0,0,0), 0.002);
	mScene->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	//Floor
	Plane p;
	p.normal = Ogre::Vector3(0,1,0);
	p.d = 0;
	MeshManager::getSingleton().createPlane("floorPlane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, p, 200000, 200000, 20, 20, true, 1, 9000, 9000, Ogre::Vector3::UNIT_Z);

	Entity* ent = mScene->createEntity("floor", "floorPlane");
	ent->setMaterialName("Examples/Rockwall");
	mScene->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

	OgreBulletCollisions::CollisionShape* Shape;
	Shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0,1,0), 0);

	OgreBulletDynamics::RigidBody* DefalutPlaneBody = new OgreBulletDynamics::RigidBody("basePlane", mWorld);
	DefalutPlaneBody->setStaticShape(Shape, 0.1, 0.8);

	mShape.push_back(Shape);
	mRigidBody.push_back(DefalutPlaneBody);

	//Box
	for(int i = 0; i < 5; i++)
	{
		for(int a = 0; a < 10; a++)
		{
			Ogre::Vector3 size = Ogre::Vector3::ZERO;
			Ogre::Vector3 position = Ogre::Vector3(i, 5*a, 0);

			Entity* ent = mScene->createEntity("Shape" + StringConverter::toString(mNumEnity), "cube.mesh");
			ent->setCastShadows(true);
			AxisAlignedBox boundingB = ent->getBoundingBox();
			size = boundingB.getSize();
			size = size/2*0.95;

			ent->setMaterialName("Examples/BumpyMetal");
			SceneNode* node = mScene->getRootSceneNode()->createChildSceneNode();
			node->scale(0.05,0.05,0.05);
			size *= 0.05;
			node->attachObject(ent);

			OgreBulletCollisions::BoxCollisionShape* sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);

			OgreBulletDynamics::RigidBody* defaultBody = new OgreBulletDynamics::RigidBody("defaultBoxRigidBody"+StringConverter::toString(mNumEnity), mWorld);

			defaultBody->setShape(node, sceneBoxShape, 0.6, 0.6, 1.0, position, Quaternion(0,0,0,1));
	
			mNumEnity++;

			mShape.push_back(sceneBoxShape);
			mRigidBody.push_back(defaultBody);
		}
	}

	Light* lightDir = mScene->createLight("MainLight");
	lightDir->setType(Light::LightTypes::LT_DIRECTIONAL);
	lightDir->setDirection(5,-5,-5);
	lightDir->setDiffuseColour(ColourValue(1,1,1));
	lightDir->setCastShadows(true);
}

void Lab7::SetupOIS()
{
	LogManager::getSingletonPtr()->logMessage("*** Initialized OIS ***");

	OIS::ParamList pl;
	size_t windowHandle;
	std::ostringstream windowHandleStr;

	mWindow->getCustomAttribute("WINDOW", &windowHandle);
	windowHandleStr << windowHandle;
	
	pl.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));

	mInput = OIS::InputManager::createInputSystem(pl);
	mKeyboard = static_cast<OIS::Keyboard*>(mInput->createInputObject(OIS::OISKeyboard, true));
	mKeyboard->setEventCallback(this);
	mMouse = static_cast<OIS::Mouse*>(mInput->createInputObject(OIS::OISMouse, true));
	mMouse->setEventCallback(this);

	int left, top;
	unsigned int width, height, depth;

	mWindow->getMetrics(width, height, depth, left, top);

	const OIS::MouseState& ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

#include <windows.h>

INT WINAPI WinMain(HINSTANCE hi, HINSTANCE phi, LPSTR lp, INT nsc)
{
	Lab7 app;

	try
	{
		app.start();
	}
	catch(Ogre::Exception& e)
	{
		MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}