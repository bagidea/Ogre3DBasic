VS 2012

Project Configuration: 

Switch to "All Configurations"

Configuration Properties - General - Character Set: 
Use Multi-Byte Character Set

Configuration Properties - Debugging - Command: 
$(OGRE_HOME)\Bin\$(Configuration)\$(ProjectName).exe

Configuration Properties - Debugging - Working Directory: 
$(OGRE_HOME)\Bin\$(Configuration)

Configuration Properties - C/C++ - General - Additional Include Directories:
$(OGRE_HOME)\include;$(OGRE_HOME)\include\OIS;$(OGRE_HOME)\include\OGRE;$(OGRE_HOME)\Samples\Common\include;$(OGRE_HOME)\boost;$(OGRE_HOME)\include\OGRE\Overlay

Configuration Properties - Linker - General - Additional Library Directories:
$(OGRE_HOME)\lib\$(Configuration);$(OGRE_HOME)\boost\lib

Configuration Properties - Build Events - Post-Build Event -Command Line: 
copy "$(OutDir)\$(TargetFileName)" "$(OGRE_HOME)\Bin\$(Configuration)"

Switch configuration to "Active(Debug)"

Configuration Properties - Linker - Input - Additional Dependencies: 
OgreMain_d.lib 
OIS_d.lib 
OgreOverlay_d.lib
	
Switch configuration to "Active(Release)"

Configuration Properties - Linker - Input - Additional Dependencies: 
OgreMain.lib
OIS.lib
OgreOverlay.lib

--------------------------- Other ----------------------------

-----[Terrain]------

Input Debug
OgreTerrain_d.lib

Input Release
OgreTerrain.lib

--------------------

------[CEGUI]-------

C/C++
$(CEGUI_HOME)\cegui\include;$(CEGUI_HOME)\build\cegui\include

Linker
$(CEGUI_HOME)\build\lib

Input Debug
CEGUIBase-0_d.lib
CEGUIOgreRenderer-0_d.lib

Input Release
CEGUIBase-0.lib
CEGUIOgreRenderer-0.lib

--------------------

------[MyGui]-------

C/C++
$(MYGUI_HOME)\MyGUIEngine\include;$(MYGUI_HOME)\Platforms\Ogre\OgrePlatform\include 

Linker
$(MYGUI_HOME)\lib\Debug;$(MYGUI_HOME)\lib\Release

Input Debug
MyGUIEngine.lib
MyGUI.OgrePlatform.lib

Input Release
MyGUIEngine_d.lib
MyGUI.OgrePlatform_d.lib

--------------------