**wolfdosmpu**
==============

Copyright © 2021 by ericvids.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License v2 ONLY, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License v2 along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


About
=====

**wolfdosmpu** is a source mod for the original 16-bit DOS versions of Wolfenstein 3-D and Spear of Destiny to add rudimentary MIDI playback support via the MPU-401 interface, replacing the OPL2-based music.

It is recommended to use this mod with [wolfmidi](https://github.com/ericvids/wolfmidi/), which converts the game's native OPL2 music data to General MIDI.

Why use this over an enhanced port like [ECWolf](http://maniacsvault.net/ecwolf/) (which already supports MIDI)? Well, you _can_ use enhanced ports with wolfmidi (I use ECWolf regularly myself), but wolfdosmpu allows you to play on the original system hardware that Wolf3D is designed for, with _absolutely no source port quirks_ for that authentic retro 90's experience. Except it's now enhanced with MPU-401-compatible GM goodness! (Also try it on an MT-32 with Roland's GM patch!)

Also, this mod is forked directly from the official Wolf3d source release from id Software, with only the barest minimum of changes needed to get things working. I did this as a personal exercise in DOS 80x86 assembly and 16-bit C programming. (Segmented memory is such a FAR-out concept, man!)

THIS MOD IS CURRENTLY IN BETA. It has been tested in emulation (via DOSBox), but since I currently don't have access to a real DOS machine from the era (with a 286 or 386 processor), I cannot yet guarantee that it works on real hardware. So please do test it and see if it works!


Version History
===============

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

2. Currently, Apogee registered versions are not supported. If your data files are from that version, you will need to patch it. The [ECWolf downloads section](https://maniacsvault.net/ecwolf/download.php) provides such a patch.

3. If your hardware MPU-401 is not being detected, try to set the port via the P option of the BLASTER environment variable. Try this even if you don't have a Sound Blaster card. (Some Roland MPU-401s and clones may be configured to use a different address instead of the default 330, e.g., 300, 332, etc.)

```
         SET BLASTER= ... P330 ...

example: SET BLASTER=A220 I7 D1 T6 P330 H5
```

Bring "M" on!
-------------

If you want to replace any track of the game with your own MIDI music, simply replace the corresponding entry in the MUSIC\ directory with a MIDI file of your choice. (Do NOT use the .MID extension. Also, do NOT touch the _INFO file unless you know what you are doing!)

However, since the MIDI files have to be optimized specifically for Wolf3D's timing algorithm, you will likely need to edit any custom MIDI files. The current limitations are:

- Only Type-0 (single-track) standard MIDI files are supported.
- All music is locked to one speed: 350 ticks per quarter note at 120 BPM. This is because the original Wolf3D MUSE engine is locked to 700Hz. If your MIDI file does not conform to this format, you will need to edit it in a sequencer program to change the tick rate and tempo, and speed-up/slow-down the notes as necessary. Tempo change messages are NOT recognized.
- System-exclusive (F0, F7) and meta-event (FF) messages are currently ignored. Please remove them if possible (to save space/processing time and to avoid variable-length data issues, described below).
- Extra-long pauses, or anything necessitating more than two bytes of MIDI variable-length data (i.e., 16384+ ticks, or roughly 23+ seconds) are not supported. The music will simply stop if the engine encounters this.

I am H@xx0r Incarnate!
----------------------

If you want to build the .EXE files yourself (or for some reason you love SoD's copy protection screen and its "Hitler Waltz" BGM), you will need to follow [Fabien Sanglard's guide](https://fabiensanglard.net/Compile_Like_Its_1992/) on how to build the Wolf3D source code on DOSBox using Borland C++ 3.1.

The version of the Wolf3D source code in wolfdosmpu has been streamlined -- it does not include unnecessary files from id Software's original source release, and it also fixes some issues with building the shareware version and both SoD commercial and demo releases (i.e., you will not need any of Fabien's additional downloads except for the Borland compiler itself -- everything else is already in here).

In addition, helper batch files are provided to quickly switch between versions. You can execute them in DOSBox before launching the Borland C++ environment.

```
_WC.BAT -- Wolf3D commercial 1.4 (Activision, GT, and Steam releases)
_WS.BAT -- Wolf3D shareware 1.4 (Apogee release)
_SC.BAT -- SoD commercial 1.0/1.4 (FormGen and Steam releases)
_SD.BAT -- SoD demo 1.0
```
