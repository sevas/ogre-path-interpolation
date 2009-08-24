#pragma once

#include <vector>

#include <Ogre.h>
#include <OgreSimpleSpline.h>
#include <OIS/OIS.h>

#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "ExampleApplication.h"

#include "CubicSpline.h"

using namespace Ogre;
using boost::asio::ip::udp;




class NetworkReceiverApp: public ExampleApplication
					  , public FrameListener
{
protected:
    typedef struct
    {
        Vector3 p1, p2, p3, p4;
    } ControlPolygon;


public:
	NetworkReceiverApp(void);
	virtual ~NetworkReceiverApp(void);

	bool frameStarted(const FrameEvent& evt);

	void operator()();

protected:
	void createScene();
	void _createAxes(int);
	void _createGrid(int);
	void _createLight();

	void _startThread();
	void _readPosition();
    void _readPdu(Vector3&, Vector3&,  boost::system::error_code&);
	void _readFloat(udp::socket&,  boost::system::error_code&, float&);
    void _predictSplineControlPolygon(const Vector3, const Vector3, const Vector3 , const Vector3 );
    void _redrawControlPolygon();

protected:
	SceneNode *mGridNode, *mBallNode, *mLightNode;
	AnimationState *mAnimState2;

	boost::thread *mThread;
	volatile bool mRunning, mConnected;

	boost::asio::io_service mIOService;
	udp::socket *mUdpSocket;
	udp::endpoint mUdpRemotePoint;
	boost::system::error_code mSocketError;
	
    Ogre::SimpleSpline mPathSpline;
    Ogre::Real mCurrentInterpolationTime;
    Ogre::uint16 mCurrentInterpolationStep;
    Ogre::Vector3 mCurrentStartPosition, mCurrentStartSpeed;
    Ogre::Vector3 mCurrentTargetPosition, mCurrentTargetSpeed;
    bool mIsMoving, mHasMoved;
    ControlPolygon mControlPolygon;
    std::vector<Ogre::SceneNode*> mControlPoints;
    Ogre::ManualObject *mCurrentPath;

    CubicSpline mPathSpline2;

    Ogre::Log *mNetworkLog;
};
