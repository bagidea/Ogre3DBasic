#ifndef LAB9_H
#define LAB9_H

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkCameraMan.h>

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

using namespace Ogre;

class Lab9
	:	public WindowEventListener,
		public FrameListener,
		public OIS::KeyListener,
		public OIS::MouseListener
{
public:
	Lab9();
	virtual ~Lab9();

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
	void SetupMyGUI();

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

	bool isGUI;
	bool chk[7];

	void click(MyGUI::WidgetPtr param);

	MyGUI::Gui* mGUI;
	MyGUI::OgrePlatform* mPlatform;
};

#endif