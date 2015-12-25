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
#include <OgreRenderTargetListener.h>
 
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
    public Ogre::RenderTargetListener,
    public OIS::KeyListener,
    public OIS::MouseListener
{
public:
  TutorialApplication();
  ~TutorialApplication();
 
  void go();
 
private:
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
 
  bool mShutdown;
 
  Ogre::Root* mRoot;
  Ogre::Camera* mCamera;
  Ogre::SceneManager* mSceneMgr;
  Ogre::RenderWindow* mWindow;
  Ogre::String mResourcesCfg;
  Ogre::String mPluginsCfg;
 
  OgreBites::SdkCameraMan* mCameraMan;
 
  // CEGUI
  bool setupCEGUI();
 
  CEGUI::OgreRenderer* mRenderer;
 
  // OIS
  OIS::Mouse* mMouse;
  OIS::Keyboard* mKeyboard;
  OIS::InputManager* mInputMgr;
 
  //////////////////////
  // Tutorial Section //
  //////////////////////
  virtual void preRenderTargetUpdate(const Ogre::RenderTargetEvent& rte);
  virtual void postRenderTargetUpdate(const Ogre::RenderTargetEvent& rte);
 
  Ogre::MovablePlane* mPlane;
  Ogre::Entity* mPlaneEntity;
  Ogre::SceneNode* mPlaneNode;
  Ogre::Rectangle2D* mMiniscreen;
};
 
#endif