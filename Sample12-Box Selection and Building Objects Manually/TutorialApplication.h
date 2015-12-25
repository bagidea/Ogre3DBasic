#ifndef TutorialApplication_H
#define TutorialApplication_H
 
#include <OgreManualObject.h>
 
class TutorialApplication : public Ogre::ManualObject
{
public:
  TutorialApplication(const Ogre::String& name);
  virtual ~TutorialApplication();
 
  void setCorners(float left, float top, float right, float bottom);
  void setCorners(const Ogre::Vector2& topLeft, const Ogre::Vector2& bottomRight);
};
 
 
#endif /* TutorialApplication_H */