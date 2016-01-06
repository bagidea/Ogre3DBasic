#ifndef SAMPLE_H
#define SAMPLE_H

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

using namespace Ogre;
using namespace OIS;
using namespace std;

class sample
	:	public WindowEventListener,
		public FrameListener
{
public:
	sample();
	virtual ~sample();

	bool start();

private:
	virtual bool frameRenderingQueued(const FrameEvent& e);

	void SetupCamera();
	void SetupScene();
	void SetupOIS();

	Root* mRoot;
	String mPlugins;
	String mResources;
	RenderWindow* mWindow;
	SceneManager* mScene;
	Camera* mCamera;

	InputManager* mInput;
	Keyboard* mKeyboard;
	Mouse* mMouse;
};

#endif