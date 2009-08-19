#include "precompiled.h"

#include "NetworkSenderApp.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#include <boost/format.hpp>


Vector3 getDerive(const Vector3 &_p, const Vector3 &_q, const Real &_dt)
{
    return (_p - _q) / _dt;
}


NetworkSenderApp::NetworkSenderApp(const char *_ipAddress)
	:mAnimState(NULL)
	,mAnimState2(NULL)
	,mUdpSocket(0)
	,mIpAddress(_ipAddress)
	,mConnected(1)
    ,mTimeSinceLastUpdate(0)
	,mTimeSinceLastUpdate(0)
	,mCurrentSpeed(Vector3::ZERO)
	,mIsMoving(false)
    ,mHasMoved(false)
{
}
//------------------------------------------------------------------------------
NetworkSenderApp::~NetworkSenderApp(void)
{
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
   

    Vector3 currentPos = mBallNode->getPosition();

    if (! mIsMoving)
    {
        if(currentPos != mLastBallPosition)
        {
            mHasMoved = true;
            mIsMoving = true;
            mCurrentSpeed = getDerive(currentPos, mLastBallPosition, evt.timeSinceLastFrame);
            mLastBallPosition = currentPos;
        }
    }
    else
    {
        mCurrentSpeed = getDerive(currentPos, mLastBallPosition, evt.timeSinceLastFrame);
        mLastBallPosition = currentPos;
        mTimeSinceLastUpdate += evt.timeSinceLastFrame;
        if (mTimeSinceLastUpdate > 1./10)
            _sendPosition();
         
    }
    

    

	return true;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool NetworkSenderApp::frameEnded_(const FrameEvent& evt)
{
    //if(mLastBallPosition == mBallNode->getPosition())
    //{
    //    if(mIsMoving)
    //    {
    //        mIsMoving = false;
    //        mCurrentSpeed = Vector3::ZERO;
    //    }
    //}
    //else
    //{
    //    if(!mIsMoving)
    //    {
    //        
    //    }
    //}

	//mTimeSinceLastUpdate += evt.timeSinceLastFrame;
	//_sendPosition();

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

	mSceneMgr->setNormaliseNormalsOnScale(true);

	_createAxes(5);
	_createGrid(5);

		mCamera->setPosition(Vector3(100, 100, 100));
	mCamera->lookAt(Vector3::ZERO);

	_createLight();


	mBallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Ball Node");
	Entity *ent = mSceneMgr->createEntity("my ball", "sphere.mesh");
	ent->setMaterialName("Objects/Ball");
//	ent->setNormaliseNormals(true);

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

	// start of track
	mLastBallPosition = Vector3(0, 0, 0);
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

}
//------------------------------------------------------------------------------
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
	mNetworkLog = LogManager::getSingleton().createLog("network.log");


	mUdpResolver = new udp::resolver(mIOService);
	mUdpQuery = new udp::resolver::query(udp::v4(), mIpAddress, "8888");
	
    mUdpReceiverEndpoint = *(mUdpResolver->resolve(*mUdpQuery));

	mUdpSocket = new udp::socket(mIOService);
	mUdpSocket->open(udp::v4());

    mConnected = true;

}
//------------------------------------------------------------------------------
void NetworkSenderApp::_sendPosition()
{
	if(mConnected)
	{		
        Vector3 pos = mBallNode->getPosition();
        Vector3 speed = mCurrentSpeed;

		boost::format fmt("[new pdu] position (%.2f  %.2f  %.2f)   speed (%.2f  %.2f  %.2f)");
		fmt % pos.x % pos.y % pos.z % speed.x % speed.y % speed.z;
		mNetworkLog->logMessage(fmt.str());

        _sendPdu(pos, speed);

		mTimeSinceLastUpdate = 0;
	}
}
//------------------------------------------------------------------------------
void NetworkSenderApp::_sendPdu(const Vector3 &_pos, const Vector3 &_speed)
{
    char arr[6*sizeof(Real)];
    
    memcpy(arr,   _pos.ptr(),   3*sizeof(Real));
    memcpy(arr+3, _speed.ptr(), 3*sizeof(Real));
    
    mUdpSocket->send_to(boost::asio::buffer(arr, 6*sizeof(Real))
                        ,mUdpReceiverEndpoint
                        ,0
                        ,mSocketError);

    if (mSocketError)
    {
        boost::format fmt("socket error : %d");
        fmt % mSocketError;
        mNetworkLog->logMessage(fmt.str());
    }
}
//------------------------------------------------------------------------------
void NetworkSenderApp::_sendFloat(float _val)
{
	char arr[4];
	memcpy(arr, &_val, sizeof(_val));

	//int n = boost::asio::write(*mSocket
	//						  ,boost::asio::buffer(arr, sizeof(_val))
	//						  ,boost::asio::transfer_all()
	//						  ,mSocketError);


	mUdpSocket->send_to(boost::asio::buffer(arr, sizeof(_val))
					   ,mUdpReceiverEndpoint
					   ,0
					   ,mSocketError);


	if (mSocketError)
	{
		boost::format fmt("socket error : %d");
		fmt % mSocketError;
		mNetworkLog->logMessage(fmt.str());
	}
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
