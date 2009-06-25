#pragma once

#include <vector>

#include <Ogre.h>
#include <OIS/OIS.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "ExampleApplication.h"
//#include "ExampleFrameListener.h"

using namespace Ogre;
using boost::asio::ip::tcp;



//class InputListener : public ExampleFrameListener
//{
//	InputListener(RenderWindow* win, Camera* cam, bool bufferedKeys = false, bool bufferedMouse = false,
//		bool bufferedJoy = false ) :
//		mCamera(cam), mTranslateVector(Vector3::ZERO), mWindow(win), mStatsOn(true), mNumScreenShots(0),
//		mMoveScale(0.0f), mRotScale(0.0f), mTimeUntilNextToggle(0), mFiltering(TFO_BILINEAR),
//		mAniso(1), mSceneDetailIndex(0), mMoveSpeed(100), mRotateSpeed(36), mDebugOverlay(0),
//		mInputManager(0), mMouse(0), mKeyboard(0), mJoy(0)
//	{
//		using namespace OIS;
//
//		mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
//
//		LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
//		ParamList pl;
//		size_t windowHnd = 0;
//		std::ostringstream windowHndStr;
//
//		win->getCustomAttribute("WINDOW", &windowHnd);
//		windowHndStr << windowHnd;
//
//		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
//		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
//		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
//		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
//		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
//
//		mInputManager = InputManager::createInputSystem( pl );
//
//		//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
//		mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject( OISKeyboard, bufferedKeys ));
//		mMouse = static_cast<Mouse*>(mInputManager->createInputObject( OISMouse, bufferedMouse ));
//		try {
//			mJoy = static_cast<JoyStick*>(mInputManager->createInputObject( OISJoyStick, bufferedJoy ));
//		}
//		catch(...) {
//			mJoy = 0;
//		}
//
//		//Set initial mouse clipping size
//		windowResized(mWindow);
//
//		showDebugOverlay(true);
//
//		//Register as a Window listener
//		WindowEventUtilities::addWindowEventListener(mWindow, this);
//	}
//};


class NetworkReceiverApp: public ExampleApplication
					  , public FrameListener
{
public:
	NetworkReceiverApp(void);
	virtual ~NetworkReceiverApp(void);

	bool frameStarted(const FrameEvent& evt);

protected:
	void createScene();
	//void createFrameListener();
	void _createAxes(int);
	void _createGrid(int);
	void _createLight();

protected:
	SceneNode *mGridNode, *mBallNode, *mLightNode;
	AnimationState *mAnimState2;

};
