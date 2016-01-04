#include "Lab2.h"

#include <OgreEntity.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreException.h>

Lab2::Lab2()
	:	mRoot(0),
		mResourcesCfg(Ogre::StringUtil::BLANK),
		mPluginsCfg(Ogre::StringUtil::BLANK),
		mWindow(0),
		mSceneMgr(0),
		mCamera(0),
		mCameraMan(0),
		mInputMgr(0),
		mKeyboard(0),
		mMouse(0)
{}


Lab2::~Lab2()
{
	if(mCameraMan) delete mCameraMan;

	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);

	delete mRoot;
}

bool Lab2::go()
{
	if(!SetupBase("Lab2 Window"))
		return false;

	SetupCamera();
	SetupScene();
	SetupOIS();

	//Start and Listen

	windowResized(mWindow);
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
	mRoot->startRendering();

	return true;
}

bool Lab2::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
	if (mWindow->isClosed()) return false;
 
	mKeyboard->capture();
	mMouse->capture();
 
	if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;

	mCameraMan->frameRenderingQueued(fe);
 
	return true;
}

bool Lab2::keyPressed(const OIS::KeyEvent& e)
{
	mCameraMan->injectKeyDown(e);
	return true;
}

bool Lab2::keyReleased(const OIS::KeyEvent& e)
{
	mCameraMan->injectKeyUp(e);
	return true;
}

bool Lab2::mouseMoved(const OIS::MouseEvent& e)
{
	mCameraMan->injectMouseMove(e);
	return true;
}

bool Lab2::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	mCameraMan->injectMouseDown(e, id);
	return true;
}

bool Lab2::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	mCameraMan->injectMouseUp(e, id);
	return true;
}

void Lab2::windowResized(Ogre::RenderWindow* rw)
{
	int left, top;
	unsigned int width, height, depth;

	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState& ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void Lab2::windowClosed(Ogre::RenderWindow* rw)
{
	if(rw == mWindow)
	{
		if(mInputMgr)
		{
			mInputMgr->destroyInputObject(mKeyboard);
			mInputMgr->destroyInputObject(mMouse);

			OIS::InputManager::destroyInputSystem(mInputMgr);
			mInputMgr = 0;
		}
	}
}

bool Lab2::SetupBase(Ogre::String title)
{
#ifdef _DEBUG
	mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
#else
	mResourcesCfg = "resources.cfg";
	mPluginsCfg = "plugins.cfg";
#endif

	mRoot = new Ogre::Root(mPluginsCfg);

	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);

	Ogre::String name, locType;
	Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();

	while(secIt.hasMoreElements())
	{
		Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator it;

		for(it = settings->begin(); it != settings->end(); it++)
		{
			name = it->second;
			locType = it->first;

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
		}
	}

	if(!(mRoot->restoreConfig() || mRoot->showConfigDialog()))
		return false;

	mWindow = mRoot->initialise(true, title);

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}

void Lab2::SetupCamera()
{
	mCamera = mSceneMgr->createCamera("MainCam");
	mCamera->setPosition(Ogre::Vector3(0, 300 ,500));
	mCamera->lookAt(Ogre::Vector3(0,0,0));
	mCamera->setNearClipDistance(0.01);
	mCamera->setFarClipDistance(1000.0);

	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}

void Lab2::SetupScene()
{
	Ogre::Entity* ninjaEntity = mSceneMgr->createEntity("ninja.mesh");
	ninjaEntity->setCastShadows(true);
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ninjaEntity);

	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

	Ogre::MeshManager::getSingleton().createPlane(
	  "ground",
	  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	  plane, 
	  1500, 1500, 20, 20, 
	  true, 
	  1, 5, 5, 
	  Ogre::Vector3::UNIT_Z
	);

	Ogre::Entity* groundEntity = mSceneMgr->createEntity("ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	groundEntity->setCastShadows(false);
	groundEntity->setMaterialName("Examples/Rockwall");

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	Ogre::Light* spotLight = mSceneMgr->createLight("SpotLight");
	spotLight->setDiffuseColour(0, 0, 1.0);
	spotLight->setSpecularColour(0, 0, 1.0);
	spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	spotLight->setDirection(-1, -1, 0);
	spotLight->setPosition(Ogre::Vector3(200, 200, 0));
	spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));

	Ogre::Light* directionalLight = mSceneMgr->createLight("DirectionalLight");
	directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
	directionalLight->setDiffuseColour(Ogre::ColourValue(.4, 0, 0));
	directionalLight->setSpecularColour(Ogre::ColourValue(.4, 0, 0));
	directionalLight->setDirection(Ogre::Vector3(0, -1, 1));

	Ogre::Light* pointLight = mSceneMgr->createLight("PointLight");
	pointLight->setType(Ogre::Light::LT_POINT);
	pointLight->setDiffuseColour(.3, .3, .3);
	pointLight->setSpecularColour(.3, .3, .3);
	pointLight->setPosition(Ogre::Vector3(0, 150, 250));
}

void Lab2::SetupOIS()
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");

	OIS::ParamList pl;
	size_t windowHandle = 0;
	std::ostringstream windowHandleStr;

	mWindow->getCustomAttribute("WINDOW", &windowHandle);
	windowHandleStr << windowHandle;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));

	mInputMgr = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*>(mInputMgr->createInputObject(OIS::OISKeyboard, true));
	mKeyboard->setEventCallback(this);
	mMouse = static_cast<OIS::Mouse*>(mInputMgr->createInputObject(OIS::OISMouse, true));
	mMouse->setEventCallback(this);
}

#include "windows.h"

INT WINAPI WinMain(HINSTANCE hi, HINSTANCE hpi, LPSTR lp, int nsc)
{
	Lab2 app;

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