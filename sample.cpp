#include "sample.h"

#include <OgreEntity.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreConfigFile.h>
#include <OgreException.h>

sample::sample()
	:	mRoot(0),
		mPlugins(StringUtil::BLANK),
		mResources(StringUtil::BLANK),
		mWindow(0),
		mScene(0),
		mCamera(0),
		mInput(0),
		mKeyboard(0),
		mMouse(0)
{
}

sample::~sample()
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

bool sample::start()
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
		ConfigFile::SettingsMultiMap* settings = secIt.getNext();
		ConfigFile::SettingsMultiMap::iterator it;

		for(it = settings->begin(); it != settings->end(); it++)
		{
			locType = it->first;
			name = it->second;

			ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
		}
	}

	if(!(mRoot->restoreConfig() || mRoot->showConfigDialog()))
		return false;

	mWindow = mRoot->initialise(true, "sample Window");

	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mScene = mRoot->createSceneManager(ST_GENERIC);

	SetupCamera();
	SetupScene();
	SetupOIS();

	WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
	mRoot->startRendering();

	return true;
}

bool sample::frameRenderingQueued(const FrameEvent& e)
{
	if(mWindow->isClosed()) return false;

	mKeyboard->capture();
	mMouse->capture();

	if(mKeyboard->isKeyDown(KC_ESCAPE)) return false;

	return true;
}

void sample::SetupCamera()
{
}

void sample::SetupScene()
{
}

void sample::SetupOIS()
{
	LogManager::getSingletonPtr()->logMessage("*** Initialized OIS ***");

	ParamList pl;
	size_t windowHandle = 0;
	ostringstream windowHandleStr;

	mWindow->getCustomAttribute("WINDOW", &windowHandle);
	windowHandleStr << windowHandle;
	pl.insert(make_pair(string("WINDOW"), windowHandleStr.str()));

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

#include "windows.h"

INT WINAPI WinMain(HINSTANCE hi, HINSTANCE hpi, LPSTR lp, INT nsc)
{
	sample app;

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
