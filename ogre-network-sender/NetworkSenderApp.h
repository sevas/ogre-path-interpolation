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
	NetworkSenderApp();
	virtual ~NetworkSenderApp(void);

	bool frameStarted(const FrameEvent& evt);
	bool frameEnded_(const FrameEvent& evt);

protected:
	void createScene();
	void _createAxes(int);
	void _createGrid(int);
	void _createLight();

    void _readConfigurationFromFile();

	void _initNetwork();
	void _sendPosition();
	void _sendFloat(float);
    void _sendPdu(const Vector3&, const Vector3&);
	void _sleep(int);

protected:
	SceneNode *mGridNode, *mBallNode, *mLightNode;
	AnimationState *mAnimState, *mAnimState2;

	Vector3 mLastBallPosition, mCurrentSpeed;
	bool mIsMoving, mHasMoved;

	std::string mIpAddress, mUdpPort;
	boost::asio::io_service mIOService;
    Real mSamplingInterval;

	udp::resolver *mUdpResolver;
	udp::resolver::query *mUdpQuery;
	udp::endpoint mUdpReceiverEndpoint;
	udp::socket *mUdpSocket;

	boost::system::error_code mSocketError;
	bool mConnected;
	Timer mTimer;
	Real mTimeSinceLastUpdate;
	Log *mNetworkLog;

	int mRate;
};
