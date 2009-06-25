#include "precompiled.h"

#include "NetworkReceiverApp.h"

NetworkReceiverApp::NetworkReceiverApp(void)
	:mAnimState2(NULL)
{
}
//------------------------------------------------------------------------------
NetworkReceiverApp::~NetworkReceiverApp(void)
{
}
//------------------------------------------------------------------------------
bool NetworkReceiverApp::frameStarted(const FrameEvent& evt)
{

	if (mAnimState2)
		mAnimState2->addTime(evt.timeSinceLastFrame);
	return true;
}
//------------------------------------------------------------------------------
void NetworkReceiverApp::createScene()
{
	const RenderSystemCapabilities* caps = Root::getSingleton().getRenderSystem()->getCapabilities();
	if (!caps->hasCapability(RSC_VERTEX_PROGRAM) || !(caps->hasCapability(RSC_FRAGMENT_PROGRAM)))
	{
		OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your card does not support vertex and fragment programs, so cannot "
			"run this demo. Sorry!", 
			"createScene");
	}

	Viewport *vp = mWindow->getViewport(0);
	vp->setBackgroundColour(ColourValue(0.7, 0.7, 0.7));

	mRoot->addFrameListener(this);

	_createAxes(5);
	_createGrid(5);

	mCamera->setPosition(Vector3(100, 100, 100));
	mCamera->lookAt(Vector3::ZERO);

	_createLight();


	mBallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Ball Node");
	Entity *ent = mSceneMgr->createEntity("my ball", "sphere.mesh");
	ent->setMaterialName("Objects/Ball");
	ent->setNormaliseNormals(true);

	ent->getSubEntity(0)->setMaterialName("Examples/CelShading");

	ent->getSubEntity(0)->setCustomParameter(0, Vector4(10.0f, 0.0f, 0.0f, 0.0f));
	ent->getSubEntity(0)->setCustomParameter(1, Vector4(0.0f, 0.5f, 0.0f, 1.0f));
	ent->getSubEntity(0)->setCustomParameter(2, Vector4(0.3f, 0.5f, 0.3f, 1.0f));



	float w = ent->getBoundingBox().getSize().x;
	float ws = 50.0 / w;
	
	mBallNode->attachObject(ent);
	mBallNode->setScale(ws, ws, ws);

}
////------------------------------------------------------------------------------
//void NetworkReceiverApp::createFrameListener()
//{
//	mFrameListener= new InputListener(mWindow, mCamera);
//	mFrameListener->showDebugOverlay(true);
//	mRoot->addFrameListener(mFrameListener);
//}
//------------------------------------------------------------------------------
void NetworkReceiverApp::_createAxes(int _nUnits)
{
	mGridNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("WorldGrid Node");
	ManualObject *line = mSceneMgr->createManualObject("X Axis");
	int scale = 100;

	line->begin("WorldGrid/XAxis", RenderOperation::OT_LINE_LIST);
	line->position(-_nUnits, 0.0, 0.0);     line->normal(Vector3::UNIT_Y);      line->colour(0.1, 0.0, 0.0);
	line->position( _nUnits, 0.0, 0.0);     line->normal(Vector3::UNIT_Y);      line->colour(1.0, 0.0, 0.0);
	line->end();
	mGridNode->attachObject(line);

	line = mSceneMgr->createManualObject("Y Axis");
	line->begin("WorldGrid/YAxis", RenderOperation::OT_LINE_LIST);
	line->position(0.0, -_nUnits, 0.0);     line->normal(Vector3::UNIT_Y);      line->colour(0.0, 0.1, 0.0);
	line->position(0.0,  _nUnits, 0.0);     line->normal(Vector3::UNIT_Y);      line->colour(0.0, 1.0, 0.0);
	line->end();
	mGridNode->attachObject(line);

	line = mSceneMgr->createManualObject("Z Axis");
	line->begin("WorldGrid/ZAxis", RenderOperation::OT_LINE_LIST);
	line->position( 0.0, 0.0, -_nUnits);     line->normal(Vector3::UNIT_Y);      line->colour(0.0, 0.0, 0.1);
	line->position( 0.0, 0.0,  _nUnits);     line->normal(Vector3::UNIT_Y);      line->colour(0.0, 0.0, 1.0);
	line->end();
	mGridNode->attachObject(line);



	mGridNode->scale(scale, scale, scale);

}
//------------------------------------------------------------------------------
void NetworkReceiverApp::_createGrid(int _nUnits)
{
	float step = 0.1;
	int nUnits = _nUnits;

	ManualObject *grid = mSceneMgr->createManualObject("Grid Lines");
	grid->begin("WorldGrid/Lines", RenderOperation::OT_LINE_LIST);

	grid->colour(0.5, 0.5, 0.5);

	for(int i=1 ; i <= int(nUnits * (1.0/step)) ; i++)
	{
		float offset=i*step;

		// horizontal (parallel to X axis)


		grid->position( -nUnits, 0.0, offset);   grid->normal(Vector3::UNIT_Y); 
		grid->position(  nUnits, 0.0, offset);   grid->normal(Vector3::UNIT_Y); 

		grid->position( -nUnits, 0.0, -offset);   grid->normal(Vector3::UNIT_Y);
		grid->position(  nUnits, 0.0, -offset);   grid->normal(Vector3::UNIT_Y);


		// vertical (parallel to Z axis)
		grid->position( offset, 0.0, -nUnits);   grid->normal(Vector3::UNIT_Y);
		grid->position( offset, 0.0,  nUnits);   grid->normal(Vector3::UNIT_Y);

		grid->position( -offset, 0.0, -nUnits);   grid->normal(Vector3::UNIT_Y);
		grid->position( -offset, 0.0,  nUnits);   grid->normal(Vector3::UNIT_Y);

	}


	grid->end();

	mGridNode->attachObject(grid);


}
//------------------------------------------------------------------------------
void NetworkReceiverApp::_createLight()
{
	Light *light = mSceneMgr->createLight("Main Light");
	light->setType(Light::LT_POINT);
	light->setPosition(Vector3(0, 100, 0));
	light->setSpecularColour(ColourValue::White);
	light->setDiffuseColour(ColourValue::White);
	/*light->setAttenuation(300
	, light->getAttenuationConstant() 
	, light->getAttenuationLinear() 
	, light->getAttenuationQuadric());
	*/

	mLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("light node");
	mLightNode->attachObject(light);

	Animation *anim = mSceneMgr->createAnimation("light track", 4);
	anim->setDefaultInterpolationMode(Animation::IM_SPLINE);

	NodeAnimationTrack *track = anim->createNodeTrack(0, mLightNode);
	TransformKeyFrame *key = track->createNodeKeyFrame(0);
	key->setTranslate(Vector3(100, 100, 0));
	key = track->createNodeKeyFrame(1);
	key->setTranslate(Vector3(100, 100, 100));
	key = track->createNodeKeyFrame(2);
	key->setTranslate(Vector3(-100, 100, 0));
	key = track->createNodeKeyFrame(3);
	key->setTranslate(Vector3(-100, 100, -100));
	key = track->createNodeKeyFrame(4);
	key->setTranslate(Vector3(100, 100, 0));

	mAnimState2 = mSceneMgr->createAnimationState("light track");
	mAnimState2->setEnabled(1);


}