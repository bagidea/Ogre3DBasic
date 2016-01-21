#include "Lab8.h"

#include <OgreEntity.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreMeshManager.h>
#include <OgreConfigFile.h>
#include <OgreException.h>

Lab8::Lab8()
	:	mRoot(0),
		mPlugins(StringUtil::BLANK),
		mResources(StringUtil::BLANK),
		mWindow(0),
		mScene(0),
		mCamera(0),
		mCameraMan(0),
		mInput(0),
		mKeyboard(0),
		mMouse(0)
{}


Lab8::~Lab8()
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

	delete mRoot;
}

bool Lab8::start()
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

	mWindow = mRoot->initialise(true, "Lab8 Window");

	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mScene = mRoot->createSceneManager(ST_GENERIC);

	std::cout << "Setup Camera : ";
	SetupCamera();
	std::cout << "Success" << std::endl;

	std::cout << "Setup Scene : ";
	SetupScene();
	std::cout << "Success" << std::endl;

	std::cout << "Setup OIS : ";
	SetupOIS();
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

bool Lab8::frameRenderingQueued(const FrameEvent& e)
{
	if(mWindow->isClosed()) return false;

	mKeyboard->capture();
	mMouse->capture();

	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;

	mScene->getSceneNode("ModelNode")->yaw(Degree(0.01));
	mScene->getSceneNode("ModelNode2")->yaw(Degree(0.01));

	mCameraMan->frameRenderingQueued(e);

	return true;
}

bool Lab8::keyPressed(const OIS::KeyEvent& e)
{
	mCameraMan->injectKeyDown(e);

	return true;
}

bool Lab8::keyReleased(const OIS::KeyEvent& e)
{
	mCameraMan->injectKeyUp(e);

	return true;
}

bool Lab8::mouseMoved(const OIS::MouseEvent& e)
{
	mCameraMan->injectMouseMove(e);

	return true;
}

bool Lab8::mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	mCameraMan->injectMouseDown(e, id);

	return true;
}

bool Lab8::mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id)
{
	mCameraMan->injectMouseUp(e, id);

	return true;
}


void Lab8::SetupCamera()
{
	mCamera = mScene->createCamera("MainCam");
	mCamera->setPosition(0,150,-200);
	mCamera->lookAt(0,0,0);
	mCamera->setNearClipDistance(0.01);
	mCamera->setFarClipDistance(1000.0);

	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue::Black);

	mCamera->setAspectRatio(Real(vp->getActualWidth())/Real(vp->getActualHeight()));

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}

void Lab8::SetupScene()
{
	mScene->setAmbientLight(ColourValue(0.2,0.2,0.2));

	//Model
	Entity* model = mScene->createEntity("ogrehead.mesh");
	model->setMaterialName("BagIdeaMaterial/Sample1");

	SceneNode* node = mScene->getRootSceneNode()->createChildSceneNode("ModelNode");
	node->setPosition(-55,30,0);
	node->yaw(Degree(180.0));
	node->attachObject(model);

	Entity* modelNormal = mScene->createEntity("ogrehead.mesh");

	SceneNode* node2 = mScene->getRootSceneNode()->createChildSceneNode("ModelNode2");
	node2->setPosition(55,30,0);
	node2->yaw(Degree(180.0));
	node2->attachObject(modelNormal);

	//Floor
	Plane plane;
	plane.normal = Ogre::Vector3(0,1,0);
	plane.d = 0;

	MeshManager::getSingleton().createPlane("Plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 100, 100, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

	Entity* floor = mScene->createEntity("Floor", "Plane");
	floor->setMaterialName("BagIdeaMaterial/Sample3");

	SceneNode* floorNode = mScene->getRootSceneNode()->createChildSceneNode();
	floorNode->setPosition(-55,0,0);
	floorNode->attachObject(floor);

	Entity* floorNormal = mScene->createEntity("FloorNarmal", "Plane");
	floorNormal->setMaterialName("BagIdeaMaterial/Sample2");

	SceneNode* floorNode2 = mScene->getRootSceneNode()->createChildSceneNode();
	floorNode2->setPosition(55,0,0);
	floorNode2->attachObject(floorNormal);

	//Light
	Light* lightDir = mScene->createLight("Directional Light");
	lightDir->setType(Light::LightTypes::LT_DIRECTIONAL);
	lightDir->setDirection(-5, -10, 10);
	lightDir->setDiffuseColour(ColourValue::White);
	lightDir->setSpecularColour(ColourValue::White);
}

void Lab8::SetupOIS()
{
	OIS::ParamList pl;
	size_t windowHandle = 0;
	std::ostringstream windowHandleStr;

	mWindow->getCustomAttribute("WINDOW", &windowHandle);
	windowHandleStr << windowHandle;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));

	mInput = OIS::InputManager::createInputSystem(pl);
	mKeyboard = static_cast<OIS::Keyboard*>(mInput->createInputObject(OIS::OISKeyboard, true));
	mKeyboard->setEventCallback(this);
	mMouse = static_cast<OIS::Mouse*>(mInput->createInputObject(OIS::OISMouse, true));
	mMouse->setEventCallback(this);
}

#include <iostream>

int main(int argc, char* argv[])
{
	Lab8 app;

	try
	{
		std::cout << "********** Start Lab8 **********" << std::endl;
		app.start();
	}
	catch(Ogre::Exception& e)
	{
		std::cout << "An exception has occured| : " << e.getFullDescription().c_str() << std::endl;
	}

	return 0;
}