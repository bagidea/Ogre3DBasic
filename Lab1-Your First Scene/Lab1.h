#ifndef LAB1_H
#define LAB1_H
 
#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>
 
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkCameraMan.h>
 
class Lab1
	:	public Ogre::WindowEventListener,
		public Ogre::FrameListener,
		public OIS::KeyListener,
		public OIS::MouseListener
{
public:
	Lab1();
	virtual ~Lab1();
 
	bool go();
 
private:
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);
 
	virtual bool keyPressed(const OIS::KeyEvent& ke);
	virtual bool keyReleased(const OIS::KeyEvent& ke);

	virtual bool mouseMoved(const OIS::MouseEvent& me);
	virtual bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id);

	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);

	bool SetupBase(Ogre::String title);
	void SetupCamera();
	void SetupScene();
	void SetupOIS();
 
	Ogre::Root* mRoot;
	Ogre::String mResourcesCfg;
	Ogre::String mPluginsCfg;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera* mCamera;

	OgreBites::SdkCameraMan* mCameraMan;
 
	OIS::InputManager* mInputMgr;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;
};
 
#endif