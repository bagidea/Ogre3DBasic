#ifndef LAB6_H
#define LAB6_H

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

using namespace Ogre;

class Lab6
	:	public WindowEventListener,
		public FrameListener,
		public OIS::KeyListener,
		public OIS::MouseListener
{
public:
	Lab6();
	virtual ~Lab6();

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

/* SampleMyGUI.layout

<?xml version="1.0" encoding="UTF-8"?>
 
<MyGUI type="Layout">
 
    <Widget type="Button" skin="Button" position="50 50 200 40" align="Default" layer="Main" name="L_BTN" >
        <Property key="Widget_Caption" value="LEFT" />
    </Widget>

    <Widget type="Button" skin="Button" position="50 110 200 40" align="Default" layer="Main" name="R_BTN" >
        <Property key="Widget_Caption" value="RIGHT" />
    </Widget>

    <Widget type="Button" skin="Button" position="50 170 200 40" align="Default" layer="Main" name="E_BTN" >
        <Property key="Widget_Caption" value="EXIT" />
    </Widget>
 
</MyGUI>

*/