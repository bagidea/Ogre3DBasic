#include "Lab9.h"

#include <OgreEntity.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreMeshManager.h>
#include <OgreConfigFile.h>
#include <OgreException.h>

Lab9::Lab9()
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
		mGUI(0),
		mPlatform(0),
		isGUI(false)
{
	std::fill_n(chk, 7, 0);
}

Lab9::~Lab9()
{
	if(mCameraMan) delete mCameraMan;

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

bool Lab9::start()
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

	while (secIt.hasMoreElements())
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

	mWindow = mRoot->initialise(true, "La9 Window");

	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mScene = mRoot->createSceneManager(ST_EXTERIOR_CLOSE);

	std::cout << "Setup Camera : ";
	SetupCamera();
	std::cout << "Success" << std::endl;

	std::cout << "Setup Scene : ";
	SetupScene();
	std::cout << "Success" << std::endl;

	std::cout << "Setup OIS : ";
	SetupOIS();
	std::cout << "Success" << std::endl;

	std::cout << "Setup MyGUI : ";
	SetupMyGUI();
	std::cout << "Success" << std::endl;

	int left, top;
	unsigned int width, height, depth;

	mWindow->getMetrics(width, height, depth, left, top);

	const OIS::MouseState& ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;

	WindowEventUtilities::addWindowEventListener(mWindow, this);

	std::cout << "********************************" << std::endl;

	mRoot->addFrameListener(this);
	mRoot->startRendering();

	return true;
}

bool Lab9::frameRenderingQueued(const FrameEvent& e)
{
	if(mWindow->isClosed()) return false;

	mKeyboard->capture();
	mMouse->capture();

	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;
	
	if(isGUI)
		mGUI->injectFrameEntered(e.timeSinceLastFrame);
	else
		mCameraMan->frameRenderingQueued(e);

	return true;
}

bool Lab9::keyPressed(const OIS::KeyEvent& e)
{
	if(isGUI)
		MyGUI::InputManager::getInstance().injectKeyPress(MyGUI::KeyCode::Enum(e.key), e.text);

	mCameraMan->injectKeyDown(e);

	if(mKeyboard->isKeyDown(OIS::KC_SPACE))
	{
		isGUI = !isGUI;
		mGUI->setVisiblePointer(isGUI);
	}

	return true;
}

bool Lab9::keyReleased(const OIS::KeyEvent& e)
{
	if(isGUI)
		MyGUI::InputManager::getInstance().injectKeyRelease(MyGUI::KeyCode::Enum(e.key));
	
	mCameraMan->injectKeyUp(e);

	return true;
}

bool Lab9::mouseMoved(const OIS::MouseEvent& e)
{
	MyGUI::InputManager::getInstance().injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);

	if(!isGUI)
		mCameraMan->injectMouseMove(e);

	return true;
}

bool Lab9::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	if(isGUI)
		MyGUI::InputManager::getInstance().injectMousePress(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));
	else
		mCameraMan->injectMouseDown(e, id);

	return true;
}

bool Lab9::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	if(isGUI)
		MyGUI::InputManager::getInstance().injectMouseRelease(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));
	else
		mCameraMan->injectMouseUp(e, id);

	return true;
}

void Lab9::SetupCamera()
{
	mCamera = mScene->createCamera("MainCam");
	mCamera->setPosition(0,100,-500);
	mCamera->lookAt(0,0,0);
	mCamera->setNearClipDistance(0.01);
	mCamera->setFarClipDistance(5000.0);

	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue::Black);

	mCamera->setAspectRatio(Real(vp->getActualWidth())/Real(vp->getActualHeight()));

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);

	CompositorManager::getSingleton().addCompositor(vp, "Bloom");
	CompositorManager::getSingleton().addCompositor(vp, "Glass");
	CompositorManager::getSingleton().addCompositor(vp, "Old TV");
	CompositorManager::getSingleton().addCompositor(vp, "Embossed");
	CompositorManager::getSingleton().addCompositor(vp, "Sharpen Edges");
	CompositorManager::getSingleton().addCompositor(vp, "Invert");
	CompositorManager::getSingleton().addCompositor(vp, "Posterize");
}

void Lab9::SetupScene()
{
	mScene->setAmbientLight(ColourValue(0.2,0.2,0.2));
	mScene->setFog(FOG_EXP2, ColourValue::Black, 0.001);

	//Model
	Entity* model = mScene->createEntity("ogrehead.mesh");
	Entity* model2 = mScene->createEntity("robot.mesh");

	StaticGeometry* sg = mScene->createStaticGeometry("Model");

	AxisAlignedBox bound = model->getBoundingBox();

	for(int i = 0; i < 1000; i++)
	{
		Quaternion q;
		q.FromAngleAxis(Degree(Math::RangeRandom(0, 359)), Ogre::Vector3::UNIT_Y);

		Real size = Math::RangeRandom(0.0,100.0)/100.0;
		Real ran = Math::RangeRandom(0,1);

		if(ran < 0.5)
			sg->addEntity(model, Ogre::Vector3(Math::RangeRandom(0.0,2000.0)-1000.0, 30.0, Math::RangeRandom(0.0,2000.0)-1000), q, Ogre::Vector3(size*2,size*2,size*2));
		else
			sg->addEntity(model2, Ogre::Vector3(Math::RangeRandom(0.0,2000.0)-1000.0, 0.0, Math::RangeRandom(0.0,2000.0)-1000), q, Ogre::Vector3(size*2,size*2,size*2));
	}

	sg->build();

	//Floor
	Plane plane;
	plane.normal = Ogre::Vector3(0,1,0);
	plane.d = 0;

	MeshManager::getSingleton().createPlane("Plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 200000, 200000, 20, 20, true, 1, 1000, 1000, Ogre::Vector3::UNIT_Z);

	Entity* floor = mScene->createEntity("Floor", "Plane");
	floor->setMaterialName("BagIdeaMaterial/Sample3");

	SceneNode* floorNode = mScene->getRootSceneNode()->createChildSceneNode();
	floorNode->setPosition(0,0,0);
	floorNode->attachObject(floor);

	//Light
	Light* lightDir = mScene->createLight("Directional Light");
	lightDir->setType(Light::LightTypes::LT_DIRECTIONAL);
	lightDir->setDirection(-5, -10, 10);
	lightDir->setDiffuseColour(ColourValue::White);
	lightDir->setSpecularColour(ColourValue::White);
}

void Lab9::SetupOIS()
{
	OIS::ParamList pl;
	size_t windowHandlle = 0;
	std::ostringstream windowHandlleStr;

	mWindow->getCustomAttribute("WINDOW", &windowHandlle);
	windowHandlleStr << windowHandlle;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHandlleStr.str()));

	mInput = OIS::InputManager::createInputSystem(pl);
	mKeyboard = static_cast<OIS::Keyboard*>(mInput->createInputObject(OIS::OISKeyboard, true));
	mKeyboard->setEventCallback(this);
	mMouse = static_cast<OIS::Mouse*>(mInput->createInputObject(OIS::OISMouse, true));
	mMouse->setEventCallback(this);
}

void Lab9::SetupMyGUI()
{
	mPlatform = new MyGUI::OgrePlatform();
	mPlatform->initialise(mWindow, mScene);
	mGUI = new MyGUI::Gui();
	mGUI->initialise();

	MyGUI::ButtonPtr button1 = mGUI->createWidget<MyGUI::Button>("Button", 10, 10, 100, 20, MyGUI::Align::Default, "Main", "1_BTN");
	button1->setCaption("Bloom");
	button1->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab9::click);

	MyGUI::ButtonPtr button2 = mGUI->createWidget<MyGUI::Button>("Button", 10, 30, 100, 20, MyGUI::Align::Default, "Main", "2_BTN");
	button2->setCaption("Glass");
	button2->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab9::click);

	MyGUI::ButtonPtr button3 = mGUI->createWidget<MyGUI::Button>("Button", 10, 50, 100, 20, MyGUI::Align::Default, "Main", "3_BTN");
	button3->setCaption("Old TV");
	button3->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab9::click);

	MyGUI::ButtonPtr button4 = mGUI->createWidget<MyGUI::Button>("Button", 10, 70, 100, 20, MyGUI::Align::Default, "Main", "4_BTN");
	button4->setCaption("Embossed");
	button4->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab9::click);

	MyGUI::ButtonPtr button5 = mGUI->createWidget<MyGUI::Button>("Button", 10, 90, 100, 20, MyGUI::Align::Default, "Main", "5_BTN");
	button5->setCaption("Sharpen Edges");
	button5->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab9::click);

	MyGUI::ButtonPtr button6 = mGUI->createWidget<MyGUI::Button>("Button", 10, 110, 100, 20, MyGUI::Align::Default, "Main", "6_BTN");
	button6->setCaption("Invert");
	button6->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab9::click);

	MyGUI::ButtonPtr button7 = mGUI->createWidget<MyGUI::Button>("Button", 10, 130, 100, 20, MyGUI::Align::Default, "Main", "7_BTN");
	button7->setCaption("Posterize");
	button7->eventMouseButtonClick += MyGUI::newDelegate(this, &Lab9::click);

	MyGUI::StaticTextPtr txt = mGUI->createWidget<MyGUI::StaticText>("StaticText", 10, 150, 100, 20, MyGUI::Align::Center, "Main");
	txt->setCaption("#FFFFFF-Press Space-");


	mGUI->setVisiblePointer(isGUI);
}

void Lab9::click(MyGUI::WidgetPtr param)
{
	if(strcmp(param->getName().c_str(), "1_BTN") == 0)
	{
		if(!chk[0])
		{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Bloom", true);
			param->setColour(MyGUI::Colour(1.0,1.0,0.2));
		}else{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Bloom", false);
			param->setColour(MyGUI::Colour::White);
		}

		chk[0] = !chk[0];
	}
	else if(strcmp(param->getName().c_str(), "2_BTN") == 0)
	{
		if(!chk[1])
		{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Glass", true);
			param->setColour(MyGUI::Colour(1.0,1.0,0.2));
		}else{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Glass", false);
			param->setColour(MyGUI::Colour::White);
		}

		chk[1] = !chk[1];
	}
	else if(strcmp(param->getName().c_str(), "3_BTN") == 0)
	{
		if(!chk[2])
		{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Old TV", true);
			param->setColour(MyGUI::Colour(1.0,1.0,0.2));
		}else{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Old TV", false);
			param->setColour(MyGUI::Colour::White);
		}

		chk[2] = !chk[2];
	}
	else if(strcmp(param->getName().c_str(), "4_BTN") == 0)
	{
		if(!chk[3])
		{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Embossed", true);
			param->setColour(MyGUI::Colour(1.0,1.0,0.2));
		}else{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Embossed", false);
			param->setColour(MyGUI::Colour::White);
		}

		chk[3] = !chk[3];
	}
	else if(strcmp(param->getName().c_str(), "5_BTN") == 0)
	{
		if(!chk[4])
		{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Sharpen Edges", true);
			param->setColour(MyGUI::Colour(1.0,1.0,0.2));
		}else{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Sharpen Edges", false);
			param->setColour(MyGUI::Colour::White);
		}

		chk[4] = !chk[4];
	}
	else if(strcmp(param->getName().c_str(), "6_BTN") == 0)
	{
		if(!chk[5])
		{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Invert", true);
			param->setColour(MyGUI::Colour(1.0,1.0,0.2));
		}else{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Invert", false);
			param->setColour(MyGUI::Colour::White);
		}

		chk[5] = !chk[5];
	}
	else if(strcmp(param->getName().c_str(), "7_BTN") == 0)
	{
		if(!chk[6])
		{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Posterize", true);
			param->setColour(MyGUI::Colour(1.0,1.0,0.2));
		}else{
			CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Posterize", false);
			param->setColour(MyGUI::Colour::White);
		}

		chk[6] = !chk[6];
	}
}

#include <iostream>

int main(int argc, char* argv[])
{
	Lab9 app;

	try
	{
		std::cout << "********** Start Lab8 **********" << std::endl;
		app.start();
	}
	catch(Ogre::Exception& e)
	{
		std::cout << "An exception has occured! : " << e.getFullDescription().c_str() << std::endl;
	}

	return 0;
}