#pragma once

#include <vector>

#include <Ogre.h>
#include <OIS/OIS.h>

#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "ExampleApplication.h"

using namespace Ogre;
using boost::asio::ip::udp;


class NetworkSenderApp: public ExampleApplication
					  , public FrameListener
{
public:
	NetworkSenderApp(const char*);
	virtual ~NetworkSenderApp(void);

	bool frameStarted(const FrameEvent& evt);
	bool frameEnded(const FrameEvent& evt);

protected:
	void createScene();
	void _createAxes(int);
	void _createGrid(int);
	void _createLight();

	void _initNetwork();
	void _sendPosition();
	void _sendFloat(float);
	void _sleep(int);

protected:
	SceneNode *mGridNode, *mBallNode, *mLightNode;
	AnimationState *mAnimState, *mAnimState2;

	std::string mIpAddress;
	boost::asio::io_service mIOService;
	/*tcp::resolver *mResolver;
	tcp::resolver::query *mQuery;
	tcp::socket *mSocket;*/
	udp::resolver *mUdpResolver;
	udp::resolver::query *mUdpQuery;
	udp::endpoint mUdpReceiverEndpoint;
	udp::socket *mUdpSocket;

	boost::system::error_code mSocketError;
	bool mConnected;
	Timer mTimer;
	Real mTimeSinceLastUpdate;
	Log *mNetworkLog;
};
