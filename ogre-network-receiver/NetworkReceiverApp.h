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

using namespace Ogre;
using boost::asio::ip::udp;




class NetworkReceiverApp: public ExampleApplication
					  , public FrameListener
{
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
	void  _readPosition();
	void  _readFloat(udp::socket&,  boost::system::error_code&, float&);

protected:
	SceneNode *mGridNode, *mBallNode, *mLightNode;
	AnimationState *mAnimState2;

	boost::thread *mThread;
	volatile bool mRunning, mConnected;

	boost::asio::io_service mIOService;
	udp::socket *mUdpSocket;
	udp::endpoint mUdpRemotePoint;
	boost::system::error_code mSocketError;
	
	Ogre::SimpleSpline mSpline;
};
