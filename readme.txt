// Translation of the original Japanese through Google Translate
// ---- Definition

  Suppression of memory usage for optimization
    MEMOPTIMIZE = 0 to 2

    I expect the following numbers to be set by the CPU
      MEMOPTIMIZE undefined ... Second Cache Enabled Machine after Celeron 333 A
      MEMOPTIMIZE = 0 ... x86
      MEMOPTIMIZE = 1 ... RISC for desktop such as PowerPC
      MEMOPTIMIZE = 2 ... Built-in RISC such as StrongARM


  Optimization of compiler arguments and return values
    Optimization does not work effectively when specifying arguments and return values ​​other than int type
    It is a definition for the compiler.
    Usually we use common.h.
      REG 8 ... UINT 8 type / (sizeof (REG 8)! = 1) In case the upper bit is 0 fill
      REG16 ... UINT16 type / (sizeof (REG16)! = 2) In case of 0fill high order bit
In both cases, 0 fill the value setting side and 0 refer to the reference side.


  Selection of OS language
    OSLANG_SJIS ... Interpret Kanji code of Shift-JIS
    Interpret Kanji code of OSLANG_EUC ... EUC

    OSLINEBREAK_CR ... MacOS "\ r"
    OSLINEBREAK_LF ... Unix "\ n"
    OSLINEBREAK_CRLF ... Windows "\ r \ n"

      * Currently it is set individually in the following source code.
        (Since Windows may be \ r \ n depending on the API and \ n there are cases ...)
        · Common / _ memory.c
        Debugsub.c
        · Statsave.c

    (For selecting milstr.h)
    SUPPORT_ANK ... Link ANK string manipulation function
    Link SUPPORT_SJIS ... SJIS string manipulation function
    Link SUPPORT_EUC ... EUC string manipulation function

      * It is currently defined at all in milstr.h.
        In ver 0.73, it becomes the thing specified by compiler.h after removing the definition of milstr.h.


CPUCORE_IA 32
Adopted IA 32 architecture
Points to note when using i386c
· Use API msgbox () when displaying CPU panic or warning.
Please properly define per compiler.h.
· The architecture without sigsetjmp (3), siglongjmp (3) has the following define
Please add per compiler.h.
------------------------------------------------------------------------------------ --------------------
        #define sigjmp_buf jmp_buf
        # Define sigsetjmp (env, mask) setjmp (env)
        #define siglongjmp (env, val) longjmp (env, val)
------------------------------------------------------------------------------------ --------------------

  CPUSTRUC_MEMWAIT
Move memory weight value to cpucore structure (vramop)

SUPPORT_CRT 15 KHZ
Support horizontal scanning 15.98 kHz (DIPSW 1 - 1)

SUPPORT_CRT 31 KHZ
Supports horizontal scanning 31.47 kHz
Fellow type is this

SUPPORT - PC 9821
Supports PC-9821 extension
Naturally 386 is required.
You also need SUPPORT_CRT31KHZ (to use the high resolution BIOS)

SUPPORT_PC9861K
Supports PC-9861K (RS-232C expansion I / F)

SUPPORT_IDEIO
Support at IDE I / O level
But I can only do ATA lead level ...

SUPPORT_SASI
Supports SASI HDD
If not defined, it always works as an IDE.

SUPPORT_SCSI
Supports SCSI HDD ... It does not work at all

SUPPORT_S 98
S98 Obtain logs

SUPPORT_WAVEREC
Supports wave file export function at Sound level
However, since sound output stops during writing, it is almost for debugging


// ---- screen

  The PC - 9801 series screen size is 641 x 400 as standard.
  As VGA does not fit, in order to force it into VGA, the screen horizontal size is width + extend
.
  8 <width <640
  8 <height <480
  Extend = 0 or 1

Typedef struct {
BYTE * ptr; // VRAM pointer
Int xalign; // x direction offset
Int yalign; // y directional offset
Int width; // width
Int height; // height
UINT bpp; // screen color bit
Int extend; // width extension
} SCRNSURF;

  The surface size is (width + extern) x height.


Const SCRNSURF * scrnmng_surflock (void);
  Screen drawing start

Void scrnmng_surfunlock (const SCRNSURF * surf);
  End of screen rendering (rendered at this timing)


Void scrnmng_setwidth (int posx, int width)
Void scrnmng _ setextend (int extend)
Void scrnmng _ setheight (int posy, int height)
  Changing drawing size
  Change window size
  Change the display area if it is in full screen.
  SCRNSURF should return this value
  Posx, width is a multiple of 8

BOOL scrnmng_isfullscreen (void) ... Not used in NP2 core
  Obtain full screen condition
    Return: non-zero with full screen

BOOL scrnmng_haveextend (void)
  Obtaining the width condition
    Return: Non-zero and horizontal width extension support

UINT scrnmng_getbpp (void)
  Get screen color bit number
    Return: Number of bits (8/16/24/32)

Void scrnmng_palchanged (void)
  Notification of palette update (only on 8bit screen support)

RGB 16 scrnmng_makepal 16 (RGB 32 pal 32)
  Create 16 bit color from RGB 32. (Only for 16bit screen support)



// ---- sound

Sound data of NP 2 is obtained from the following function of sound.c
  Const SINT32 * sound_pcmlock (void)
  Void sound_pcmunlock (const SINT 32 * hdl)


Insert SOUND_CRITICAL semaphore (see sndcsec.c)
SOUNDRESERVE Reserved buffer size in milliseconds
  Specification when interrupt processing sound.
  The maximum delay time of interrupt is specified by SOUNDRESERVE.
  (In the case of Win 9x, there is no interruption because we watch the ring buffer on our own · As specified time
  Sound light comes, so this process was unnecessary)


UINT soundmng_create (UINT rate, UINT ms)
  Securing sound stream
    Input: rate Sampling rate (11025/22050/44100)
            Ms Sampling Buffer Size (ms)
    Return: Number of sampled buffers acquired

            It is not necessary to follow ms (as SDL or buffer size is limited)
            The sound buffer operation of NP 2 uses only the return value.


Void soundmng_destroy (void)
  End of sound stream

Void soundmng_reset (void)
  Resetting the sound stream

Void soundmng_play (void)
  Play sound stream

Void soundmng_stop (void)
  Stop the sound stream

Void soundmng_sync (void)
  Sound stream callback

Void soundmng_setreverse (BOOL reverse)
  Sound stream output reversal setting
    Input: reverse Horizontal inversion with nonzero

BOOL soundmng_pcmplay (UINT num, BOOL loop)
  PCM playback
    Input: num PCM number
            Loop Non-zero loop

Void soundmng_pcmstop (UINT num)
  PCM stop
    Input: num PCM number



// ---- mouse

BYTE mousemng_getstat (SINT 16 * x, SINT 16 * y, int clear)
  Get status of mouse
    Input: clear Reset the counter after feeding the state with non-0
    Output: * x direction count from x clear
            * Y Count in y direction from clear
    Return: bit 7 Left button status (0: pressed)
            Bit 5 State of right button (0: pressed)
· The architecture without sigsetjmp (3), siglongjmp (3) has the following define
Please add per compiler.h.
------------------------------------------------------------------------------------ --------------------
        #define sigjmp_buf jmp_buf
        # Define sigsetjmp (env, mask) setjmp (env)
        #define siglongjmp (env, val) longjmp (env, val)
------------------------------------------------------------------------------------ --------------------

  CPUSTRUC_MEMWAIT
Move memory weight value to cpucore structure (vramop)

SUPPORT_CRT 15 KHZ
Support horizontal scanning 15.98 kHz (DIPSW 1 - 1)

SUPPORT_CRT 31 KHZ
Supports horizontal scanning 31.47 kHz
Fellow type is this

SUPPORT - PC 9821
Supports PC-9821 extension
Naturally 386 is required.
You also need SUPPORT_CRT31KHZ (to use the high resolution BIOS)

SUPPORT_PC9861K
Supports PC-9861K (RS-232C expansion I / F)

SUPPORT_IDEIO
Support at IDE I / O level
But I can only do ATA lead level ...

SUPPORT_SASI
Supports SASI HDD
If not defined, it always works as an IDE.

SUPPORT_SCSI
Supports SCSI HDD ... It does not work at all

SUPPORT_S 98
S98 Obtain logs

SUPPORT_WAVEREC
Supports wave file export function at Sound level
However, since sound output stops during writing, it is almost for debugging


// ---- screen

  The PC - 9801 series screen size is 641 x 400 as standard.
  As VGA does not fit, in order to force it into VGA, the screen horizontal size is width + extend
.
  8 <width <640
  8 <height <480
  Extend = 0 or 1

Typedef struct {
BYTE * ptr; // VRAM pointer
Int xalign; // x direction offset
Int yalign; // y directional offset
Int width; // width
Int height; // height
UINT bpp; // screen color bit
Int extend; // width extension
} SCRNSURF;

  The surface size is (width + extern) x height.


Const SCRNSURF * scrnmng_surflock (void);
  Screen drawing start

Void scrnmng_surfunlock (const SCRNSURF * surf);
  End of screen rendering (rendered at this timing)


Void scrnmng_setwidth (int posx, int width)
Void scrnmng _ setextend (int extend)
Void scrnmng _ setheight (int posy, int height)
  Changing drawing size
  Change window size
  Change the display area if it is in full screen.
  SCRNSURF should return this value
  Posx, width is a multiple of 8

BOOL scrnmng_isfullscreen (void) ... Not used in NP2 core
  Obtain full screen condition
    Return: non-zero with full screen

BOOL scrnmng_haveextend (void)
  Obtaining the width condition
    Return: Non-zero and horizontal width extension support

UINT scrnmng_getbpp (void)
  Get screen color bit number
    Return: Number of bits (8/16/24/32)

Void scrnmng_palchanged (void)
  Notification of palette update (only on 8bit screen support)

RGB 16 scrnmng_makepal 16 (RGB 32 pal 32)
  Create 16 bit color from RGB 32. (Only for 16bit screen support)



// ---- sound

Sound data of NP 2 is obtained from the following function of sound.c
  Const SINT32 * sound_pcmlock (void)
  Void sound_pcmunlock (const SINT 32 * hdl)


Insert SOUND_CRITICAL semaphore (see sndcsec.c)
SOUNDRESERVE Reserved buffer size in milliseconds
  Specification when interrupt processing sound.
  The maximum delay time of interrupt is specified by SOUNDRESERVE.
  (In the case of Win 9x, there is no interruption because we watch the ring buffer on our own · As specified time
  Sound light comes, so this process was unnecessary)


UINT soundmng_create (UINT rate, UINT ms)
  Securing sound stream
    Input: rate Sampling rate (11025/22050/44100)
            Ms Sampling Buffer Size (ms)
    Return: Number of sampled buffers acquired

            It is not necessary to follow ms (as SDL or buffer size is limited)
            The sound buffer operation of NP 2 uses only the return value.


Void soundmng_destroy (void)
  End of sound stream

Void soundmng_reset (void)
  Resetting the sound stream

Void soundmng_play (void)
  Play sound stream

Void soundmng_stop (void)
  Stop the sound stream

Void soundmng_sync (void)
  Sound stream callback

Void soundmng_setreverse (BOOL reverse)
  Sound stream output reversal setting
    Input: reverse Horizontal inversion with nonzero

BOOL soundmng_pcmplay (UINT num, BOOL loop)
  PCM playback
    Input: num PCM number
            Loop Non-zero loop

Void soundmng_pcmstop (UINT num)
  PCM stop
    Input: num PCM number



// ---- mouse

BYTE mousemng_getstat (SINT 16 * x, SINT 16 * y, int clear)
  Get status of mouse
    Input: clear Reset the counter after obtaining the state with non-0
    Output: * x direction count from x clear
            * Y Count in y direction from clear
    Return: bit 7 Left button status (0: pressed)
            Bit 5 State of right button (0: pressed)



// ---- serial / parallel / midi

COMMNG commng_create (UINT device)
  Serial Open
    Input: device number
    Return: Handle (NULL on failure)


Void commng_destroy (COMMNG hdl)
  Serial close
    Input: Handle (NULL on failure)



/ / - - joy stick

BYTE joymng_getstat (void)
  Get status of joystick

    Return: bit 0 status of up button (0: pressed)
            State of the bit 1 down button
            Bit 2 Left button state
            Bit 3 State of the right button
            State of bit 4 fire button 1
            State of bit 5 fire button 2
            The state of bit 6 button 1
            The state of bit 7 button 2


// ----

Void sysmng_update (UINT bitmap)
  Called when the state changes.

Void sysmng_cpureset (void)
  Called on reset



Void taskmng_exit (void)
  Terminate the system.
