#ifndef TutorialApplication_H
#define TutorialApplication_H
 
#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreEntity.h>
#include <OgreFrameListener.h>
#include <OgreWindowEventUtilities.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <OgreSceneQuery.h>
 
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
 
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>
 
#include <SdkCameraMan.h>
 
class TutorialApplication
  : public Ogre::WindowEventListener,
    public Ogre::FrameListener,
    public OIS::KeyListener,
    public OIS::MouseListener
{
public:
  TutorialApplication();
  ~TutorialApplication();
 
  void go();
 
  enum QueryFlags
  {
    NINJA_MASK = 1 << 0,
    ROBOT_MASK = 1 << 1
  };
 
private:
  bool mShutdown;
 
  Ogre::Root* mRoot;
  Ogre::Camera* mCamera;
  Ogre::SceneManager* mSceneMgr;
  Ogre::RenderWindow* mWindow;
  Ogre::String mResourcesCfg;
  Ogre::String mPluginsCfg;
 
  OgreBites::SdkCameraMan* mCameraMan;
 
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);
 
  virtual bool keyPressed(const OIS::KeyEvent& ke);
  virtual bool keyReleased(const OIS::KeyEvent& ke);
 
  virtual bool mouseMoved(const OIS::MouseEvent& me);
  virtual bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id);
 
  virtual void windowResized(Ogre::RenderWindow* rw);
  virtual void windowClosed(Ogre::RenderWindow* rw);
 
  bool setup();
  bool configure();
  void chooseSceneManager();
  void createCamera();
  void createScene();
  void destroyScene();
  void createFrameListener();
  void createViewports();
  void setupResources();
  void createResourceListener();
  void loadResources();
 
  // CEGUI
  CEGUI::OgreRenderer* mRenderer;
 
  bool setupCEGUI();
 
  // OIS
  OIS::Mouse* mMouse;
  OIS::Keyboard* mKeyboard;
  OIS::InputManager* mInputMgr;
 
  // Terrain
  void setupTerrain(Ogre::Light* light);
  void defineTerrain(long x, long y);
  void initBlendMaps(Ogre::Terrain* terrain);
  void configureTerrainDefaults(Ogre::Light* light);
 
  bool mTerrainsImported;
 
  Ogre::TerrainGroup* mTerrainGroup;
  Ogre::TerrainGlobalOptions* mTerrainGlobals;
 
  //////////////////////
  // Tutorial Section //
  //////////////////////
  void handleCameraCollision();
 
  float mRotSpd;
  bool mMovableFound, mRobotMode;
  bool mLMouseDown, mRMouseDown;
 
  Ogre::SceneNode* mCurObject;
  Ogre::RaySceneQuery* mRayScnQuery;
};
 
#endif