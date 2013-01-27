===============================
Neko Project II - Debug Edition
===============================

__([Windows binaries are here] (http://thpatch.net/wiki/Neko_Project_II_-_Debug_Edition))__




About
-----

This is a fork of the PC-98 emulator [Xnp2] (https://github.com/nonakap/xnp2) with extended debugging features. The aim is to achieve a level of debugging comfort high enough to thoroughly modify PC-98 binaries and add new features to them.

In addition to the assembly, memory and register viewers for emulated code already present in Xnp2, this version also includes:

* single-step execution
* code breakpoints
* Shift-JIS memory display
* memory searching

What's still missing:

* memory modification
* memory breakpoints
* assembly modification
* read emulated files from a directory instead of using .hdi images

As Xnp2 (and, most importantly, its debugging features already present) doesn't come with a cross-platform method for GUI coding, these extended features are currently _exclusive to the Win9x build_. After all that refactoring, it probably doesn't even build on Linux anymore...

This is, and probably ever will be, only tested against the Touhou Project games 01-05. While it is fully compatible with them, both the debugging functions and the rather outdated code base make it __noticeably laggier__ (on average about 10 fps slower) than the recommended [fmgen build] (http://www.shrinemaiden.org/forum/index.php?topic=11117.0). Thus, this edition is not what you're looking for if you want to actually _play_ the games.



Building
--------

### Windows
Visual C++ 2010 is required for building. I don't really see a reason in maintaining project configurations for previous versions. Once you've understood property sheets, you really don't want to go back.

You'll also need:

* the **DirectX SDK**. Apparently, this is included in the Windows SDK now.
Starting with the June 2010 release, the required `ddraw.lib` is no longer included. You can get it from [here] (http://thpatch.net/wiki/File:Ddraw_lib.zip). Since we're only using legacy functionality though, any old version of the SDK will do.
 
* **[YASM] (http://yasm.tortall.net/)** to build the assembly code.
Get the VS2010 archive and install it globally into Visual Studio's *BuildCustomizations* directory.
 

 
### Linux
(via http://eru-cool.blogspot.com/2012/02/emulador-pc-98-nativo-para-linux-para.html)

You need the development libraries of `GTK2`, `SDL` and `SDL_mixer`, as well as `nasm` to build the assembly code. On Ubuntu, the required packages are:

    build-essential nasm libsdl1.2-dev libsdl-mixer1.2-dev libgtk2.0-dev libxxf86vm-dev
	
Also, the Touhou games require the build to be configured with IA-32 support. From the root directory, the build commands are:

    cd x11
    ./autogen.sh --enable-ia32
    make


	
Debugging
---------
The debugging window can be accessed under `Tools → Debug utility`.

##### Unassemble
* Current instruction is always at the top of the window


Contributing
------------
Don't be shy. This fork is basically a sandbox anyway, so feel free to send pull requests.

I especially appreciate if someone is willing to waste the time to:
 * port the legacy DirectDraw code to Direct3D, or even
 * port the entire Win32 API code to GTK2, similar to the Linux version.
