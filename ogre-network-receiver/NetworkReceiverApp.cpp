#include "precompiled.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <boost/format.hpp>

#include "NetworkReceiverApp.h"

NetworkReceiverApp::NetworkReceiverApp(void)
	:mAnimState2(NULL)
	,mThread(0)
	,mUdpSocket(0)
    ,mCurrentInterpolationTime(0)
    ,mIsMoving(false)
    ,mHasMoved(false)
    ,mCurrentStartPosition(Vector3::ZERO)
    ,mCurrentStartSpeed(Vector3::ZERO)
    ,mCurrentInterpolationStep(0)
{
    mTitle = "Receiver";
    mPathSpline.clear();


}
//------------------------------------------------------------------------------
NetworkReceiverApp::~NetworkReceiverApp(void)
{
	mRunning = false;
	//mThread->join();
}
//------------------------------------------------------------------------------
bool NetworkReceiverApp::frameStarted(const FrameEvent& evt)
{
	if (mAnimState2)
		mAnimState2->addTime(evt.timeSinceLastFrame);

    if (mIsMoving)
    {
        //Vector3 newPos = mPathSpline.interpolate(mCurrentInterpolationTime);
        Vector3 newPos = mPathSpline2.evaluate(mCurrentInterpolationTime);
        mBallNode->setPosition(newPos);
        if ((mCurrentInterpolationTime + (evt.timeSinceLastFrame / 0.5)) < 1.0)
        {
            mNetworkLog->logMessage(boost::str(boost::format("[%d] %f : (%.2f, %.2f, %.2f)") 
                                                            % mCurrentInterpolationStep 
                                                            % mCurrentInterpolationTime
                                                            % newPos.x % newPos.y % newPos.z));
            mCurrentInterpolationTime += (evt.timeSinceLastFrame / 0.5) ;
            mCurrentInterpolationStep++;
        }
    }

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

	mSceneMgr->setNormaliseNormalsOnScale(true);

	_createAxes(5);
	_createGrid(5);

	mCamera->setPosition(Vector3(100, 100, 100));
	mCamera->lookAt(Vector3::ZERO);

	_createLight();


	mBallNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Ball Node");
	Entity *ent = mSceneMgr->createEntity("my ball", "sphere.mesh");
	ent->setMaterialName("Objects/Ball");

	ent->getSubEntity(0)->setMaterialName("Examples/CelShading");

	ent->getSubEntity(0)->setCustomParameter(0, Vector4(10.0f, 0.0f, 0.0f, 0.0f));
	ent->getSubEntity(0)->setCustomParameter(1, Vector4(0.0f, 0.5f, 0.0f, 1.0f));
	ent->getSubEntity(0)->setCustomParameter(2, Vector4(0.3f, 0.5f, 0.3f, 1.0f));



	float w = ent->getBoundingBox().getSize().x;
	float ws = 10.0 / w;
	
	mBallNode->attachObject(ent);
	mBallNode->setScale(ws, ws, ws);

    mCurrentPath = mSceneMgr->createManualObject("current spline path");
    mSceneMgr->getRootSceneNode()->attachObject(mCurrentPath);
    
    mControlPoints.resize(4);
    for(int i=0 ; i<4 ; i++)
    {
        Ogre::Entity *pointSphere;
        mControlPoints[i]= mSceneMgr->getRootSceneNode()->createChildSceneNode("Control Point "+Ogre::StringConverter::toString(i));
        pointSphere = mSceneMgr->createEntity("Control Point "+Ogre::StringConverter::toString(i), "geosphere4500.mesh");
        
        switch(i)
        {
        case 0:
            pointSphere->setMaterialName("Objects/StartPathPoint");
            break;
        case 1:
            pointSphere->setMaterialName("Objects/StartControlPoint");
            break;
        case 2:
            pointSphere->setMaterialName("Objects/EndPathPoint");
            break;
        case 3:
            pointSphere->setMaterialName("Objects/EndControlPoint");
            break;
        }
        float w = pointSphere->getBoundingBox().getSize().x;
        float ws = 1.0 / w;

        mControlPoints[i]->attachObject(pointSphere);
        mControlPoints[i]->setScale(ws, ws, ws);
        
    }
    
    _startThread();

}
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
void NetworkReceiverApp::_startThread()
{
	mThread = new boost::thread(boost::ref<NetworkReceiverApp>(*this));
}
//------------------------------------------------------------------------------
void NetworkReceiverApp::operator()()
{

	//Timer timer;
	mRunning = true;
	mUdpRemotePoint = udp::endpoint(udp::v4(), 8888);
	mUdpSocket = new udp::socket(mIOService, mUdpRemotePoint);

	mNetworkLog = LogManager::getSingleton().createLog("network_receiver.log");


	while(mRunning)
	{
        _readPosition();	
	}
}
//------------------------------------------------------------------------------
void NetworkReceiverApp::_readPosition()
{
	boost::system::error_code ec;
	Vector3 position, speed;
    
    _readPdu(position, speed, ec);

    boost::format fmt("[received pdu] position (%.2f  %.2f  %.2f)   speed (%.2f  %.2f  %.2f)");
    fmt % position.x % position.y % position.z % speed.x % speed.y % speed.z;
    mNetworkLog->logMessage(fmt.str());


	if (ec)
		mConnected = false;
    else if(!mHasMoved)
    {
        mCurrentTargetPosition = position;
        mCurrentTargetSpeed = speed;
        mHasMoved = true;
    }
    else
    {
        mCurrentStartPosition = mCurrentTargetPosition;
        mCurrentStartSpeed = mCurrentTargetSpeed;
        mCurrentTargetPosition = position;
        mCurrentTargetSpeed = speed;

        //    
        //boost::format fmt2("[new path] %s : (%.2f %.2f %.2f) (%.2f %.2f %.2f)");
        //fmt2 % "start" % mCurrentStartPosition.x % mCurrentStartPosition.y % mCurrentStartPosition.z
        //               % mCurrentStartSpeed.x    % mCurrentStartSpeed.y   % mCurrentStartSpeed.z;
        //mNetworkLog->logMessage(fmt2.str());
        //fmt2 % "stop" % mCurrentTargetPosition.x % mCurrentTargetPosition.y % mCurrentTargetPosition.z
        //               % mCurrentTargetSpeed.x    % mCurrentTargetSpeed.y   % mCurrentTargetSpeed.z;
        //mNetworkLog->logMessage(fmt2.str());


        mPathSpline2.setStartPoint(mCurrentStartPosition, mCurrentStartSpeed);
        mPathSpline2.setEndPoint(mCurrentTargetPosition, mCurrentTargetSpeed);
        mPathSpline2.calcControlPoints();

        

        _predictSplineControlPolygon(mCurrentStartPosition, mCurrentStartSpeed
                                    ,mCurrentTargetPosition, mCurrentTargetSpeed);

        //mPathSpline.clear();

        //mPathSpline.addPoint(mControlPolygon.p1);
        //mPathSpline.addPoint(mControlPolygon.p2);
        //mPathSpline.addPoint(mControlPolygon.p3);
        //mPathSpline.addPoint(mControlPolygon.p4);

        //mPathSpline.recalcTangents();


        mCurrentInterpolationTime = 0;
        mCurrentInterpolationStep=0;
        _redrawControlPolygon();
        mIsMoving = true;
    }
}
//------------------------------------------------------------------------------
void NetworkReceiverApp::_predictSplineControlPolygon(const Vector3 _startPos
                                                     ,const Vector3 _startSpeed
                                                     ,const Vector3 _targetPos
                                                     ,const Vector3 _targetSpeed)
{
    mControlPolygon.p1 = _startPos;
    mControlPolygon.p4 = _targetPos;

    mControlPolygon.p2 = _startPos  + _startSpeed;
    mControlPolygon.p3 = _targetPos - _targetSpeed;
    
}
//------------------------------------------------------------------------------
void NetworkReceiverApp::_redrawControlPolygon()
{
    //mCurrentPath->clear();
    mCurrentPath->begin("Objects/DynamicLine", Ogre::RenderOperation::OT_LINE_STRIP);
    mCurrentPath->position(mControlPolygon.p1);
    mCurrentPath->position(mControlPolygon.p2);
    mCurrentPath->position(mControlPolygon.p3);
    mCurrentPath->position(mControlPolygon.p4);
    mCurrentPath->end();

    mControlPoints[0]->setPosition(mControlPolygon.p1);
    mControlPoints[1]->setPosition(mControlPolygon.p2);
    mControlPoints[2]->setPosition(mControlPolygon.p3);
    mControlPoints[3]->setPosition(mControlPolygon.p4);

}
//------------------------------------------------------------------------------
void NetworkReceiverApp::_readPdu(Vector3& _oPos, Vector3& _oSpeed, boost::system::error_code &_error)
{
    boost::array<char, 6*sizeof(Real)> buf;
    mUdpSocket->receive_from(boost::asio::buffer(buf), mUdpRemotePoint, 0, _error);

    memcpy(_oPos.ptr(),     buf.c_array(), 3*sizeof(Real));
    memcpy(_oSpeed.ptr(),   buf.c_array()+(3*sizeof(Real)), 3*sizeof(Real));
}
//------------------------------------------------------------------------------
void NetworkReceiverApp::_readFloat(udp::socket&_socket, boost::system::error_code &_error, float &_val)
{
	boost::array<char, 4> buf;
	mUdpSocket->receive_from(boost::asio::buffer(buf), mUdpRemotePoint, 0, _error);

	memcpy(&_val, buf.c_array(), 4);
}
//------------------------------------------------------------------------------
