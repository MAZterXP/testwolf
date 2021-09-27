**wolfdosmpu**
==============

Copyright © 2021 by ericvids.

This program is free software; you can redistribute it and/or modify it under the terms of either the original source code license agreement as published by id Software (LICENSE1) or the GNU General Public License v2 ONLY, as published by the Free Software Foundation (LICENSE2).

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the LICENSE files for more details.


About
=====

**wolfdosmpu** is a source mod for the original 16-bit DOS versions of Wolfenstein 3-D and Spear of Destiny to add rudimentary MIDI playback support via the MPU-401 interface, replacing the OPL2-based music.

It is recommended to use this mod with [wolfmidi](https://github.com/ericvids/wolfmidi/), which converts the game's native OPL2 music data to General MIDI.

Why use this over an enhanced port like [ECWolf](http://maniacsvault.net/ecwolf/) (which already supports MIDI)?

Well, you _can_ use the music files generated by wolfmidi with enhanced ports that already support MIDI (I use ECWolf regularly myself)...

... BUT wolfdosmpu allows you to play on the original system hardware that Wolf3D is designed for, with _absolutely no source port quirks_ for that authentic retro 90's experience. Except it's now enhanced with MPU-401-compatible GM goodness! (Also try it on an MT-32 with Roland's GM patch!)

Also, this mod is forked directly from the official Wolf3d source release from id Software, with only the barest minimum of changes needed to get things working. I did this as a personal exercise in DOS 80x86 assembly and 16-bit C programming. (Segmented memory is such a FAR-out concept, man!)

THIS MOD IS CURRENTLY IN BETA. It has been tested in emulation (via DOSBox), but since I currently don't have access to a real DOS machine from the era (with a 286 or 386 processor), I cannot yet guarantee that it works on real hardware. So please do test it and see if it works!


Version History
===============

1.22 BETA (2021-09-27)
----------------------
- Fixed Read This support in WOLF3DRM.EXE and WOLF3DSM.EXE so that they don't shrink the view window every time (and cause a crash once in a while). This is due to the help screens taking up a lot of memory, which Wolf3D compensates for by releasing some memory used by the view window.

1.21 BETA (2021-09-26)
----------------------
- Partial compatibility with Sound Blaster AWEUTIL (MPU-401 port emulation) in DOS. The game must be started with the NOXMS parameter to prevent crashing. (Thanks to Gmlb256 for testing!)

1.20 BETA (2021-09-25)
----------------------
- Rewrote MPU code to fully reuse the existing OPL sequencer state variables for MPU playback. This should make this mod 100% savegame- and data-segment-compatible with the latest commercial versions of Wolf3D and SoD.
- Currently testing the removal of the clear interrupt flag instruction in the inner assembly loop. Maybe that will fix issues for some hardware. Maybe not. Maybe it's worse. Please test.
- Altered .PRJ file to strip debugging symbols for better LZEXE compression. (decided after discussion with Akuma and Gmlb256)
- Replaced SIGNON.OBJ and GAMEPAL.OBJ with ones from the [Wolf3D game source recreation project](https://bitbucket.org/gamesrc-ver-recreation/wolf3d/). For the Wolf3D screen, I personally dislike the weird colors of the Activision logo, so I opted for the id logo for neutrality. :)
- Moved all the license files to the README section. I believe the general consensus is that Wolf3D's source code came under dual licenses -- limited non-commercial license and GPL). Like many other mods, I defer the option to the user.

1.10 BETA (2021-09-24)
----------------------
- Added support for Apogee registered version 1.4/1.4g by using techniques to reduce data segment usage (e.g., by allocating strings as individual characters in the code segment).
- Corrected a very rare edge case where midiTick may get executed before the length of the MIDI file has been calculated, and the song counter could potentially go out-of-bounds.
- Limited the MPU port selection in BLASTER environment variable to the range 2xx-3xx hex because poking some ports by accident can potentially kill your system!
- Changed sound options menu to display "MPU-401 (General MIDI)" instead of "AdLib/Sound Blaster". (suggested by Gmlb256)
- Added Fabrice Bellard's [LZEXE](https://bellard.org/lzexe.html) compressor in the build workflow. This is the original compressor used by id to fit the shareware game on a single floppy disk. (suggested by Gmlb256)
- Revamped build workflow to allow building the original OPL2 versions (essentially "vanilla" Wolf3D) by specifying any argument in the version selection batch files.

1.00 BETA (2021-09-22)
----------------------
- Initial release.


Usage
=====

Can I play NOW, daddy?
----------------------

Download a pre-built .EXE file for your game from the [Releases section](https://github.com/ericvids/wolfdosmpu/releases/). Drop this .EXE into your game's installation directory, e.g., C:\WOLF3D.

Also download the latest [wolfmidi.zip](https://github.com/ericvids/wolfmidi/releases/). You will need to unzip wolfmidi.zip's contents inside your game's installation directory, e.g., C:\WOLF3D. _You need to preserve the directory structure when unzipping! (Refer to your preferred zip utility's documentation for details.)_ Afterwards, your game directory should look like this:

```
  WOLF3D\
  | MUSIC\
  | | COPYPRO
  | | CORNER
  | | .
  | | .
  | | _INFO
  | AUDIOHED.WL6
  | AUDIOT.WL6
  | .
  | .
  | VSWAP.WL6
  | WOLF3D.EXE (original version)
  | WOLF3DCM.EXE (wolfdosmpu version)
```

Then simply launch the wolfdosmpu executable for your game (e.g., WOLF3DCM.EXE for Wolf3D commercial version).

In case you're wondering, the MUSIC\ directory contains all the tracks for _both_ Wolf3D and SoD games. Any tracks with the same name between each game happen to be exact byte-for-byte duplicates. If you want to save space, you may install the data files of both Wolf3D and SoD in the same directory, so they can share the same MUSIC\ directory.

Don't hurt MIDI.
----------------

1. If you use wolfdosmpu inside DOSBox, select your MIDI device with the help of [this guide](https://www.dosbox.com/wiki/Configuration:MIDI).

2. If your hardware MPU-401 is not being detected, try to set the port via the P option of the BLASTER environment variable. Try this even if you don't have a Sound Blaster card. (Some Roland MPU-401s and clones may be configured to use a different address instead of the default 330, e.g., 300, 332, etc.)

```
         SET BLASTER= ... P330 ...

example: SET BLASTER=A220 I7 D1 T6 P330 H5
```
3. __Note on memory and TSRs__: wolfdosmpu generally requires an additional 96K more RAM (estimated) than the original Wolf3D, needed for MIDI file caching and parsing. Also, TSRs such as AWEUTIL (which enables the necessary MPU-401 support on Sound Blaster AWE cards) are known to crash the game unless you run with the NOXMS command line parameter (e.g., "WOLF3DCM.EXE NOXMS"). In such a case, it might help to load any other required TSRs in the upper memory area (using the LOADHIGH command) and to load Microsoft's EMM386.

4. Apogee version 1.4/1.4g's .WL6 data files are _NOT_ compatible with WOLF3DCM.EXE; use WOLF3DRM.EXE instead. However, the data files are identical between both Apogee 1.4 and 1.4g. The main reason for keeping the Apogee version is for its "Read This" feature, which is missing in the commercial versions and is potentially useful in other mods. (Since version 1.20, I have opted to replace the publisher logo in the sign-on screen with the id logo, for all Wolf3D builds.)

5. __Note on savegames__: Savegames are only compatible between the final commercial (i.e., Activision) version of each game (Wolf3D or SoD) and its wolfdosmpu-enhanced counterpart. This is simply because the open-source Wolf3D codebase was last used to build these two commercial versions. Note that savegames are also incompatible between these versions and their Apogee, GT and FormGen predecessor versions. Simply put, id didn't really care about savegame compatibility across versions; most of their classic games have been like this. Supporting savegame compatibility for the demo SoD and shareware/registered Wolf3D builds of wolfdosmpu will require extensive backporting of community changes, e.g., from the [Wolf3D game source recreation project](https://bitbucket.org/gamesrc-ver-recreation/wolf3d/), and currently I feel that it's not worth the hassle. Interestingly enough, savegames from WOLF3DRM.EXE actually work in WOLF3DCM.EXE (and vice versa) because the arrangement of the savegame variables are similar between the two, so there's that...

Bring "M" on!
-------------

If you want to replace any track of the game with your own MIDI music, simply replace the corresponding entry in the MUSIC\ directory with a MIDI file of your choice. (Do NOT use the .MID extension. Also, do NOT touch the _INFO file unless you know what you are doing!)

However, since the MIDI files have to be optimized specifically for Wolf3D's timing algorithm, you will likely need to edit any custom MIDI files. The current limitations are:

- Only Type-0 (single-track) standard MIDI files up to 65535 bytes long are supported.
- Only one tick rate and BPM is supported: 350 ticks per quarter note (TPQN) at 120 beats per minute (BPM) -- essentially 700 ticks per second. This is because the original Wolf3D MUSE engine is locked to 700Hz. If your MIDI file does not have "350 TPQN" in its header, it will NOT play. You will need to edit it in a sequencer program to change the tick rate, and speed-up/slow-down the notes as necessary by decreasing/increasing note spacings instead of sending BPM change commands. (Tempo change is also NOT supported.) An alternative way to convert your MIDI files is to use MIDI recorder software: preset the recording to 350 TPQN and 120 BPM, and simply record the playback of an existing MIDI file.
- System-exclusive (F0, F7) and meta-event (FF) messages are currently ignored. Please remove them if possible (to save space/processing time and to avoid variable-length data issues, described below).
- Extra-long pauses, or anything necessitating more than two bytes of MIDI variable-length data (i.e., 16384+ ticks, or roughly 23+ seconds of pause between any note change) are not supported. The music will simply stop if the engine encounters this.

I am H@xx0r Incarnate!
----------------------

If you want to build the .EXE files yourself (or for some reason you love SoD's copy protection screen and its "Hitler Waltz" BGM), you will need to follow [Fabien Sanglard's guide](https://fabiensanglard.net/Compile_Like_Its_1992/) on how to build the Wolf3D source code on DOSBox using Borland C++ 3.1.

The version of the Wolf3D source code in wolfdosmpu has been streamlined -- it does not include unnecessary files from id Software's original source release, and it also fixes some issues with building versions other than Wolf3D commercial release 1.4 (i.e., you will not need any of Fabien's additional downloads except for the Borland compiler itself -- everything else is already in here).

In addition, helper batch files are provided to quickly switch between versions. You should execute the batch file of your preferred version inside DOSBox before launching the Borland C++ environment (BC.EXE). Then simply press F9 to build.

```
Supported versions: (savegame compatibility is preserved between original .EXE and wolfdosmpu-enabled .EXE)
_WC.BAT -- Wolf3D commercial v1.4 (id, GT and Activision/Steam releases)
_SC.BAT -- SoD commercial v1.0/v1.4 (FormGen and Activision/Steam releases)

Semi-supported versions: (savegame compatibility is NOT preserved between original .EXE and wolfdosmpu-enabled .EXE)
_WR.BAT -- Wolf3D registered v1.4/v1.4g (Apogee releases)
_WS.BAT -- Wolf3D shareware v1.4/v1.4g (Apogee releases)
_SD.BAT -- SoD demo 1.0 (FormGen release)
```

As of version 1.10, LZEXE has been integrated into the build system. Whenever you switch between versions, an LZEXE-compressed .EXE file is generated from the last built WOLF3D.EXE in the OBJ\ subdirectory, and renamed as appropriate (e.g., WOLF3DCM.EXE, SPEARCM.EXE, etc.). If you want LZEXE compression on your current build but don't want to switch between game versions, you can run VERSION.BAT to trigger the compression routine on its own.

Also as of version 1.10, you can build the "vanilla" Wolf3D/SoD games without MPU support by passing (any) argument to each batch file. (Note that these "vanilla" versions are savegame-compatible with their corresponding MPU-supporting version.) The output of the batch file will indicate "MPU enabled" (wolfdosmpu version) or "MPU disabled" (vanilla version). You can also build ALL versions at once by executing the _ALL.BAT file. Warning: This builds 10 .EXE files (5 versions, each with vanilla and wolfdosmpu variants), and it takes a while.
