#include "Lab4.h"

#include <OgreEntity.h>
#include <OgreParticleSystem.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreConfigFile.h>
#include <OgreException.h>

Lab4::Lab4()
	:	mRoot(0),
		mPlugins(StringUtil::BLANK),
		mResources(StringUtil::BLANK),
		mWindow(0),
		mScene(0),
		mCamera(0),
		mInput(0),
		mKeyboard(0),
		mMouse(0),
		degree(180)
{
}

Lab4::~Lab4()
{
	WindowEventUtilities::removeWindowEventListener(mWindow, this);

	if(mInput)
	{
		mInput->destroyInputObject(mKeyboard);
		mInput->destroyInputObject(mMouse);

		InputManager::destroyInputSystem(mInput);
		mInput = 0;
	}

	delete mRoot;
}

bool Lab4::go()
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

	String name, locType, secName;
	ConfigFile::SectionIterator secIt = cf.getSectionIterator();

	while(secIt.hasMoreElements())
	{
		secName = secIt.peekNextKey();
		ConfigFile::SettingsMultiMap* settings = secIt.getNext();
		ConfigFile::SettingsMultiMap::iterator it;

		for(it = settings->begin(); it != settings->end(); it++)
		{
			locType = it->first;
			name = it->second;

			ResourceGroupManager::getSingleton().addResourceLocation(name, locType, secName);
		}
	}

	if(!(mRoot->restoreConfig() || mRoot->showConfigDialog()))
		return false;

	mWindow = mRoot->initialise(true, "Lab4 Window");

	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mScene = mRoot->createSceneManager(ST_EXTERIOR_CLOSE);

	SetupCamera();
	SetupScene();
	SetupOIS();

	WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
	mRoot->startRendering();

	return true;
}

bool Lab4::frameRenderingQueued(const FrameEvent& e)
{
	if(mWindow->isClosed()) return false;

	mKeyboard->capture();
	mMouse->capture();

	if(mKeyboard->isKeyDown(KC_ESCAPE)) return false;

	update(e);

	return true;
}

void Lab4::SetupCamera()
{
	mCamera = mScene->createCamera("MainCam");
	mCamera->setPosition(Ogre::Vector3(0,0,-200));
	mCamera->lookAt(Ogre::Vector3(0,0,0));
	mCamera->setNearClipDistance(0.01);
	mCamera->setFarClipDistance(10000.0);

	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0,0,0));

	//mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
	mCamera->setAutoAspectRatio(true);
}

void Lab4::SetupScene()
{
	mScene->setAmbientLight(ColourValue(0.2,0.2,0.2));

	Entity* model = mScene->createEntity("ogrehead.mesh");
	SceneNode* modelNode = mScene->getRootSceneNode()->createChildSceneNode("model");
	modelNode->yaw(Degree(180));
	modelNode->attachObject(model);

	Ogre::SceneNode* particlenode= mScene->getRootSceneNode()->createChildSceneNode();
	
	Ogre::ParticleSystem* ps1= mScene->createParticleSystem("particle1","Examples/GreenyNimbus");
	Ogre::ParticleSystem* ps2= mScene->createParticleSystem("particle2","Examples/PurpleFountain");
	Ogre::ParticleSystem* ps3= mScene->createParticleSystem("particle3","Examples/Aureola");
	
	particlenode->attachObject(ps1);
	particlenode->attachObject(ps2);
	particlenode->attachObject(ps3);

	Light* light = mScene->createLight("MainLight");
	light->setType(Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(0.5,-1,1));

	mScene->setSkyBox(true, "Examples/EarlyMorningSkyBoxNoFog");
}

void Lab4::SetupOIS()
{
	LogManager::getSingletonPtr()->logMessage("*** Initialized OIS ***");

	ParamList pl;
	size_t windowHandle;
	std::ostringstream windowHandleStr;

	mWindow->getCustomAttribute("WINDOW", &windowHandle);
	windowHandleStr << windowHandle;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));

	mInput = InputManager::createInputSystem(pl);
	mKeyboard = static_cast<Keyboard*>(mInput->createInputObject(OISKeyboard, false));
	mMouse = static_cast<Mouse*>(mInput->createInputObject(OISMouse, false));

	int left, top;
	unsigned int width, height, depth;

	mWindow->getMetrics(width, height, depth, left, top);

	const MouseState& ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void Lab4::update(const FrameEvent& e)
{
	mScene->getSceneNode("model")->resetOrientation();
	mScene->getSceneNode("model")->yaw(Degree(degree));

	degree += 0.5;

	if(degree >= 360)
		degree = 0;
}

#include "windows.h"

INT WINAPI WinMain(HINSTANCE hi, HINSTANCE phi, LPSTR lp, INT nsc)
{
	Lab4 app;

	try
	{
		app.go();
	}
	catch(Ogre::Exception& e)
	{
		MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}