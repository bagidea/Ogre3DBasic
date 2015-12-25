#include "BaseApplication.h"
 
BaseApplication::BaseApplication()
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
    mInputMgr(0),
    mSelecting(false),
    mLMouseDown(false),
    mRMouseDown(false),
    mRotSpd(0.1),
    mTutorialApplication(0),
    mRayScnQuery(0),
    mVolQuery(0)
{
}
 
BaseApplication::~BaseApplication()
{
  if (mCameraMan) delete mCameraMan;
 
  mSceneMgr->destroyQuery(mVolQuery);
 
  if (mTutorialApplication)
    delete mTutorialApplication;
 
  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
  windowClosed(mWindow);
 
  delete mRoot;
}
 
void BaseApplication::go()
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
 
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& fe)
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
 
bool BaseApplication::keyPressed(const OIS::KeyEvent& ke)
{
  // CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  // context.injectKeyDown((CEGUI::Key::Scan)e.key);
  // context.injectChar((CEGUI::Key::Scan)e.text);
 
  mCameraMan->injectKeyDown(ke);
 
  return true;
}
 
bool BaseApplication::keyReleased(const OIS::KeyEvent& ke)
{
  // CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(
  //   (CEGUI::Key::Scan)e.key);
 
  mCameraMan->injectKeyUp(ke);
 
  return true;
}
 
bool BaseApplication::mouseMoved(const OIS::MouseEvent& me)
{
  CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  context.injectMouseMove(me.state.X.rel, me.state.Y.rel);
 
  // mCameraMan->injectMouseMove(me);
 
  if (mSelecting)
  {
    CEGUI::MouseCursor* mouse = &context.getMouseCursor();
    mStop.x = mouse->getPosition().d_x / (float)me.state.width;
    mStop.y = mouse->getPosition().d_y / (float)me.state.height;
 
    mTutorialApplication->setCorners(mStart, mStop);
  }
 
  if (mLMouseDown)
  {
  }
  else if (mRMouseDown)
  {
    mCamera->yaw(Ogre::Degree(-me.state.X.rel * mRotSpd));
    mCamera->pitch(Ogre::Degree(-me.state.Y.rel * mRotSpd));
  }
 
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
 
bool BaseApplication::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
  CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  context.injectMouseButtonDown(convertButton(id));
 
  // mCameraMan->injectMouseDown(me, id);
 
  if (id == OIS::MB_Left)
  {
    CEGUI::MouseCursor* mouse = &context.getMouseCursor();
    mStart.x = mouse->getPosition().d_x / (float)me.state.width;
    mStart.y = mouse->getPosition().d_y / (float)me.state.height;
    mStop = mStart;
 
    mSelecting = true;
    mTutorialApplication->clear();
    mTutorialApplication->setVisible(true);
 
    mLMouseDown = true;
  }
  else if (id == OIS::MB_Right)
  {
    context.getMouseCursor().hide();
 
    mRMouseDown = true;
  }
 
  return true;
}
 
bool BaseApplication::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
  CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
  context.injectMouseButtonUp(convertButton(id));
 
  // mCameraMan->injectMouseUp(me, id);
 
  if (id == OIS::MB_Left)
  {
    performSelection(mStart, mStop);
    mSelecting = false;
    mTutorialApplication->setVisible(false);
 
    mLMouseDown = false;
  }
  else if (id == OIS::MB_Right)
  {
    context.getMouseCursor().show();
    mRMouseDown = false;
  }
 
  return true;
}
 
void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
  unsigned int width, height, depth;
  int left, top;
  rw->getMetrics(width, height, depth, left, top);
 
  const OIS::MouseState& ms = mMouse->getMouseState();
  ms.width = width;
  ms.height = height;
}
 
void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
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
 
bool BaseApplication::setup()
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
 
bool BaseApplication::configure()
{
  if (!(mRoot->restoreConfig() || mRoot->showConfigDialog()))
  {
    return false;
  }
 
  mWindow = mRoot->initialise(true, "ITutorial");
 
  return true;
}
 
void BaseApplication::chooseSceneManager()
{
  mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
}
 
void BaseApplication::createCamera()
{
  mCamera = mSceneMgr->createCamera("PlayerCam");
 
  mCamera->setPosition(Ogre::Vector3(0, 0, 80));
  mCamera->lookAt(Ogre::Vector3(0, 0, -300));
  mCamera->setNearClipDistance(5);
 
  mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}
 
void BaseApplication::createScene()
{
  mSceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));
 
  for (int i = 0; i < 10; ++i)
  {
    for (int j = 0; j < 10; ++j)
    {
      Ogre::Entity* ent = mSceneMgr->createEntity("robot.mesh");
 
      Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
      node->setPosition(Ogre::Vector3(i * 15, 0, j * 15));
      node->attachObject(ent);
      node->setScale(0.2, 0.2, 0.2);
    }
  }
 
  mCamera->setPosition(-60, 100, -60);
  mCamera->lookAt(60, 0, 60);
 
  mTutorialApplication = new TutorialApplication("TutorialApplication");
  mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mTutorialApplication);
 
  mVolQuery = mSceneMgr->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());
 
  CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
  CEGUI::Window* rootWin = wmgr.loadLayoutFromFile("test.layout");
 
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWin);
 
}
 
void BaseApplication::destroyScene()
{
}
 
void BaseApplication::createFrameListener()
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
 
void BaseApplication::createViewports()
{
  Ogre::Viewport* vp = mWindow->addViewport(mCamera);
  vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
 
  mCamera->setAspectRatio(
    Ogre::Real(vp->getActualWidth()) /
    Ogre::Real(vp->getActualHeight()));
}
 
void BaseApplication::setupResources()
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
 
void BaseApplication::createResourceListener()
{
}
 
void BaseApplication::loadResources()
{
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
 
bool BaseApplication::setupCEGUI()
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
 
void BaseApplication::performSelection(
  const Ogre::Vector2& first,
  const Ogre::Vector2& second)
{
  float left = first.x, right = second.x;
  float top = first.y, bottom = second.y;
 
  if (left > right)
    swap(left, right);
 
  if (top > bottom)
    swap(top, bottom);
 
  if ((right - left) * (bottom - top) < 0.0001)
    return;
 
  Ogre::Ray topLeft = mCamera->getCameraToViewportRay(left, top);
  Ogre::Ray topRight = mCamera->getCameraToViewportRay(right, top);
  Ogre::Ray bottomLeft = mCamera->getCameraToViewportRay(left, bottom);
  Ogre::Ray bottomRight = mCamera->getCameraToViewportRay(right, bottom);
 
  Ogre::Plane frontPlane, topPlane, leftPlane, bottomPlane, rightPlane;
 
  frontPlane = Ogre::Plane(
    topLeft.getOrigin(),
    topRight.getOrigin(),
    bottomRight.getOrigin());
 
  topPlane = Ogre::Plane(
    topLeft.getOrigin(),
    topLeft.getPoint(10),
    topRight.getPoint(10));
 
  leftPlane = Ogre::Plane(
    topLeft.getOrigin(),
    bottomLeft.getPoint(10),
    topLeft.getPoint(10));
 
  bottomPlane = Ogre::Plane(
    bottomLeft.getOrigin(),
    bottomRight.getPoint(10),
    bottomLeft.getPoint(10));
 
  rightPlane = Ogre::Plane(
    topRight.getOrigin(),
    topRight.getPoint(10),
    bottomRight.getPoint(10));
 
  Ogre::PlaneBoundedVolume vol;
 
  vol.planes.push_back(frontPlane);
  vol.planes.push_back(topPlane);
  vol.planes.push_back(leftPlane);
  vol.planes.push_back(bottomPlane);
  vol.planes.push_back(rightPlane);
 
  Ogre::PlaneBoundedVolumeList volList;
  volList.push_back(vol);
 
  mVolQuery->setVolumes(volList);
  Ogre::SceneQueryResult result = mVolQuery->execute();
 
  deselectObjects();
 
  Ogre::SceneQueryResultMovableList::iterator it;
  for (it = result.movables.begin(); it != result.movables.end(); ++it)
  {
    std::cout << (*it)->getParentSceneNode()->getPosition() << std::endl;;
    selectObject(*it);
  }
}
 
void BaseApplication::deselectObjects()
{
  std::list<Ogre::MovableObject*>::iterator it;
 
  for (it = mSelected.begin(); it != mSelected.end(); ++it)
    (*it)->getParentSceneNode()->showBoundingBox(false);
}
 
void BaseApplication::selectObject(Ogre::MovableObject* obj)
{
  obj->getParentSceneNode()->showBoundingBox(true);
  mSelected.push_back(obj);
}
 
void BaseApplication::swap(float& x, float& y)
{
  float temp = x;
  x = y;
  y = temp;
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
  BaseApplication app;
 
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