#ifndef LAB4_H
#define LAB4_H

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

using namespace Ogre;
using namespace OIS;

class Lab4
	:	public WindowEventListener,
		public FrameListener
{
public:
	Lab4();
	virtual ~Lab4();

	bool go();
private:
	virtual bool frameRenderingQueued(const FrameEvent& e);

	void SetupCamera();
	void SetupScene();
	void SetupOIS();

	void update(const FrameEvent& e);

	Root* mRoot;
	String mPlugins;
	String mResources;
	RenderWindow* mWindow;
	SceneManager* mScene;
	Camera* mCamera;

	InputManager* mInput;
	Keyboard* mKeyboard;
	Mouse* mMouse;

	Real degree;
};

#endif
