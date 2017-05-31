Neko Project II - Debug Edition (documented!)
===============================

A project to document Neko Project II Debug in English, to eventually make it useful for hacking software other than Touhou.

The build files, including the project definition, are in folder "Win9x" (all required files are included). For a detailed overview of the project's organization, see "Overview.txt".

Build instructions:

Install Visual Studio 2010 (v10.0) or higher. Download **[YASM](http://yasm.tortall.net/)** v1.01 (1.6 and higher will not work!). Install the YASM executable, vsyasm.exe, to the main directory of your VC++ installation, probably C:\Program Files\Microsoft Visual Studio 10.0\VC\bin. Install the other YASM files to C:\Program Files\MSBuild\Microsoft.Cpp\v4.0\BuildCustomizations. Then click [Build -> Build Solution] to build.


There are several build configurations available:


* Release - original distribution optimized for end users


* Debug - includes advanced debugger


* Trap - has break-on-IRQ functionality


* Trace - under this build, the emulator will meticulously log its activity.


Right now the goal is to unify the Debug, Trap, and Trace versions into one super debugger/analyzer.


The authors' original statements follow:


__([Windows binaries are here](https://github.com/nmlgc/np2debug/releases))__

About
-----
This is a fork of the PC-98 emulator [Neko Project II](http://www.yui.ne.jp/np2/) with extended debugging features. The aim is to achieve a level of debugging comfort high enough to thoroughly modify PC-98 binaries and add new features to them.

The Win9x version of the upstream Neko Project II already comes with assembly, memory and register viewers for emulated code, which, however, have to be manually enabled by putting `I286SAVE=true` into its .ini file. In addition to enabling these features by default, this fork also includes:

* single-step execution
* code breakpoints
* Shift-JIS memory display
* memory searching
* memory modification
* memory breakpoints
* stack viewer
* real-directory-to-DOS-drive mounting (experimental)

What's still missing:
* assembly modification by actually writing assembly mnemonics

Since all these features were build on top of the previously existing Win9x code, they are currently *exclusive to the Win9x version*.

This is, and probably ever will be, only tested against the Touhou Project games 01-05. While it is fully compatible with them, both the debugging functions and the rather outdated code base make it **noticeably laggier** (on average about 10 fps slower) than the recommended [fmgen build](http://www.shrinemaiden.org/forum/index.php?topic=11117.0). Thus, this edition is not what you're looking for if you want to actually *play* the games.

Building
--------
### Windows
Visual C++ 2010 is required for building. I don't really see a reason in maintaining project configurations for previous versions. Once you've understood property sheets, you really don't want to go back.

You'll also need:

* the **DirectX SDK**. Apparently, this is included in the Windows SDK now.
Starting with the June 2010 release, the required `ddraw.lib` is no longer included. You can get it from [here](http://thpatch.net/wiki/File:Ddraw_lib.zip). Since we're only using legacy functionality though, any old version of the SDK will do.

* **[YASM](http://yasm.tortall.net/)** to build the assembly code.
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

Directory mounting
------------------
This *very hackish and experimental* feature overrides the DOS system functions called via `INT 21`, providing direct access to "real" files in a directory on your local file system from the emulator, without having to copy them on an image every time.

Doing something like this effectively requires a complete DOS emulation. Thus, the following things are confirmed to be impossible with this method:

* Running programs (.EXE or .COM) from the mounted directory. The DOS EXEC syscall (`INT 21,4B`) would have to call the file opening function via `INT 21` too - which is impossible because `INT 21` is not reentrant.
* Directory structures need to be replicated on the image. This is necessary because DOS still keeps its own directory state. When using `mkdir` or `rmdir` on the command prompt, directories are created and removed both inside the mounted directory and on the image.

… Really, this is only supposed to work for the few cases we need during the creation of the Touhou PC-98 translation patches. Don't expect it to work for anything else.
