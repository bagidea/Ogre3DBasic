#include "Lab1.h"
 
#include <OgreEntity.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreException.h>
 
Lab1::Lab1()
	:	mRoot(0),
		mResourcesCfg(Ogre::StringUtil::BLANK),
		mPluginsCfg(Ogre::StringUtil::BLANK),
		mWindow(0),
		mSceneMgr(0),
		mCamera(0),
		mCameraMan(0),
		mInputMgr(0),
		mMouse(0),
		mKeyboard(0)
{}
 
Lab1::~Lab1()
{
	if (mCameraMan) delete mCameraMan;

	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
 
	delete mRoot;
}
 
bool Lab1::go()
{
	if(!SetupBase("Lab1 Window"))
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
 
bool Lab1::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
	if (mWindow->isClosed()) return false;
 
	mKeyboard->capture();
	mMouse->capture();
 
	if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;

	mCameraMan->frameRenderingQueued(fe);
 
	return true;
}

bool Lab1::keyPressed(const OIS::KeyEvent& ke)
{
  mCameraMan->injectKeyDown(ke);
  return true;
}
 
bool Lab1::keyReleased(const OIS::KeyEvent& ke)
{
  mCameraMan->injectKeyUp(ke);
  return true;
}

bool Lab1::mouseMoved(const OIS::MouseEvent& me)
{
  mCameraMan->injectMouseMove(me);
  return true;
}
 
bool Lab1::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
  mCameraMan->injectMouseDown(me, id);
  return true;
}
 
bool Lab1::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
  mCameraMan->injectMouseUp(me, id);
  return true;
}
 
void Lab1::windowResized(Ogre::RenderWindow* rw)
{
	int left, top;
	unsigned int width, height, depth;
 
	rw->getMetrics(width, height, depth, left, top);
 
	const OIS::MouseState& ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}
 
void Lab1::windowClosed(Ogre::RenderWindow* rw)
{
	if(rw == mWindow)
	{
		if(mInputMgr)
		{
			mInputMgr->destroyInputObject(mMouse);
			mInputMgr->destroyInputObject(mKeyboard);
 
			OIS::InputManager::destroyInputSystem(mInputMgr);
			mInputMgr = 0;
		}
	}
}

bool Lab1::SetupBase(Ogre::String title)
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
 
	while (secIt.hasMoreElements())
	{
		Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator it;
 
		for (it = settings->begin(); it != settings->end(); ++it)
		{
			locType = it->first;
			name = it->second;
 
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
		}
	}
 
	if (!(mRoot->restoreConfig() || mRoot->showConfigDialog()))
		return false;
 
	mWindow = mRoot->initialise(true, title);
 
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
 
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}

void Lab1::SetupCamera()
{
	mCamera = mSceneMgr->createCamera("MainCam");
	mCamera->setPosition(0, 47, 222);
	//mCamera->lookAt(0, 0, -300);
	mCamera->setNearClipDistance(0.01);
	mCamera->setFarClipDistance(1000.0);
 
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
 
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}

void Lab1::SetupScene()
{
	// Create Scene
	Ogre::Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");
	Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ogreNode->attachObject(ogreEntity);

	Ogre::Entity* ogreEntity2 = mSceneMgr->createEntity("ogrehead.mesh");
	Ogre::SceneNode* ogreNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(84, 48, 0));
	ogreNode2->attachObject(ogreEntity2);

	Ogre::Entity* ogreEntity3 = mSceneMgr->createEntity("ogrehead.mesh");
	Ogre::SceneNode* ogreNode3 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ogreNode3->setPosition(0, 104, 0);
	ogreNode3->setScale(2, 1.2, 1); 
	ogreNode3->attachObject(ogreEntity3);

	Ogre::Entity* ogreEntity4 = mSceneMgr->createEntity("ogrehead.mesh");
	Ogre::SceneNode* ogreNode4 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ogreNode4->setPosition(-84, 48, 0);
	ogreNode4->roll(Ogre::Degree(-90));
	ogreNode4->attachObject(ogreEntity4);
 
	mSceneMgr->setAmbientLight(Ogre::ColourValue(.5, .5, .5));
 
	Ogre::Light* light = mSceneMgr->createLight("MainLight");
	light->setPosition(20, 80, 50);
}

void Lab1::SetupOIS()
{
	// OIS
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

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	Lab1 app;
 
	try
	{
		app.go();
	}
	catch(Ogre::Exception& e)
	{
		MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!",	MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}
 
	return 0;
}