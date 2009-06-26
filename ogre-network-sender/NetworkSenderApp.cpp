#include "precompiled.h"

#include "NetworkSenderApp.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <boost/thread/xtime.hpp>



NetworkSenderApp::NetworkSenderApp(const char *_ipAddress)
	:mAnimState(NULL)
	,mAnimState2(NULL)
	,mSocket(0)
	,mIpAddress(_ipAddress)
{
}
//------------------------------------------------------------------------------
NetworkSenderApp::~NetworkSenderApp(void)
{
	mSocket->close();
}
//------------------------------------------------------------------------------
bool NetworkSenderApp::frameStarted(const FrameEvent& evt)
{
	if (mAnimState)
		mAnimState->addTime(evt.timeSinceLastFrame);

	if (mAnimState2)
		mAnimState2->addTime(evt.timeSinceLastFrame);


	mTimeSinceLastUpdate += evt.timeSinceLastFrame;
	_sendPosition();

	return true;
}
//------------------------------------------------------------------------------
void NetworkSenderApp::createScene()
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

	float totalTime = 4;
	float halfTime  = totalTime/2;

	Animation *anim = mSceneMgr->createAnimation("bouncing ball", totalTime);
	anim->setDefaultInterpolationMode(Animation::IM_SPLINE);
	NodeAnimationTrack *track = anim->createNodeTrack(0, mBallNode);
	TransformKeyFrame *key = track->createNodeKeyFrame(0);
	
	// start
	key->setScale(Vector3(ws, ws, ws));
	key->setTranslate(Vector3(0, 0, 0));    


	// first half
	key = track->createNodeKeyFrame(halfTime * 1./4);
	key->setScale(Vector3(ws, ws, ws));
	key->setTranslate(Vector3(50, 0, -25));

	key = track->createNodeKeyFrame(halfTime * 2./4);
	key->setScale(Vector3(ws, ws, ws));
	key->setTranslate(Vector3(100, 0, 0));

	key = track->createNodeKeyFrame(halfTime * 3./4);
	key->setScale(Vector3(ws, ws, ws));
	key->setTranslate(Vector3(50, 0, 25));

	key = track->createNodeKeyFrame(halfTime);
	key->setScale(Vector3(ws, ws, ws));
	key->setTranslate(Vector3(0, 0, 0));

	// second half
	key = track->createNodeKeyFrame(halfTime + halfTime * 1./4);
	key->setScale(Vector3(ws, ws, ws));
	key->setTranslate(Vector3(-50, 0, -25));

	key = track->createNodeKeyFrame(halfTime + halfTime * 2./4);
	key->setScale(Vector3(ws, ws, ws));
	key->setTranslate(Vector3(-100, 0, 0));

	key = track->createNodeKeyFrame(halfTime + halfTime * 3./4);
	key->setScale(Vector3(ws, ws, ws));
	key->setTranslate(Vector3(-50, 0, 25));

	
	key = track->createNodeKeyFrame(totalTime);
	key->setScale(Vector3(ws, ws, ws));
	key->setTranslate(Vector3(0, 0, 0));

	mAnimState = mSceneMgr->createAnimationState("bouncing ball");
	mAnimState->setEnabled(1);

	_initNetwork();
}
//------------------------------------------------------------------------------
void NetworkSenderApp::_createAxes(int _nUnits)
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

}//------------------------------------------------------------------------------
void NetworkSenderApp::_createGrid(int _nUnits)
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
void NetworkSenderApp::_createLight()
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
//------------------------------------------------------------------------------
void NetworkSenderApp::_initNetwork()
{
	boost::asio::io_service io_service;
	tcp::resolver resolver(io_service);


	tcp::resolver::query query(mIpAddress, "8888");

	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	tcp::resolver::iterator end;

	mSocket = new tcp::socket(io_service);
	
	mSocketError = boost::asio::error::host_not_found;
    mConnected = false;

	//while (!mConnected)
	//{
	mConnected = false;
	while (mSocketError && endpoint_iterator != end)
	{
		mSocket->close();
		mSocket->connect(*endpoint_iterator++, mSocketError);

	}
	if (mSocketError)
		throw boost::system::system_error(mSocketError);

	mConnected = true;
	//}

	mTimer.reset();
}
//------------------------------------------------------------------------------
void NetworkSenderApp::_sendPosition()
{
	if(mConnected)
	{
		
		if (mTimeSinceLastUpdate > 1./60)
		{
			Vector3 pos = mBallNode->getPosition();
			_sendFloat(pos.x);
			_sendFloat(pos.y);
			_sendFloat(pos.z);

			mTimeSinceLastUpdate = 0;
		}
	}
}
//------------------------------------------------------------------------------
void NetworkSenderApp::_sendFloat(float _val)
{
	char arr[4];
	memcpy(arr, &_val, sizeof(_val));

	int n = boost::asio::write((*mSocket)
							  , boost::asio::buffer(arr, sizeof(_val))
							  , boost::asio::transfer_all()
							  , mSocketError);
	mConnected = (n == 4);
	
}
//------------------------------------------------------------------------------
void NetworkSenderApp::_sleep(int _ms)
{
	// sleep 20ms
	boost::xtime sleeptime;
	boost::xtime_get(&sleeptime, boost::TIME_UTC);
	sleeptime.nsec += 1000000 * _ms;
	boost::thread::sleep(sleeptime);
}
//------------------------------------------------------------------------------
