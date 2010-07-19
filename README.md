Description
-----------

Spline based implementation for path interpolation. Network client/server implementation using ogre. Uses boost for networking and threading.
This work is not intended to be a final product.

Dependencies
------------

- Ogre 1.6
- boost 1.36 (not tested with newer releases. Should be okay though)

To build the project, you need to set the following environment variables:

- `$(OGRE16_HOME)`
- `$(BOOST_HOME)`


The following DLLs must be copied to the executable directory for a successful run:

- OgreMain(_d).dll
- OIS(_d).dll

Don't forget to modify the `Plugins.cfg` file to point to the folder that holds the Ogre plugin DLLs (RenderSystemGL, PluginCgProgramManager et al.) 


Licence
-------

WTFPL. 
See COPYING file for details.