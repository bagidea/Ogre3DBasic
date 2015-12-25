#include "TutorialApplication.h"
 
TutorialApplication::TutorialApplication()
  : mShutdown(false),
    mRoot(0),
    mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mCameraMan(0),
    mRenderer(0),
    mMouse(0),
    mKeyboard(0),
    mInputMgr(0)
{
}
 
TutorialApplication::~TutorialApplication()
{
  if (mCameraMan) delete mCameraMan;
 
  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
  windowClosed(mWindow);
 
  delete mRoot;
}
 
void TutorialApplication::go()
{
  #ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
  #else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
  #endif
 
  if (!setup())
    return;
 
  mRoot->startRendering();
 
  destroyScene();
}
 
bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
  if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
    mShutdown = true;
 
  if (mShutdown)
    return false;
 
  if (mWindow->isClosed())
    return false;
 
  mKeyboard->capture();
  mMouse->capture();
 
  mCameraMan->frameRenderingQueued(fe);
 
  CEGUI::System::getSingleton().injectTimePulse(fe.timeSinceLastFrame);
 
  return true;
}
 
bool TutorialApplication::keyPressed(const OIS::KeyEvent& ke)
{
  // CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  // context.injectKeyDown((CEGUI::Key::Scan)ke.key);
  // context.injectChar((CEGUI::Key::Scan)ke.text);
 
  mCameraMan->injectKeyDown(ke);
 
  return true;
}
 
bool TutorialApplication::keyReleased(const OIS::KeyEvent& ke)
{
  // CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  // context.injectKeyUp((CEGUI::Key::Scan)ke.key);
 
  mCameraMan->injectKeyUp(ke);
 
  return true;
}
 
bool TutorialApplication::mouseMoved(const OIS::MouseEvent& me)
{
  // CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  // context.injectMouseMove(me.state.X.rel, me.state.Y.rel);
 
  mCameraMan->injectMouseMove(me);
 
  return true;
}
 
// Helper function for mouse events
CEGUI::MouseButton convertButton(OIS::MouseButtonID id)
{
  switch (id)
  {
  case OIS::MB_Left:
    return CEGUI::LeftButton;
  case OIS::MB_Right:
    return CEGUI::RightButton;
  case OIS::MB_Middle:
    return CEGUI::MiddleButton;
  default:
    return CEGUI::LeftButton;
  }
}
 
bool TutorialApplication::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
  // CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  // context.injectMouseButtonDown(convertButton(id));
 
  mCameraMan->injectMouseDown(me, id);
 
  return true;
}
 
bool TutorialApplication::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
  // CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  // context.injectMouseButtonUp(convertButton(id));
 
  mCameraMan->injectMouseUp(me, id);
 
  return true;
}
 
void TutorialApplication::windowResized(Ogre::RenderWindow* rw)
{
  unsigned int width, height, depth;
  int left, top;
  rw->getMetrics(width, height, depth, left, top);
 
  const OIS::MouseState& ms = mMouse->getMouseState();
  ms.width = width;
  ms.height = height;
}
 
void TutorialApplication::windowClosed(Ogre::RenderWindow* rw)
{
  if (rw == mWindow)
  {
    if (mInputMgr)
    {
      mInputMgr->destroyInputObject(mMouse);
      mInputMgr->destroyInputObject(mKeyboard);
 
      OIS::InputManager::destroyInputSystem(mInputMgr);
      mInputMgr = 0;
    }
  }
}
 
bool TutorialApplication::setup()
{
  mRoot = new Ogre::Root(mPluginsCfg);
 
  setupResources();
 
  if (!configure())
    return false;
 
  chooseSceneManager();
  createCamera();
  createViewports();
 
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
 
  createResourceListener();
  loadResources();
 
  setupCEGUI();
 
  createScene();
 
  createFrameListener();
 
  return true;
}
 
bool TutorialApplication::configure()
{
  if (!(mRoot->restoreConfig() || mRoot->showConfigDialog()))
  {
    return false;
  }
 
  mWindow = mRoot->initialise(true, "ITutorial");
 
  return true;
}
 
void TutorialApplication::chooseSceneManager()
{
  mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
}
 
void TutorialApplication::createCamera()
{
  mCamera = mSceneMgr->createCamera("PlayerCam");
 
  mCamera->setPosition(Ogre::Vector3(0, 0, 80));
  mCamera->lookAt(Ogre::Vector3(0, 0, -300));
  mCamera->setNearClipDistance(5);
 
  mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}
 
void TutorialApplication::createScene()
{
  mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
 
  createGrassMesh();
 
  mCamera->setPosition(150, 50, 150);
  mCamera->lookAt(0, 0, 0);
 
  Ogre::Entity* robot = mSceneMgr->createEntity("robot", "robot.mesh");
  mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(robot);
 
  Ogre::Plane plane;
  plane.normal = Ogre::Vector3::UNIT_Y;
  plane.d = 0;
 
  Ogre::MeshManager::getSingleton().createPlane(
    "floor",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    plane,
    450.0, 450.0,
    10, 10, true, 1,
    50.0, 50.0,
    Ogre::Vector3::UNIT_Z);
 
  Ogre::Entity* planeEntity = mSceneMgr->createEntity("floor");
  planeEntity->setMaterialName("Examples/GrassFloor");
  planeEntity->setCastShadows(false);
  mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(planeEntity);
 
  Ogre::Entity* grass = mSceneMgr->createEntity("GrassBladesMesh");
  Ogre::StaticGeometry* sg = mSceneMgr->createStaticGeometry("GrassArea");
 
  const int size = 375;
  const int amount = 20;
 
  sg->setRegionDimensions(Ogre::Vector3(size, size, size));
  sg->setOrigin(Ogre::Vector3(-size/2, 0, -size/2));
 
  for (int x = -size/2; x < size/2; x += (size / amount))
  {
    for (int z = -size/2; z < size/2; z += (size / amount))
    {
      Ogre::Real r = size / (float)amount / 2;
      Ogre::Vector3 pos(
	x + Ogre::Math::RangeRandom(-r, r),
	0,
	z + Ogre::Math::RangeRandom(-r, r));
 
      Ogre::Vector3 scale(1, Ogre::Math::RangeRandom(0.9, 1.1), 1);
 
      Ogre::Quaternion orientation;
      orientation.FromAngleAxis(
	Ogre::Degree(Ogre::Math::RangeRandom(0, 359)),
	Ogre::Vector3::UNIT_Y);
 
      sg->addEntity(grass, pos, orientation, scale);
    }
  }
 
  sg->build();
 
  CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
  CEGUI::Window* rootWin = wmgr.loadLayoutFromFile("test.layout");
 
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWin);
 
}
 
void TutorialApplication::destroyScene()
{
}
 
void TutorialApplication::createFrameListener()
{
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
 
  OIS::ParamList pl;
  size_t windowHnd = 0;
  std::ostringstream windowHndStr;
 
  mWindow->getCustomAttribute("WINDOW", &windowHnd);
  windowHndStr << windowHnd;
  pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
 
  mInputMgr = OIS::InputManager::createInputSystem(pl);
 
  mKeyboard = static_cast<OIS::Keyboard*>(
    mInputMgr->createInputObject(OIS::OISKeyboard, true));
  mMouse = static_cast<OIS::Mouse*>(
    mInputMgr->createInputObject(OIS::OISMouse, true));
 
  mKeyboard->setEventCallback(this);
  mMouse->setEventCallback(this);
 
  windowResized(mWindow);
 
  Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
 
  mRoot->addFrameListener(this);
 
  Ogre::LogManager::getSingletonPtr()->logMessage("Finished");
}
 
void TutorialApplication::createViewports()
{
  Ogre::Viewport* vp = mWindow->addViewport(mCamera);
  vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
 
  mCamera->setAspectRatio(
    Ogre::Real(vp->getActualWidth()) /
    Ogre::Real(vp->getActualHeight()));
}
 
void TutorialApplication::setupResources()
{
  Ogre::ConfigFile cf;
  cf.load(mResourcesCfg);
 
  Ogre::String secName, typeName, archName;
  Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();
 
  while (secIt.hasMoreElements())
  {
    secName = secIt.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator setIt;
 
    for (setIt = settings->begin(); setIt != settings->end(); ++setIt)
    {
      typeName = setIt->first;
      archName = setIt->second;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
	archName, typeName, secName);
    }
  }
}
 
void TutorialApplication::createResourceListener()
{
}
 
void TutorialApplication::loadResources()
{
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
 
bool TutorialApplication::setupCEGUI()
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
 
void TutorialApplication::createGrassMesh()
{
  const float width = 25;
  const float height = 30;
  Ogre::Vector3 vec(width / 2, 0, 0);
  Ogre::ManualObject obj("GrassObject");
 
  Ogre::Quaternion quat;
  quat.FromAngleAxis(Ogre::Degree(60), Ogre::Vector3::UNIT_Y);
 
  obj.begin("Examples/GrassBlades", Ogre::RenderOperation::OT_TRIANGLE_LIST);
 
  for (int i = 0; i < 3; ++i)
  {
    obj.position(-vec.x, height, -vec.z);
    obj.textureCoord(0, 0);
    obj.position(vec.x, height, vec.z);
    obj.textureCoord(1, 0);
    obj.position(-vec.x, 0, -vec.z);
    obj.textureCoord(0, 1);
    obj.position(vec.x, 0, vec.z);
    obj.textureCoord(1, 1);
 
    int offset = 4 * i;
    obj.triangle(offset, offset + 3, offset + 1);
    obj.triangle(offset, offset + 2, offset + 3);
 
    vec = quat * vec;
  }
 
  obj.end();
  obj.convertToMesh("GrassBladesMesh");
}
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  #define WIN32_LEAN_AND_MEAN
  #include "windows.h"
#endif
 
#ifdef __cplusplus
  extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
  int main(int argc, char *argv[])
#endif
{
  TutorialApplication app;
 
  try
  {
    app.go();
  }
  catch(Ogre::Exception& e)
  {
    #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      MessageBox(
	NULL,
	e.getFullDescription().c_str(),
	"An exception has occured!",
	MB_OK | MB_ICONERROR | MB_TASKMODAL);
    #else
      std::cerr << "An exception has occured: " <<
	e.getFullDescription().c_str() << std::endl;
    #endif
  }
 
  return 0;
}
 
#ifdef __cplusplus
  }
#endif