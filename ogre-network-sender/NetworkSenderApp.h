#pragma once

#include <vector>

#include <Ogre.h>
#include <OIS/OIS.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "ExampleApplication.h"

using namespace Ogre;
using boost::asio::ip::tcp;


class NetworkSenderApp: public ExampleApplication
					  , public FrameListener
{
public:
	NetworkSenderApp(void);
	virtual ~NetworkSenderApp(void);

	bool frameStarted(const FrameEvent& evt);

protected:
	void createScene();
	void _createAxes(int);
	void _createGrid(int);
	void _createLight();

protected:
	SceneNode *mGridNode, *mBallNode, *mLightNode;
	AnimationState *mAnimState, *mAnimState2;

};
