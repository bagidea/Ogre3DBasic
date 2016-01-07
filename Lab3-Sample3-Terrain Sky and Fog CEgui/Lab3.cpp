#include "Lab3.h"

#include <OgreEntity.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreConfigFile.h>
#include <OgreException.h>

Lab3::Lab3()
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
		mTerrainsImported(0),
		mTerrainGroup(0),
		mTerrainGlobals(0),
		mRenderer(0)
{
}

Lab3::~Lab3()
{
	WindowEventUtilities::removeWindowEventListener(mWindow, this);

	if(mCameraMan) delete mCameraMan;

	if(mInput)
	{
		mInput->destroyInputObject(mKeyboard);
		mInput->destroyInputObject(mMouse);

		InputManager::destroyInputSystem(mInput);
		mInput = 0;
	}

	delete mRoot;
}

bool Lab3::start()
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

	mWindow = mRoot->initialise(true, "Lab3 Window");

	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	mScene = mRoot->createSceneManager(ST_EXTERIOR_CLOSE);

	setupCEGUI();
	SetupCamera();
	SetupScene();
	SetupOIS();

	WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
	mRoot->startRendering();

	return true;
}

bool Lab3::frameRenderingQueued(const FrameEvent& e)
{
	if(mWindow->isClosed()) return false;

	mKeyboard->capture();
	mMouse->capture();

	if(mKeyboard->isKeyDown(KC_ESCAPE)) return false;

	mCameraMan->frameRenderingQueued(e);

	update(e);

	return true;
}

bool Lab3::keyPressed(const KeyEvent& e)
{
	mCameraMan->injectKeyDown(e);

	return true;
}

bool Lab3::keyReleased(const KeyEvent& e)
{
	mCameraMan->injectKeyUp(e);

	return true;
}

bool Lab3::mouseMoved(const MouseEvent& e)
{
	mCameraMan->injectMouseMove(e);

	return true;
}

bool Lab3::mousePressed(const MouseEvent& e, MouseButtonID id)
{
	mCameraMan->injectMouseDown(e, id);

	return true;
}

bool Lab3::mouseReleased(const MouseEvent& e, MouseButtonID id)
{
	mCameraMan->injectMouseUp(e, id);

	return true;
}

void Lab3::SetupCamera()
{
	//Camera
	mCamera = mScene->createCamera("MainCam");
	mCamera->setPosition(Ogre::Vector3(1683, 50, 2116));
	mCamera->lookAt(Ogre::Vector3(1963, 50, 1660));
	mCamera->setNearClipDistance(0.01);
	mCamera->setFarClipDistance(10000.0);

	//Viewport
	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0, 0, 0));
 
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}

void Lab3::SetupScene()
{
	mScene->setAmbientLight(ColourValue(0.2, 0.2, 0.2));
 
	Ogre::Vector3 lightDir(0.0, -0.15, -0.75);
	lightDir.normalise();
 
	Light* light = mScene->createLight("TestLight");
	light->setType(Light::LT_DIRECTIONAL);
	light->setDirection(lightDir);
	light->setDiffuseColour(ColourValue::White);
	light->setSpecularColour(ColourValue(0.98, 0.74, 0.18));
 
	// Fog
	ColourValue fadeColour(0.68, 0.44, 0.08);
	mWindow->getViewport(0)->setBackgroundColour(fadeColour);
 
	mScene->setFog(FOG_EXP2, fadeColour, 0.001);
 
	// Terrain
	mTerrainGlobals = OGRE_NEW TerrainGlobalOptions();
 
	mTerrainGroup = OGRE_NEW TerrainGroup(mScene, Terrain::ALIGN_X_Z, 513, 12000.0);
	mTerrainGroup->setFilenameConvention(String("terrain"), String("dat"));
	mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);

	configureTerrainDefaults(light);
 
	for (long x = 0; x <= 0; ++x)
		for (long y = 0; y <= 0; ++y)
			defineTerrain(x, y);
 
	mTerrainGroup->loadAllTerrains(true);
 
	if (mTerrainsImported)
	{
		TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
 
		while (ti.hasMoreElements())
		{
			Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
		}
	}
 
	mTerrainGroup->freeTemporaryResources();

	mScene->setSkyBox(true, "Examples/EarlyMorningSkyBoxNoFog");

	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* rootWin = wmgr.loadLayoutFromFile("test.layout");
 
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWin);
}

void Lab3::SetupOIS()
{
	LogManager::getSingletonPtr()->logMessage("*** Initialized OIS ***");

	ParamList pl;
	size_t windowHandle = 0;
	ostringstream windowHandleStr;

	mWindow->getCustomAttribute("WINDOW", &windowHandle);
	windowHandleStr << windowHandle;
	pl.insert(make_pair(string("WINDOW"), windowHandleStr.str()));

	mInput = InputManager::createInputSystem(pl);
	mKeyboard = static_cast<Keyboard*>(mInput->createInputObject(OISKeyboard, true));
	mKeyboard->setEventCallback(this);
	mMouse = static_cast<Mouse*>(mInput->createInputObject(OISMouse, true));
	mMouse->setEventCallback(this);

	int left, top;
	unsigned int width, height, depth;

	mWindow->getMetrics(width, height, depth, left, top);

	const MouseState& ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void Lab3::update(const FrameEvent& e)
{
	CEGUI::System::getSingleton().injectTimePulse(e.timeSinceLastFrame);
 
	CEGUI::Window* terrainLabel = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->getChild("TerrainLabel");

	if (mTerrainGroup->isDerivedDataUpdateInProgress())
	{
		if (mTerrainsImported)
		  terrainLabel->setText("Building terrain, please wait...");
		else
		  terrainLabel->setText("Updating textures, patience...");
	}
	else
	{
		terrainLabel->setText("");

		if (mTerrainsImported)
		{
			mTerrainGroup->saveAllTerrains(true);
			mTerrainsImported = false;
		}
	}

	Ogre::Vector3 camPos = mCamera->getPosition();
	Ogre::Ray camRay(Ogre::Vector3(camPos.x, 5000.0, camPos.z),
    Ogre::Vector3::NEGATIVE_UNIT_Y);
 
	TerrainGroup::RayResult result = mTerrainGroup->rayIntersects(camRay);
 
	if (result.terrain)
	{
		Real terrainHeight = result.position.y;
 
		if (camPos.y < (terrainHeight + 10.0))
			mCamera->setPosition(camPos.x, terrainHeight + 10.0, camPos.z);
	}
}

///////////////////////// Terrain Start ///////////////////////
void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
	img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
 
	if (flipX)
		img.flipAroundY();
	if (flipY)
		img.flipAroundX();
}

void Lab3::defineTerrain(long x, long y)
{
	Ogre::String filename = mTerrainGroup->generateFilename(x, y);
 
	bool exists = Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename);
 
	if (exists)
		mTerrainGroup->defineTerrain(x, y);
	else
	{
		Ogre::Image img;
		getTerrainImage(x % 2 != 0, y % 2 != 0, img);
		mTerrainGroup->defineTerrain(x, y, &img);
 
		mTerrainsImported = true;
	}
}
 
void Lab3::initBlendMaps(Ogre::Terrain* terrain)
{
	Ogre::Real minHeight0 = 70;
	Ogre::Real fadeDist0 = 40;
	Ogre::Real minHeight1 = 70;
	Ogre::Real fadeDist1 = 15;
 
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
 
	float* pBlend0 = blendMap0->getBlendPointer();
	float* pBlend1 = blendMap1->getBlendPointer();
 
	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
		  Ogre::Real tx, ty;
 
		  blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
		  Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
		  Ogre::Real val = (height - minHeight0) / fadeDist0;
		  val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
		  *pBlend0++ = val;
 
		  val = (height - minHeight1) / fadeDist1;
		  val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
		  *pBlend1++ = val;
		}
	}
 
	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();
}
 
void Lab3::configureTerrainDefaults(Ogre::Light* light)
{
	mTerrainGlobals->setMaxPixelError(8);
	mTerrainGlobals->setCompositeMapDistance(3000);
 
	mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(mScene->getAmbientLight());
	mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
 
	Ogre::Terrain::ImportData& importData = mTerrainGroup->getDefaultImportSettings();
	importData.terrainSize = 513;
	importData.worldSize = 12000.0;
	importData.inputScale = 600;
	importData.minBatchSize = 33;
	importData.maxBatchSize = 65;
 
	importData.layerList.resize(3);
	importData.layerList[0].worldSize = 100;
	importData.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	importData.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	importData.layerList[1].worldSize = 30;
	importData.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	importData.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	importData.layerList[2].worldSize = 200;
	importData.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	importData.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}
///////////////////////// Terrain End ///////////////////////

bool Lab3::setupCEGUI()
{
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing CEGUI ***");
 
  mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
 
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
 
  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
  CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-10.font");
 
  CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
 
  context.setDefaultFont("DejaVuSans-10");
  context.getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
 
  Ogre::LogManager::getSingletonPtr()->logMessage("Finished");
 
  return true;
}

#include "windows.h"

INT WINAPI WinMain(HINSTANCE hi, HINSTANCE hpi, LPSTR lp, INT nsc)
{
	Lab3 app;

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
