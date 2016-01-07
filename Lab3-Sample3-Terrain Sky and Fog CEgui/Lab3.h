#ifndef LAB3_H
#define LAB3_H

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkCameraMan.h>

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

using namespace Ogre;
using namespace OIS;
using namespace std;

class Lab3
	:	public WindowEventListener,
		public FrameListener,
		public KeyListener,
		public MouseListener
{
public:
	Lab3();
	virtual ~Lab3();

	bool start();

private:
	virtual bool frameRenderingQueued(const FrameEvent& e);
	void update(const FrameEvent& e);

	virtual bool keyPressed(const KeyEvent& e);
	virtual bool keyReleased(const KeyEvent& e);

	virtual bool mouseMoved(const MouseEvent& e);
	virtual bool mousePressed(const MouseEvent& e, MouseButtonID id);
	virtual bool mouseReleased(const MouseEvent& e, MouseButtonID id);

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

	OgreBites::SdkCameraMan* mCameraMan;

	void defineTerrain(long x, long y);
	void initBlendMaps(Ogre::Terrain* terrain);
	void configureTerrainDefaults(Ogre::Light* light);
 
	bool mTerrainsImported;
	TerrainGroup* mTerrainGroup;
	TerrainGlobalOptions* mTerrainGlobals;

	bool setupCEGUI();

	CEGUI::OgreRenderer* mRenderer;
};

#endif

/*material Examples/EarlyMorningSkyBoxNoFog
{
	technique
	{
		pass
		{
			lighting off
			depth_write 
			fog_override true none

			texture_unit
			{
				cubic_texture early_morning.jpg separateUV
				tex_address_mode clamp
			}
		}
	}
}*/