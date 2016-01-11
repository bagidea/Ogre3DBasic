#ifndef LAB5_H
#define LAB5_H

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

using namespace Ogre;

class Lab5
	:	public WindowEventListener,
		public FrameListener,
		public OIS::KeyListener,
		public OIS::MouseListener
{
public:
	Lab5();
	virtual ~Lab5();

	bool start();
private:
	virtual bool frameRenderingQueued(const FrameEvent& e);

	virtual bool keyPressed(const OIS::KeyEvent& e);
	virtual bool keyReleased(const OIS::KeyEvent& e);
	virtual bool mouseMoved(const OIS::MouseEvent& e);
	virtual bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id);

	void SetupMyGUI();
	void SetupCamera();
	void SetupScene();
	void SetupOIS();

	Root* mRoot;
	String mPlugins;
	String mResources;
	RenderWindow* mWindow;
	SceneManager* mScene;
	Camera* mCamera;

	OIS::InputManager* mInput;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;

	MyGUI::Gui* mGUI;
	MyGUI::OgrePlatform* mPlatform;

	void click(MyGUI::WidgetPtr param);

	bool isEnd;
	Real Dev;
};

#endif