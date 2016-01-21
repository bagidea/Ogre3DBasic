#ifndef LAB8_H
#define LAB8_H

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkCameraMan.h>

using namespace Ogre;

class Lab8
	:	public WindowEventListener,
		public FrameListener,
		public OIS::KeyListener,
		public OIS::MouseListener
{
public:
	Lab8();
	virtual ~Lab8();

	bool start();
private:
	virtual bool frameRenderingQueued(const FrameEvent& e);
	
	virtual bool keyPressed(const OIS::KeyEvent& e);
	virtual bool keyReleased(const OIS::KeyEvent& e);

	virtual bool mouseMoved(const OIS::MouseEvent& e);
	virtual bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id);

	void SetupCamera();
	void SetupScene();
	void SetupOIS();

	Root* mRoot;
	String mPlugins;
	String mResources;
	RenderWindow* mWindow;
	SceneManager* mScene;
	Camera* mCamera;

	OgreBites::SdkCameraMan* mCameraMan;

	OIS::InputManager* mInput;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;
};

#endif