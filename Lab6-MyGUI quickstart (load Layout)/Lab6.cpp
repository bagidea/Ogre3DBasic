#include "Lab6.h"

#include <OgreEntity.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreConfigFile.h>
#include <OgreException.h>

Lab6::Lab6()
	:	mRoot(0),
		mPlugins(StringUtil::BLANK),
		mResources(StringUtil::BLANK),
		mWindow(0),
		mScene(0),
		mCamera(0),
		mInput(0),
		mKeyboard(0),
		mMouse(0),
		mGUI(0),
		mPlatform(0),
		isEnd(false),
		Dev(0.1)
{}


Lab6::~Lab6()
{
	WindowEventUtilities::removeWindowEventListener(mWindow, this);

	if(mInput)
	{
		mInput->destroyInputObject(mKeyboard);
		mInput->destroyInputObject(mMouse);

		OIS::InputManager::destroyInputSystem(mInput);
		mInput = 0;
	}

	mGUI->shutdown();
	delete mGUI;
	mGUI = 0;
	mPlatform->shutdown();
	delete mPlatform;
	mPlatform = 0;

	delete mRoot;
}

bool Lab6::start()
{
#ifdef _DEBUG
	mPlugins = "plugins_d.cfg";
	mResources = "resources_d.cfg";
#else
	mPlugins = "plugins.cfg";
	mResources = "resource.cfg";
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

	if(!(mRoot->restoreConfig() || mRoot->showConfigDialog())) return false;

	mWindow = mRoot->initialise(true, "Lab6 Window");

	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mScene = mRoot->createSceneManager(ST_GENERIC);

	SetupCamera();
	SetupScene();
	SetupOIS();
	SetupMyGUI();

	WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
	mRoot->startRendering();

	return true;
}

bool Lab6::frameRenderingQueued(const FrameEvent& e)
{
	if(mWindow->isClosed()) return false;

	mKeyboard->capture();
	mMouse->capture();

	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;

	if(isEnd) return false;

	mGUI->injectFrameEntered(e.timeSinceLastFrame);
	
	mScene->getSceneNode("Model")->yaw(Degree(Dev));

	return true;
}

bool Lab6::keyPressed(const OIS::KeyEvent& e)
{
	MyGUI::InputManager::getInstance().injectKeyPress(MyGUI::KeyCode::Enum(e.key), e.text);

	return true;
}

bool Lab6::keyReleased(const OIS::KeyEvent& e)
{
	MyGUI::InputManager::getInstance().injectKeyRelease(MyGUI::KeyCode::Enum(e.key));

	return true;
}

bool Lab6::mouseMoved(const OIS::MouseEvent& e)
{
	MyGUI::InputManager::getInstance().injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);

	return true;
}

bool Lab6::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	MyGUI::InputManager::getInstance().injectMousePress(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));

	return true;
}

bool Lab6::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	MyGUI::InputManager::getInstance().injectMouseRelease(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));

	return true;
}

void Lab6::SetupCamera()
{
	mCamera = mScene->createCamera("MainCam");
	mCamera->setPosition(0, 100, -150);
	mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(0.01);
	mCamera->setFarClipDistance(1000.0);
 
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
 
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void Lab6::SetupScene()
{
	Entity* model = mScene->createEntity("ogrehead.mesh");
	SceneNode* modelNode = mScene->getRootSceneNode()->createChildSceneNode("Model");
	modelNode->yaw(Degree(180));
	modelNode->attachObject(model);

	mScene->setAmbientLight(ColourValue(0.2,0.2,0.2));

	Light* light = mScene->createLight("MainLight");
	light->setPosition(0, 50, -100);
}

void Lab6::SetupOIS()
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

void Lab6::SetupMyGUI()
{
	mPlatform = new MyGUI::OgrePlatform();
	mPlatform->initialise(mWindow, mScene);
	mGUI = new MyGUI::Gui();
	mGUI->initialise();

	MyGUI::LayoutManager::getInstance().loadLayout("SampleMyGUI.layout");

	MyGUI::ButtonPtr button = mGUI->findWidget<MyGUI::Button>("L_BTN");
	button->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab6::click);

	MyGUI::ButtonPtr button2 = mGUI->findWidget<MyGUI::Button>("R_BTN");
	button2->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab6::click);

	MyGUI::ButtonPtr button3 = mGUI->findWidget<MyGUI::Button>("E_BTN");
	button3->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab6::click);

	/*MyGUI::ButtonPtr button = mGUI->createWidget<MyGUI::Button>("Button", 100, 50, 200, 40, MyGUI::Align::Default, "Main", "L_BTN");
	button->setCaption("LEFT");
	button->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab6::click);

	MyGUI::ButtonPtr button2 = mGUI->createWidget<MyGUI::Button>("Button", mWindow->getWidth()-300, 50, 200, 40, MyGUI::Align::Default, "Main", "R_BTN");
	button2->setCaption("RIGHT");
	button2->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab6::click);

	MyGUI::ButtonPtr button3 = mGUI->createWidget<MyGUI::Button>("Button", (mWindow->getWidth()/2)-100, mWindow->getHeight()-90, 200, 40, MyGUI::Align::Default, "Main", "E_BTN");
	button3->setCaption("EXIT");
	button3->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab6::click);*/
}

void Lab6::click(MyGUI::WidgetPtr param)
{
	if(strcmp(param->getName().c_str(), "L_BTN") == 0)
		Dev = -0.1;
	if(strcmp(param->getName().c_str(), "R_BTN") == 0)
		Dev = 0.1;
	else if(strcmp(param->getName().c_str(), "E_BTN") == 0)
		isEnd = true;
}

#include <windows.h>

INT WINAPI WinMain(HINSTANCE hi, HINSTANCE phi, LPSTR lp, INT nsc)
{
	Lab6 app;

	try
	{
		app.start();
	}
	catch(Ogre::Exception& e)
	{
		MessageBox(NULL, e.getFullDescription().c_str(), "An exception occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}