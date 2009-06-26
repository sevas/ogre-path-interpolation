#include "precompiled.h"

#include "NetworkSenderApp.h"



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
	{
		// Create application object
		NetworkSenderApp app(strCmdLine);

		try {
			app.go();
		} catch( Exception& e ) {
			MessageBoxA( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);

		}

		return 0;
	}

#ifdef __cplusplus
}
#endif
