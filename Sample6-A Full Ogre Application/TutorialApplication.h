#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>
 
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

//---------------------------------------------------------------------------
 
class TutorialApplication : public Ogre::WindowEventListener, public Ogre::FrameListener
{
public:
	TutorialApplication();
	virtual ~TutorialApplication();
 
	bool go();
 
private:
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);
 
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);
 
	Ogre::Root* mRoot;
	Ogre::String mResourcesCfg;
	Ogre::String mPluginsCfg;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera* mCamera;
 
	OIS::InputManager* mInputMgr;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;
 
};

//---------------------------------------------------------------------------

#endif // #ifndef __TutorialApplication_h_

//---------------------------------------------------------------------------
