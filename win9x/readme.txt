
　Neko Project II
                                       NP2 developer team, 1999-2001, 2003, 2004



·Overview

Based on the PC - 9801 VX 21, the major functions of the PC - 9801 series are implemented by software, so allowing
the operation of PC - 98 software.

Neko Project II reproduces the standard hardware of the PC - 9801 series and some BIOS and C bus devices, such as video controllers and sound cards. For this reason, programs installed from floppy disk, such as MS-DOS work, but ROM-based programs such as N88-BASIC and LIO etc. are not supported out of the box. In order to operate these, it is necessary to acquire ROM images from the actual machine.


· Minimum system requirements

486 DX 2 or higher processor
Microsoft - Windows 4.00 (Windows 95 / NT 4)
DirectX 2 or more

· DirectX 3 or higher is required to play sound.

· IE 4.01 or higher is required to use help.
Please click here if IE 4.01 is not available.
Http://www.yui.ne.jp/np 2/help.html


· Recommended system for 80s era games

Celeron 300A or later processor
MS - Windows 98 / MS - Windows 2000
DirectX 3 and above


- Recommended system for 90s era games (which require lots of speed)
  - CPU made after 2006



·How to use

To use Neko Project II, you will need a PC-98 formatted boot floppy disk image.

The following disk image formats are supported:
  - 88 format
  - XDF (solid image) format

When you have obtained a boot floppy image, start np2.exe.

To start the floppy-based software, access the menu by pressing F11 on your keyboard.
After navigating to [FDD 1 -> Open] or [FDD 2 -> Open], insert the disk image.
Reset the machine.

To use the hard disk, create one by navigating the menu [Emulate -> Newdisk].
After creating the HDD image, connect it by navigatin to [HardDisk -> SASI -> 1 -> Open]
Then reset.

Use the F12 key or the middle mouse button to switch the mouse.

The CPU is 80286 (real mode only).
Please be aware that protected mode can not be handled when building environment.
To use V30, set DIP SW 3-8 OFF.
CPU speed can be changed by Configure.

The emulator's settings are saved to np2.ini.



·Menu

Emulate
Reset -- Resets the hardware.
Configure -- Open the Configuration panel.
NewDisk -- Creates a blank disk image.
Font -- Select font file.
Exit -- Quit Neko Project II.

FDD 1
Select -- load disk image into floppy drive 1.
Eject -- unload disk image from floppy drive 1.

FDD 2
Select -- load disk image into floppy drive 2.
Eject -- unload disk image from floppy drive 2.

HardDisk
SASI-I
Select the hard disk image of Open SASI-1.
Remove SASI-1 is returned to the unconfigured state.
SASI-2
Select the Open SASI-2 hard disk image.
Remove SASI-2 is returned to the unconfigured state.

Screen
Window Window mode is set.
FullScreen The full screen mode is set.
Normal Does not rotate the screen.
Left Rotated Rotates the screen 90 degrees to the left.
Right Rotated Rotates the screen 90 degrees to the right.
Disp Vsync Drawing timing (at check Vsync)
Real Palettes Palette update timing (for each raster by check)
No Wait I will not take the timing.
Auto frame The display timing is automatically determined.
Full frame Displays all frames.
1/2 frame Draw once every 2 times.
1/3 frame Draw once every 3 times.
1/4 frame Draw once every 4 times.
Screen option Opens the screen setting.

Device
Keyboard
Keyboard Treats the numeric keypad as a keyboard.
JoyKey - 1 Assigns the numeric keypad to joystick 1.
JoyKey - 2 Assign the numeric keypad to joystick 2.
Mechanical SHIFT Make a note-style mechanical shift.
Mechanical CTRL makes note mechanical control.
Mechanical GRPH to make a note-style mechanical graph key.
F12 = Mouse Assigns the F12 key to mouse switching.
F12 = Copy Assign the F12 key to the Copy key.
F12 = Stop Assigns the F12 key to the Stop key.
F12 = tenkey [=] Assign the F12 key to the numeric keypad = key.
F12 = tenkey [,] Assigns the F12 key to the keys of the numeric keypad.
Sound
Beep off Makes the beep silent.
Beep low Reduce the beep volume.
Beep mid Sets the beep volume to medium.
Beep high Increases the beep volume.
Disable boards FM sound board is not used.
PC-9801-14 Use the music generator board.
PC - 9801 - 26 K Use the PC - 9801 - 26 K board.
PC-9801-86 PC-9801-86 board is used.
We will use PC-9801-26K + 86 PC-9801-26K and 86 board.
PC-9801-86 + Chibi-oto PC-9801-86 We will use board and chibi.
PC - 9801 - 118 Use the PC - 9801 - 118 board.
Speak board We will use the speaker board.
Spark board We use spark board.
AMD-98 AMD-98 is used.
We will use JAST SOUND JAST SOUND.
Seek Sound Take disk seek timing.
Memory
--  use 640 KB memory for main memory only.
--  use 1.6MB main memory and extended memory 1MB.
--  use 3.6MB main memory and extended memory 3MB.
--  use 7.6 MB main memory and extended memory 7 MB.
Mouse Switch -- mouse operation.
Serial option -- Opens the serial setting.
MIDI option -- Opens the MIDI setting.
MIDI panic -- Mutes the MIDI.
Sound option -- Opens the sound setting.

Other
BMP Save Saves the screen under emulation in BMP format.
S 98 logging S 98 Take a log.
Open the Calendar calendar setting.
Shortcut Key
ALT + Enter Assign ALT + Enter to screen switch.
Assign ALT + F4 ALT + F4 to the end of the application.
Clock Disp Displays the clock count.
Frame Disp Displays the number of frames.
Joy Reverse Swap the joystick buttons.
Joy Rapid Places the joystick's button in a state of rapid fire.
Mouse Rapid Puts the button of the mouse in a state of rapid fire.
Use SSTP Use the SSTP protocol.
Help Display help.
About Display version.



· Setting dialog

Configue
CPU
Base Clock Specify the base clock of the CPU.
Multiple Specify the clock magnification of the CPU.
Architecture
PC-9801VM It becomes NEC specification equipped with V30 or earlier CPU.
PC-9801VX NEC specification with i286 or later CPU installed.
PC-286 EPSON compatible machine specification becomes.
Sound
Sampling Rate Specifies the output frequency of the sound.
Buffer Specify the buffer size of the sound.
      Disable Does not use the MMX MMX instruction.
      Comfirm Dialog Displays a confirmation dialog at reset / exit.
      Resume Uses the resume function.

Screen option
Video
LCD PC - 98 -- The notebook is in liquid crystal mode.
Reverse -- Reverse the liquid crystal display.
Use skipline rev -- Correct skip line.
Ratio -- Specify the brightness of the skip line.
Chip -- Specify the GDC chip.
GRCG --Specifies the GRCG chip.
Enable 16 color -- Enables analog 16 color.
Timing
T-RAM -- Specifies text RAM access wait value.
V-RAM -- Specify the video RAM access wait value.
GRCG -- Specifies the access wait value when using GRCG.
RealPalettes Adjust -- Adjust the timing of RealPalettes.

Serial option
COM1 -- Specifies RS-232C serial port.
PC-9861K -- Set the dip switch of PC-9861K.
CH.1 -- Specify the port of channel 1 of PC - 9861K.
CH.2 -- Specify the port of channel 2 of PC - 9861K.

MIDI option
      MPU - PC98II -- Set the dip switch of MPU - PC98II.

Sound option
Mixer -- Sets the volume of various chips.
PC - 9801 - 14 -- Set the volume of PC - 9801 - 14.
26 -- Set the jumper on PC-9801-26K.
86 -- Set the dip switch of PC-9801-86.
Set the jumper of the SPB speaker board.
JoyPad -- Joypad setting.

Calendar
Real -- will always be at the same time as reality.
Use Virtual Calendar -- virtual calendar.



· Disk image

The following image formats are supported.
FDD - D88 format, XDF (solid image) format
SASI - THD format (T 98), HDI format (Anex 86)
SCSI - HDD format (Virtual 98)



· About key setting
In bios.rom and the same folder (depending on the model, this way of writing ...)
Please create a text file named key.txt and fill in the key setting.

Form)
[Keyname] = [key 1] (key 2) (key 3)
[Keyname] [TAB] [key 1] (key 2) (key 3)
Userkey [1 - 2] = [key 1] (key 2) (key 3) ... (key 15)
Full key 0 ~ 9 A ~ Z - ^ \ [[];;,,. / _
STOP COPY ESC TAB BS RET SPC XFER NFER
                 INS DEL RLUP RLDN HMCL HELP
Funk F1 to F10 VF1 to VF5
Numeric keypad [0] - [9] [-] [/] [*] [+] [=] [,] [.]
Shift SHIFT CAPS KANA GRPH CTRL

It is possible to set 3 keys for normal key and 15 keys for userkey at the same time.
Because the [=] key is delimited by =, please separate it with TAB or [EQU].

Example:
        ----------------------- key.txt

W = UP (Try diamond key as a cursor ...)
S = LEFT
D = RIGHT
X = DOWN
[7] = [4] [8] (Try numeric keypad at the same time ...)
[9] = [6] [8]
[1] = [4] [2]
[3] = [6] [2]
Userkey1 = CTRL XFER (CTRL + XFER FEP switching)
Userkey 2 = D O A Z (D + O + A + Z pressed simultaneously)

        -----------------------



· About ROMEO

To use ROMEO, pcidebug.dll is placed in the same folder as np2.exe
Place pcidbg95.vxd (Win95 / 98) / pcidbgnt.sys (WinNT 4/2000 / XP)
Please set useromeo of np2.ini to true.

PC-9801-118 The YMF 288 part of the sound source is output from ROMEO.



Email: np2@yui.ne.jp
Webpage: http://www.yui.ne.jp/np 2/
