#include "TutorialApplication.h"
 
TutorialApplication::TutorialApplication(const Ogre::String& name)
  : Ogre::ManualObject(name)
{
  setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
  setUseIdentityProjection(true);
  setUseIdentityView(true);
  setQueryFlags(0);
}
 
TutorialApplication::~TutorialApplication()
{
}
 
void TutorialApplication::setCorners(float left, float top, float right, float bottom)
{
  left = 2 * left - 1;
  right = 2 * right - 1;
  top = 1 - 2 * top;
  bottom = 1 - 2 * bottom;
 
  clear();
  begin("Examples/KnotTexture", Ogre::RenderOperation::OT_LINE_STRIP);
  position(left, top, -1);
  position(right, top, -1);
  position(right, bottom, -1);
  position(left, bottom, -1);
  position(left, top, -1);
  end();
 
  setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
}
 
void TutorialApplication::setCorners(
  const Ogre::Vector2& topLeft,
  const Ogre::Vector2& bottomRight)
{
  setCorners(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}