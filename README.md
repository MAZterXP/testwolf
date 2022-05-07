**wolfdosmpu**
==============

Copyright © 2021 by ericvids.

This program is free and open source software; you can redistribute it and/or modify it under the terms of either the original source code license agreement as published by id Software (LICENSE1) or the GNU General Public License v2 ONLY, as published by the Free Software Foundation (LICENSE2).

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the LICENSE files for more details.


About
=====

**wolfdosmpu** started as a source mod for the original 16-bit DOS versions of Wolfenstein 3-D and Spear of Destiny to add rudimentary MIDI playback support via the MPU-401 interface, replacing the OPL2-based music. Since then, many features have been added (gameplay bug fixes, compatibility options, additional support for sound hardware, modern WASD-style controls, and an automap function), while keeping the core Wolf3D game and interface unchanged unless absolutely necessary. Thus, the project has essentially grown into a proverbial "lost version update" of vanilla Wolf3D/SoD, catering to people who still prefer playing these games on DOS. Personally, it has become my favorite way to play these games even on modern systems (via DOSBox). Try it, you might like it yourself. :)

It is recommended to use this mod with [wolfmidi](https://github.com/ericvids/wolfmidi/), which converts the game's native OPL2 music data to General MIDI.

This mod is forked directly from the official Wolf3D source release from id Software. I did this as a personal exercise in DOS 80x86 assembly and 16-bit C programming. (Segmented memory is such a FAR-out concept, man!)


Installation
============

Can I play NOW, daddy?
----------------------

Download a pre-built EXE file for your game from the [Releases section](https://github.com/ericvids/wolfdosmpu/releases/). Drop this EXE into your game's installation directory, e.g., C:\WOLF3D.

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
  | WOLF3D.EXE (vanilla version)
  | WOLF3DCM.EXE (wolfdosmpu version)
```

Then simply launch the wolfdosmpu EXE for your game (e.g., WOLF3DCM.EXE for Wolf3D commercial version).

If you don't get MPU-401 music, check the Sound menu. If your MUSIC\ directory is being read by the game, the "AdLib/Sound Blaster" music option will change to "MPU-401/General MIDI"; otherwise, the game will revert to playing OPL2 music. If you are not hearing any music at all and the "MPU-401/General MIDI" option is shown, something else is wrong -- see the [tips section](#dont-hurt-midi) for troubleshooting.

In case you're wondering, the MUSIC\ directory contains all the tracks for _both_ Wolf3D and SoD games. Any tracks with the same name between each game happen to be exact byte-for-byte duplicates. If you want to save space, you may install the data files of both Wolf3D and SoD in the same directory, so they can share the same MUSIC\ directory.

Don't hurt MIDI.
----------------

Here are some tips and suggestions for some common problems:

1. If you use wolfdosmpu inside DOSBox, wolfdosmpu will sometimes play on a different MIDI device than you intended (or will not play anything at all). In this case, select your MIDI device with the help of [this guide](https://www.dosbox.com/wiki/Configuration:MIDI).

2. If your hardware MPU-401 is not being detected, try to set the port via the P option of the BLASTER environment variable.
   ```
            SET BLASTER= ... P330 ...

   example: SET BLASTER=A220 I7 D1 T6 P330 H5
   ```
   Try this even if you don't have a Sound Blaster card (in which case, BLASTER would contain only the P option). Some Roland MPU-401s and clones may be configured to use a different address instead of the default 330, e.g., 300, 332, etc.

3. If you have a DMA-less Sound-Blaster-compatible card (such as an ES1688-based card), you have to specify the address of your Sound-Blaster-compatible in your BLASTER environment variable but _do not_ specify the IRQ (I) and DMA (D) parameters. For example: "SET BLASTER=A220" or "SET BLASTER=A220 P330" (without the quotes). This enables DMA-less mode, which uses your CPU to push the audio samples to your device with the proper timing.

4. If you have a parallel port digitized sound device that is similar to but not fully compatible with the Disney Sound Source (e.g., Covox Speech Thing), you can enable Covox compatibility mode by using the "SS#" command-line parameter, where # is the LPT port number. For example: "WOLF3DCM SS2" (without the quotes) to use a Covox on LPT2. The Sound menu will confirm that this mode is enabled by renaming the "Disney Sound Source" option to "Covox/Sound Source"; make sure that this option is selected. Covox compatibility mode, like DMA-less mode (above), relies on your CPU to push the audio samples to your device with the proper timing. (This mode also works for the Disney Sound Source itself; the game simply ignores the device's built-in sample buffering.)

5. __Be careful when using Apogee shareware-/registered-version data files!__ Make sure your Apogee data files are version 1.4 or 1.4g. All earlier Apogee versions (1.2 and below) are not supported.

   As a countermeasure, wolfdosmpu (as of version 1.42) tries to detect whether your data files are of the correct version, by checking the extension (WL6/WL1/SOD/SDM) and whether the VGAHEAD.WL6 file reports the correct size. If you get the error "NO .WL6/.WL1 FILES TO BE FOUND!", you are probably using shareware version data files on the registered/commercial version EXEs (or vice versa). If you get the error "VGAHEAD.WL6 IS INCOMPATIBLE!", you are probably using registered version data files on a commercial version EXE (or vice versa). Switch to the correct version EXE and try again.

   The main reason for keeping the Apogee versions is for their "Read This" feature, which is missing in the commercial versions and is potentially useful in other mods. (Since version 1.20, I have opted to replace the publisher logo in the sign-on screen with the id logo, for all Wolf3D builds.)

6. __Note on memory and TSRs__: wolfdosmpu generally requires more conventional RAM than the vanilla Wolf3D engine. The additional memory is used for MIDI file caching and parsing, and has to be allocated in conventional memory for compatibility reasons. The amount needed will increase if you use longer custom songs. The modern-controls EXEs also consume more memory for program code supporting the automap and other features.

   TSRs such as AWEUTIL (which enables the necessary MPU-401 support on Sound Blaster AWE cards) consume part of your conventional memory. As with other old DOS games, try to load as few TSRs as possible; for necessary TSRs such as mouse drivers and AWEUTIL, load them into the upper memory area by using LOADHIGH. You can check your available conventional memory using the MEM command.

   My own testing with DOSBox-X indicates that the modern-controls EXEs require at least 576 KB of free conventional memory in order to run properly (with the default music). The MPU-only EXEs use about 8 KB less, but I still recommend freeing at least 576 KB of conventional memory for the best results.

   One tester, Gmlb256, suggests to have about 578 to 590 KB of free conventional memory when using AWEUTIL (that is, about 615 to 620 KB _before_ loading AWEUTIL; the additional usage will depend on the soundfont you use). Gmlb256 also suggests using real-mode UMB drivers such as UMBPCI if you don't want to use an EMM such as EMM386 or QEMM.

   If you use the "nomain" command-line parameter, you can force the wolfdosmpu EXE to load regardless of available main memory. THIS IS AN EXPERIMENTAL FEATURE, and you risk all sorts of unexpected behavior, such as trigerring the mythical id Software copy protection code that erases your hard drive if you don't have a registered copy of v1.1. Don't say I didn't warn you. (Seriously, however, expect the game to exhibit weird problems like showing incorrect wall graphics and crashing when starting or loading a game.)

7. __Note on savegames__: Beginning with version 1.48, wolfdosmpu is now backwards-compatible with savegames from the original WOLF3D.EXE (v1.4 only) and SPEAR.EXE! See the FAQ below for details.

Bring "M" on!
-------------

If you want to use your own MIDI music, simply replace each music file that you want to change in the MUSIC\ directory with a MIDI file of your choice. (Do NOT use the MID extension. Also, do NOT touch the _INFO file unless you know what you are doing!)

However, since the MIDI files have to be optimized specifically for Wolf3D's timing algorithm, you will likely need to edit any custom MIDI files. The current limitations are:

- Only Type-0 (single-track) standard MIDI files up to 65535 bytes long are supported.
- Only one tick rate and BPM is supported: 350 ticks per quarter note (TPQN) at 120 beats per minute (BPM) -- essentially 700 ticks per second. This is because the vanilla Wolf3D MUSE engine is locked to 700Hz. If your MIDI file does not have "350 TPQN" in its header, it will NOT play. You will need to edit it in a sequencer program to change the tick rate, and speed-up/slow-down the notes as necessary by decreasing/increasing note spacings instead of sending BPM change commands. (Tempo change is also NOT supported.) An alternative way to convert your MIDI files is to use MIDI recorder software: preset the recording to 350 TPQN and 120 BPM, and simply record the playback of an existing MIDI file.
- System-exclusive (F0, F7) and meta-event (FF) messages are currently ignored. Please remove them if possible (to save space/processing time and to avoid variable-length data issues, described below).
- Extra-long pauses, or anything necessitating more than two bytes of MIDI variable-length data (i.e., 16384+ ticks, or roughly 23+ seconds of pause between any note change) are not supported. The music will simply stop if the engine encounters this.

I am H@xx0r Incarnate!
----------------------

If you want to build the EXE files yourself (or for some reason you love SoD's copy protection screen and its "Hitler Waltz" BGM), you will need to follow [Fabien Sanglard's guide](https://fabiensanglard.net/Compile_Like_Its_1992/) on how to build the Wolf3D source code on DOSBox using Borland C++ 3.1.

The version of the Wolf3D source code in wolfdosmpu has been streamlined -- it does not include unnecessary files from id Software's original source release, and it also fixes some issues with building versions other than Wolf3D commercial release 1.4 (i.e., you will not need any of Fabien's additional downloads except for the Borland compiler itself -- everything else is already in here).

In addition, helper batch files are provided to quickly switch between versions. You should execute the batch file of your preferred version inside DOSBox before launching the Borland C++ environment (BC.EXE). Then simply press F9 to build.

```
Supported versions:
_WC.BAT -- Wolf3D commercial v1.4 (id, GT and Activision/Steam releases)
_WR.BAT -- Wolf3D registered v1.4/v1.4g (Apogee releases)
_SC.BAT -- SoD commercial v1.0/v1.4 (FormGen and Activision/Steam releases)

Semi-supported versions:
_WS.BAT -- Wolf3D shareware v1.4/v1.4g (Apogee releases)
_SD.BAT -- SoD demo v1.0 (FormGen release)
```

As of version 1.10, LZEXE has been integrated into the build system. Whenever you switch between versions, an LZEXE-compressed EXE file is generated from the last built WOLF3D.EXE in the OBJ\ subdirectory, and renamed as appropriate (e.g., WOLF3DCM.EXE, SPEARCM.EXE, etc.). If you want LZEXE compression on your current build but don't want to switch between game versions, you can run VERSION.BAT to trigger the compression routine on its own.

Also as of version 1.30, you can build the wolfdosmpu EXEs without modern control support by passing the "M" (MPU-only-version) argument to each batch file, or a completely clean version (equivalent to the original id source release, with only minimal modifications to compile SoD and the Apogee versions) by passing the "V" (vanilla) argument. You can build all 5 game versions (Wolf3D commercial, Wolf3D registered, Wolf3D shareware, SoD commercial, and SoD demo) of each variant at once by executing the _ALL.BAT file with the "V", "M", or "W" (WASD-version) argument, or with no arguments to build all 15 EXEs.


FAQ
===

General
-------

- ### Why use wolfdosmpu over an enhanced port like [ECWolf](http://maniacsvault.net/ecwolf/) (which already supports MIDI)?

  Well, you _can_ use the music files generated by wolfmidi with enhanced ports that already support MIDI (I used to play ECWolf regularly myself)...

  ... BUT wolfdosmpu gives you a way to play Wolf3D with enhanced music on the original system hardware that Wolf3D is designed for, while retaining the original look-and-feel of the game, and optionally with _no gameplay changes introduced by modern source ports_, for that authentic retro 90's experience.

- ### "Optionally with no gameplay changes"?

  Because you also have the option to play using modern first-person-shooter controls, right on your DOS system! (WASD keys for movement and mouse for turning, as well as the Tab key to check the map ala Doom or your collection progress ala Quake.) For those who consider this "cheating" or otherwise an inauthentic experience, you have two alternatives: (1) you can turn off all enhancements in the menus, or (2) you can download a version of the EXE with just the MPU-401 support and essential bugfixes, controllable via command-line.

  Also, some gameplay changes are the result of fixing bugs in the vanilla game, but if you feel that these bugs should stay, you can use the COMP command-line parameter to re-enable them! See the [Usage FAQ](#usage) for details.

- ### What if I want the automap feature but prefer the classic controls?

  Don't worry, the modern-controls EXEs _can_ be used with classic controls. Just disable the options "Mouse Enabled -> Turn Control Only", "Always Run", and "Keys Always Strafe", then go to Customize Controls and set your usual keys. Don't forget to set the "Tab Key Function" to Automap!

- ### I don't have an MPU-401 or a compatible MIDI module, can I still use this?

  If you have a Sound Blaster AWE card, you can use your card's supplied AWEUTIL TSR program to provide MPU-401 emulation.

  If you have a MIDI module running off a serial port interface or a non-MPU-401 MIDI interface on an older Sound Blaster or compatible card, you can use [SoftMPU](https://bjt42.github.io/softmpu/).

  If you have a relatively modern Windows machine (XP and above), you can use DOSBox to provide full MPU-401 emulation, along with any MIDI module that you can plug into your machine via a USB-MIDI cable, or even use softsynths such as [Coolsoft VirtualMIDISynth](https://coolsoft.altervista.org/en/virtualmidisynth) or [Yamaha S-YXG50](https://veg.by/en/projects/syxg50/). More information is available in the [wolfmidi README](https://github.com/ericvids/wolfmidi/#bring-m-on). (In fact, I developed wolfdosmpu primarily on DOSBox, particularly the [DOSBox-X](https://dosbox-x.com/) fork. The kind people at the [vogons forum](https://vogons.org) were gracious enough to test the EXEs on real DOS machines.)

  If you don't have any of these, at the very least, wolfdosmpu works like a bug-fixed version of Wolf3D if you don't supply the music files.

Usage
-----

- ### Why is the modern-controls version significantly slower than the MPU-only version on my real 286/386?

  The automap requires the game to retain information about areas the player has already seen in real-time. Thankfully, an internal table called "spotvis" already tracks this, with the first bit of each byte (tile) being set for a tile that has been seen, but since Wolf3D only needs it to select which sprites to render on a frame, the table is completely erased between frames (using a very fast assembly-language routine). wolfdosmpu instead opts to retain the table, and replicates the previous visibility information over to the second bit of each tile. But it still has to "forget" the currently-seen tiles, which means using a more expensive per-byte bitmask operation between each frame.

  Moreover, in order to save on conventional memory, the spotvis table is also reused to store automap-related state variables such as whether a door has been opened or a secret wall has been pushed. Also, Wolf3D's spotvis normally only handles blank tiles, but since the automap also needs to render the walls surrounding the blank tiles, wall hits also have to be stored in the spotvis, which effectively adds one more write operation per ray cast.

  All these little changes cause an estimated 10 to 33 percent decrease in frame rate. There might be slightly more efficient methods, but there will always be a memory tradeoff (and we are also constrained by the conventional memory limit). Since I'm still discovering more bugs in the classic code that need fixing, that takes priority over optimization for now.

- ### What does the ^ symbol in the completion stats/automap mean?

  The ^ indicates that there are more kills/secrets/treasures on the level but they are currently inaccessible. When you get a key or push a secret door that makes more items accessible, the counts increase, and eventually the ^ disappears when everything is accessible. The ^ will not disappear in E2M8, E3M10 and E6M10 by the way, because of map bugs and/or ghosts that can not be killed.

- ### Why are keys marked as treasure in the automap? And why is the silver key marked as gold?

  The color gold/brown is used in the automap to denote "map completion" bonuses. This includes all treasure, keys, and the spear. As of version 1.48, the keys and spear will glow on the automap whether or not they are currently visible in first-person view, making them easier to find.

  All other bonuses are colored blue, meaning they are not essential for 100% completion. If the silver key was colored blue or gray (the two closest colors to silver in the 16-color automap palette), it would become too hard to distinguish it from the rest of the map; it's generally easier to just "look for everything that's gold and pick it up". Besides, in practice, the actual color of the key does not matter in Wolf3D since the locked doors don't actually indicate their key color, unlike in Doom.

- ### Why are pools of blood and gibs marked as bonuses in the automap?

  Because they ARE bonuses! They boost your health by 1% if your health is 10% or below. The official hint book actually tells you this. ;)

- ### I've cleared the level but the automap hints still don't show!

  If there is an existing enemy on the map that is physically or visually accessible from your location (i.e., an enemy that is not behind a secret wall, but may be reached through unlocked doors or shot through pillars or barrels), pushwall hints will not show. Kill them all! (Yeah, even the dogs. Sadness.)

  Keep an eye out for areas where the automap visibility has been cut off, and for doors that you have not actually seen opened (they will glow on the automap).

  Since version 1.43, stationary enemies that you have already seen (but may have forgotten to snipe) are now marked in automap Hint mode. You still have to kill them first to show the pushwall hints.

  If you're REALLY stuck, temporarily switch the automap mode to "Full map" to find out where the enemies are. Remember to switch back if you don't want spoilers for the next level!

  "Show hints if clear" will simply not work on E2M9, E4M9, E6M9, and SoD Map 21, because the boss is part of the enemy count and the game ends when you kill it. It is also impossible to get 100% kills in SoD Map 21 because of the regenerating spectres. Switch to "Always show hints" or "Full map" to reveal the pushwalls in these levels. (Interestingly, E3M9 is an exception because the boss is behind a locked door, and you can kill all enemies before the locked door without getting the key to reveal the map's lone pushwall.)

- ### How do the command-line options work in wolfdosmpu?

  Here is a list of Wolf3D command-line options that work somewhat differently in wolfdosmpu:
  ```
  comp # - enables compatibility mode (# = compatibility flags)
  nocomp - disables compatibility mode
  noal   - disables OPL2 support, but will NOT disable Sound Blaster digitized sound
           (unlike vanilla Wolf3D)
  noss   - aside from disabling Disney Sound Source support, this will also enable the menu option
           for PC Speaker digitized sound! (for DOSBox users, the speaker will be very loud --
           enter "mixer spkr 40" on the command line to fix this)
  ss1    - enables Covox Speech Thing support on LPT1
  ss2    - enables Covox Speech Thing support on LPT2
  ss3    - enables Covox Speech Thing support on LPT3
  nomain - disables main memory size check; use with caution!
  nompu  - disables MPU-401 support (i.e., re-enables OPL2 music)
  ```

- ### What is this COMP parameter thingamajig?

  You can enable specific quirks/bugs of the vanilla game using the COMP (compatibility) command-line parameter. These parameters may be useful when you play custom maps that depend on these quirks. Specify a number after COMP, which should be the sum of the flags you want to enable (alternatively, specify a negative number to disable those flags and enable everything else):
  ```
    1: pushwalls move 3 tiles unless blocked
       (note: the default maps assume that pushwalls move 2 tiles maximum; this option renders these
       maps impossible to complete 100%)

    2: make fake-Hitler fireball logic framerate-dependent, slowing them down considerably when
       running at 70 frames per second (this bug went undetected because the game likely ran at just
       17.5 to 35 fps on development computers)

    4: pick up items using the vanilla viewpoint-based logic, which sometimes fails to detect items
       when walking backwards or sideways, or when running fast through a row of items

    8: disable circle-strafing (turning and strafing simultaneously) when playing with modern
       controls, which is technically impossible on the vanilla Wolf3D input handling code
       (note: the demo file format is not updated to handle circle-strafing, so it is always disabled
       when playing/recording demos)

   16: limit sprite rendering to a maximum of 50 visible sprites at any time; beyond this limit, some
       active enemies may turn invisible due to the renderer prioritizing statics and enemies with
       lower indices (even if they are dead)

   32: pushwalls reactivate when they land on a space where another pushwall was placed before
       (note: the vanilla game does not immediately manifest this bug, requiring you to save and
       reload before you can push the wall again; this flag reactivates the pushwall immediately for
       your convenience)
       (note 2: if loading an older savegame, or a savegame with this COMP flag on, a pushwall that
       already occupies another pushwall's spot will always reactivate regardless of the current COMP
       setting; however, later pushwalls that end up occupying other pushwalls' spots will be tracked
       and will not reactivate as long as the COMP flag is off)

   64: doors with patrolling enemies facing them at level start become "phantom" doors, which will
       let actors pass through as if they were open; the door is reset to a regular door when opened
       (note: wolfdosmpu fixes this bug by opening all doors faced by patrolling enemies on level
       start; see E4M6 on hard mode for an example, in the top left room with the crowns)

  128: enable quirks related to door-blocking, such as enemies being able to close doors blocked by
       corpses by walking on the tile occupied by the corpse (without necessarily passing through the
       door), and unblocked doors remaining open if they were previously blocked for 7.8 minutes
       (note: enabling these quirks will cause the automap to mark blocked doors as closed instead of
       open unless the blocked door is visible from the player's first-person viewpoint)

  256: disable sound enhancements, re-enabling the following sound quirks:
       - the distance attenuation range of audible sound is set to 8 tiles (instead of 15)
       - sound effects are quickly dropped when higher-priority sounds override them (e.g., gunshots
         and door sounds will get completely muted if an enemy reacts at the exact same time)
       - doors that are re-opened while they are currently closing will not play the open-door sound
       - on SB pro, the pushwall sound is non-positioned (i.e., it will play at full volume even
         while you walk away from it)
       - on PC Speaker digitized sound, doors will also use digitized sound effects (which will sound
         garbled due to the speaker's limitations)

  ```
  For example, to mimic the vanilla game's behavior when they were run on a Pentium (or faster) system, specify COMP 511 (e.g., WOLF3DCM COMP 511). On the other hand, COMP 510 (or COMP -1) disables the 3-tile pushwall move, which is ideal for completionists who wish to retain all other engine quirks.

  COMP by itself will enable all compatibility flags (including future ones). Use NOCOMP (or simply not specify COMP at all) to disable all compatibility flags, which is the default setting.

- ### What happened to my old wolfdosmpu savegames?!
  If you have existing wolfdosmpu savegames (from versions 1.32 through 1.47), you may notice that the Load Game menu does not show these savegames. _This is not a bug_ -- wolfdosmpu's savegame naming convention has simply changed back to Wolf3D/SoD's original convention. (It was only changed to prevent people from accidentally corrupting their old original savegames, which wolfdosmpu now supports.)
  - Please rename your existing wolfdosmpu savegames from SAVEGM_n.xxx to SAVEGAMn.xxx (e.g., SAVEGM_0.WL6 -> SAVEGAM0.WL6).
  - Any subsequent saves will convert the old wolfdosmpu savegame to the new default internal format, explained below.

- ### How does savegame compatibility work?
  Upon loading a savegame, wolfdosmpu intelligently guesses its format, and switches to this format internally for the remainder of the game session (i.e., until a new game is started or loaded). This means that:
  - You can cross-load savegames of different formats, such as:
    - Load an Apogee-format savegame in wolfdosmpu's commercial version (WOLF3DCM.EXE/WOLF3DCW.EXE).
    - Load a GT- or Activision-format savegame in wolfdosmpu's registered version (WOLF3DRM.EXE/WOLF3DRW.EXE).
    - Load a shareware/demo savegame in wolfdosmpu's registered/commercial versions -- however, you will have to rename its extension (.WL1 -> .WL6 / .SDM -> .SOD) to make the savegame visible in the menu. _Be forewarned that renaming these savegames do NOT make them compatible with the original registered/commercial EXEs, only with wolfdosmpu!_
  - Any subsequent saves in wolfdosmpu will also be compatible with your original EXE, but only _that_ EXE.
    - For example, if you load and save an Activision-format savegame, the resulting savegame will be compatible with your original Activision-version EXE, but _not_ with the Apogee-version EXE (even if you use WOLF3DRM.EXE/WOLF3DRW.EXE)!
    - If you load and save a shareware/demo savegame in a registered/commercial wolfdosmpu version, the resulting savegame will be compatible with your original shareware/demo EXE (after renaming back to .WL1/.SDM), but _not_ with original registered/commercial EXEs.
      - The only exception is if you go past level 2 in SoD -- the savegame will automatically be converted to Activision-format because the SoD demo only goes up to level 2.
  - Upon launching a new game, wolfdosmpu's default internal format is:
    - Activision format for the commercial versions (CM/CW).
    - Apogee format for the shareware and registered versions (RM/RW/SM/SW).
    - FormGen format for the SoD demo version only (DM/DW).
    - Note: There is no way to create a fresh new wolfdosmpu game in GT format or commercial FormGen format. If compatibility is strictly required, start a new game from a GT- or FormGen-version EXE, save it, and load it in the wolfdosmpu EXE.
  - When using the modern-controls version, automap data is appended to the end of the savegame. This is safely ignored by the original EXEs.
  - Automap data will be lost if a wolfdosmpu savegame is loaded and saved again on your original EXE.
  - FormGen-version EXEs have a bug where only the first 8 levels' statistics are saved. To preserve compatibility with these FormGen-version EXEs, wolfdosmpu forgets the latter levels' stats whenever a FormGen-format savegame is loaded and saved, leading to incorrect stats upon finishing the game.
    - To ensure that all level stats are preserved, use an Activision-format savegame (or start a fresh new game in wolfdosmpu).


Version History
===============

1.49 (2022-05-08)
-----------------
- Now allows menu setting of the view size and mouse sensitivity to be stretched to the unofficial limits supported by the engine, both in the modern-controls and MPU-only versions. (Why both? Because players have enabled this on vanilla Wolf3D by hacking the config file -- so we might as well give them the full freedom. :))
- Special fixes to make the maximum view size (AKA "borderless mode") work better:
  - Fixed missing vertical wall lines/columns by disabling the related column-drawing optimization (but only for this view size). This eliminates no-clip-like artifacts when hugging walls at an angle, but note that this makes the game run a bit slower than vanilla, so only use the maximum view size if you don't mind (or notice) the performance penalty.
  - Now properly renders the view's bottom border. (This also entails erasing this border when the game goes into the Get Psyched, level completion and victory screens.)
  - Note that game logic determinism is slightly different on maximum view size. In fact, if you ever feel that the game plays differently, e.g., enemies take unusually more shots, it's probably due to your view size. _This is a known problem of the vanilla Wolf3D engine,_ with the view size determining your exact shooting radius. To see the problem in action, play back the shareware version's first demo on different view sizes -- the demo ends sooner than normal at the maximum view size, when BJ misses a shot on a guard and tries to run past him instead.
- Expanded COMP 256 to include other sound fixes that potentially alter gameplay from the original (such as muted door re-opening, non-positioned pushwall sound, etc.).
- Fixed some quirks when using the mouse to navigate menus (e.g., continuous triggering of options when the mouse button is held down).

1.48 (2022-05-05)
-----------------
- AKA the "Wolfenstein 3-D 30th Anniversary" release. :D
- Thanks to an obscure hack, wolfdosmpu is now backwards-compatible with savegames from the original WOLF3D.EXE (v1.4 only) and SPEAR.EXE! See the FAQ above for details.
- Now makes the positions of keys (and the spear) always glow on the automap. This can help players find the map's "quickest-path" solution without obtaining all treasure.
- Improved the distance attenuation range of sound effects (when playing on SB pro). The original attenuation lookup table was designed to minimize the volume of a sound at just 8 tiles away, which makes the sound falloff too short. This fix extends the range to 15 tiles, making sounds that are further away much more audible, while maintaining clear stereo separation. (This should be a better solution than the previous one implemented in 1.45.)
- Added COMP 256 to revert to the old sound engine behavior, disabling the extended distance attenuation range and sound queueing -- for people who still prefer the vanilla sound behavior.
- Fixed goobers/debugmode Tab+key cheats in the MPU-only EXEs.
- Removed the extra player mark that appears on the automap while BJ is doing the victory jump.
- Changed color of the ^ symbol in the stats to white (because people confuse it as a half-rendered 0).
- Removed the extra keypress just after exiting Robert's Jukebox.
- Now shows wolfdosmpu version at the sign-on screen instead of program exit.

1.47 (2021-11-27)
-----------------
- Really fixed first-sample dropping on Sound Blaster this time (I hope) -- it seems to be caused by a driver problem that throws a stream-continue interrupt too early when the interrupt is masked and subsequently unmasked. The new workaround defers playing of full samples until after the first interrupt.
- Added COMP 128 to revert the enemies-closing-unclosable-doors glitch. Also fixed more cases where the glitch can happen (e.g., when two or more enemies step on all the corpses that block the door from closing with perfect timing) and a related glitch where a door that has been blocked open for more than 32767 tics (7.8 minutes) would not close for another 7.8 minutes even if unblocked.
- Fixed so that Esc and function keys are inaccessible when the game is already in a special playstate (e.g., player dying, level completion, etc.). This fixes many succeeding bugs such as the player dying immediately after a quick load (which happens when the player was killed at the exact moment they press F9), or saving at the exact moment of death and reloading to find yourself alive but with zero health.
- Reverted stereo separation formula -- the change was too jarring for players used to the original formula. The sound queueing system is retained, however.

1.46 (2021-11-25)
-----------------
- Implemented a sounding queueing scheme where a sound effect may only override a currently-playing one when the latter has played for a minimum length, replacing the door-related special cases in the previous version. This scheme fixes more sound cut-off issues while guaranteeing that the next sound effect still plays -- all player-activated digitized sounds (active door opening/closing, pushwall activation, and gun firing) are assigned minimum lengths. No more "silenced" gunshots or door-opening due to an enemy shouting too quickly in reaction -- and the enemy shout is not dropped! (Although you will still only hear the shout in full if you stop firing... again, sound effects mixing is currently not possible due to the Wolf3D sound engine being hardcoded to play monaural digitized sound with hardware stereo panning only.)
- Fixed glitch where doors that are "perpetually" open due to a partially-blocking corpse may suddenly close when an enemy passes over the corpse (not necessarily through the door). Incidentally this also fixes a special case in the automap where such doors are indicated as open only when the player looks at it (whereas they should be perpetually open due to the corpse).
- Fixed player weapon getting stuck in a firing frame when the player grabs the spear while firing.
- Fixed Save Game not disabling after episode completion.

1.45 (2021-11-20)
-----------------
- Added "nompu" command-line parameter to disable MPU-401 usage and revert to OPL2 music. Removing/renaming your MUSIC directory has the same effect. Just in case you REALLY want that...
- Fixed the sound attenuation behavior on SB pro, because SB pro volume level 7 of 15 (the minimum volume encoded into the Wolf3D volume lookup table) is actually near inaudible instead of half volume as one might expect. Since the developer intention seems to be for attenuating sounds only down to the half-volume point (coupled with the fact that non-SB-pro devices do not attenuate volume, making it "unfair" for SB pro owners if they don't hear some sounds), the value sent to the SB pro is now adjusted closer to the SB pro's real half-volume point. This fix also reduces (but not eliminates) sounds getting cut off abruptly -- which is actually a low-volume sound effect taking precedence over a high-volume one -- and also makes the sound positioning a bit more pleasing with headphones due to the more-balanced volume distribution.
- Added special cases for door sounds, which account for many of the sound cut-off issues players experience in Wolf3D. Direct door interaction by the player should now always play a sound regardless of a currently-playing sound's priority, but the sound can be overridden by higher-priority sound effects afterwards. Ambient door closing sounds will not override other door sounds anymore (but ambient door opening still will, to alert the player of an enemy opening a door). Reopening a currently-closing door should also now override the door-closing sound with a door-open sound. All that said, the real solution to the sound cut-off problem is to do software-based mixing of simultaneous sounds ala Doom, which would require significant engine changes and may be too much work for 286 (don't know when/if this will happen at all).
- Fixed classic Sound Blaster code where the first parts of digitized sounds can get dropped sometimes due to a wayward interrupt.
- Fixed new Covox support where the first few digitized sounds will not play (but subsequent ones work fine).
- Fixed slowdown issue with Covox and DMA-less SB support combined with very short non-digitized AdLib sound effects (i.e., the digitized sound's pitch lowers when the player runs into a wall or holds the Open key).
- Fixed DONOTHINGSND not getting played on tiles previously occupied by pushwalls (it now plays, as with any other tile, for consistency).
- PUSHWALLSND is now a positioned sound.
- Fixed rare possibility for the automap to break when the player is straferunning through a one-tile-wide corridor on lower framerates, which can result in the player skipping entire tiles (and breaking spotvis connectivity). Item pickup logic has also been fixed to not skip these tiles (except if COMP 4 is specified).
- Previous version cleared keyboard inputs after screen fade-/fizzle-in, but this changed the level start's feel significantly (players would start pressing directional keys during the fade-in only for the movement to not register, killing running starts). Changed so that only the LastScan code is cleared, which only affects the processing of the menu keys Esc and F1-F10 (which was the intended fix).
- Previous version broke some behavioral compatibility with existing maps with holowalls -- in particular, shooting inside a holowall should alert all enemies in the level. (Fixed in this version.)
- Reverted SAVENEARHEAP to its original value (it was modified to help reduce the memory requirement to 576 KB, but it apparently causes random crashes; still not sure why).

1.44 (2021-11-16)
-----------------
- Corrected a couple of crashing and corruption cases in the classic code. You should now be able to play Spear of Destiny from start to finish without crashing or experiencing corrupted graphics, even if you save/load your game a lot.
  - Now stops MM_GetPtr() from exhausting all available memory block structures (not the actual memory) over the course of the game. This is due to the page manager aggressively requesting new blocks every time the game goes into the play loop "until it fails" and causes the temporary block structure to be leaked, eventually bombing with "MM_ClearBlock: No purgable blocks!"
  - Fixed out-of-memory issue that can happen on rare memory configurations after the level restarts due to player death.
  - Fixed memory corruption issue where pushing a secret wall again while it is moving can cause random static map ornaments to become impassable. I have experienced it myself on E4M2 and SoD Map 1, and [this video](https://www.youtube.com/watch?v=_1ieQL_8wgA) shows how it can be triggered in SoD Map 2 (which renders it unwinnable if no enemies are left to help reverse the glitch).
  - Fixed memory corruption issue where loading a game and entering an already-pushed secret wall's original position puts the player in an undefined area, subsequently causing random walls to change to texture page number 1 (darkened stone wall).
  - Increased the conventional memory requirement to 576 KB (exactly 9/10ths of 640 KB) to avoid memory thrashing issues. You can now bypass this memory requirement with the "nomain" command-line parameter, forcing the game to run on a low-memory system. Please save your game often if you need to use "nomain" because the game is much more likely to crash.
- Gameplay fixes:
  - Fixed the classic "phantom doors" glitch where actors can pass through a closed door if a patrolling enemy was initially placed facing that door. This was fixed because it was clearly not intended by the developer when it happens on E4M6 on the hardest difficulty (in the top left room with the crowns). However, since this bug may be intentionally used in mods to surprise players with ambushes or to make "fake" locked doors that the player can pass through without a key, you can re-enable it by using the new COMP flag 64. (A related quirk, "holowalls", remains supported for mods without the need for COMP.)
  - Now gives the gold key to the player upon getting the spear; otherwise, a player can trick an enemy to open the locked door without the key, grab the spear, and get into an unwinnable final level (because you need the key to get out).
- Rendering fixes:
  - Now properly renders bonus item sprites that could not be picked up (e.g., 25-ammo boxes). Previously they seem to disappear as if they were already picked up, only to reappear when the player backs away.
  - Fixed rendering of door sides with respect to pushwalls:
    - If a pushwall's landing spot is adjacent to a door, the pushwall is now marked as a door side (see E2M3, rightmost pushwall of the map, also the start of a chain of secrets with lots of treasure).
    - If a pushwall passes against a wall that is marked as a door side, that wall is now rendered correctly with its regular texture instead of the door side texture (see E4M6 pushwall in the top left room with the crowns, or the E2M8 pushwall maze when choosing the pushwall that blocks you from getting the Apogee sign).
- Interface fixes:
  - Made sure quicksave message appears for at least 10 tics; otherwise, the message sometimes doesn't get displayed when your monitor's native refresh rate is less than 70Hz, e.g., when running on modern hardware or DOSBox.
  - Now displays up to 999:99 total episode time. This was previously capped at 99:99. Note that :99 shows instead of :59 to indicate timer overflow, like in the vanilla EXE. So now you will know the exact amount of time you wasted playing Wolf3D instead of working today! (Also because, even after so many hours of "debugging" wolfdosmpu, I still could not finish below 99:59 on SoD at the hardest difficulty with 100% completion ratios...)
  - Fixed typo in SoD ending text ("by" -> "be"). If it isn't obvious by now, I've been playing SoD a lot. :)
  - Fixed quicksave/quickload so that it immediately becomes active after a normal save/load. Also, if the user pressed Esc on the save/load screen, the quicksave/quickload slot will now revert to the previous one used instead of changing to the one currently selected by the cursor. (I've been victimized by this...)
  - Fixed so that keyboard and mouse input is always cleared after screen fade-/fizzle-ins (i.e., if you impatiently pressed Esc multiple times at a function key menu, the game screen won't anymore immediately fade back out to the main menu).

1.43 (2021-11-03)
-----------------
- Automap now makes doors glow if they have not been opened (more accurately, "haven't been _seen_ open"), to better visualize unexplored areas in a busy map. However, doors will only start glowing on levels started on version 1.43 onwards (i.e., your old savegames will still work fine, but no doors will glow until you proceed to the next level).
- Automap tracing has been improved to cut off visibility at unopened doors. This improvement reduces automap errors where visibility would "seep through" areas that the player hasn't explored yet but has already been "seen" by the ray-casting engine, which is caused by precision errors when checking for wall collisions. This issue also affects other source ports and derivative games that rely on the ray-casting engine for automapping, such as ECWolf and the DOS version of Super 3-D Noah's Ark. (The actual precision errors are left untouched because fixing them may affect the speed and determinism of the renderer and may cause subtle differences in gameplay.)
- Now marks previously-seen stationary enemies on the map if they are the only ones left and you have seen all of them (only on "Show hints if clear" automap mode or higher, renamed from "Show secrets if clear"). The rationale for this is that the automap is supposed to help the player determine where enemies may still be hiding, and if these stationary enemies were seen but never alerted, they should obviously remain where they are. If they are not marked (and the map is already mostly explored), then the player is forced to explore every part of the level again, which just wastes time and adds to frustration. However, if there are any moving enemies left (aside from ghosts), no enemy positions will be marked unless the player sees them directly, because their positions _should_ be uncertain to the player.
- Fixed modern-controls bug where items can still be picked up even when they are made inaccessible by pushing a secret wall on top of them. This bug did not affect the MPU-only version because the spotvis algorithm was modified only in the modern-controls version (to support showing walls on the automap).
- Fixed a classic bug where pushwall spots reactivate when another pushwall is pushed into its place and the user saves and reloads the game. To re-enable the previous behavior, use new COMP flag 32.
- Fixed so that the OPL2 device is cleaned up twice on system exit. The vanilla EXEs were actually doing this second clean-up in the main game code, but it does not respect the "noal" option, so it was #ifdef-ed out for safety. But then I later noticed that not cleaning up twice caused the game to sometimes not detect the OPL2 properly on the next startup, so I reimplemented it with proper support for "noal". This might be a DOSBox-only thing, but I'm not taking chances.
- Fixed a classic bug that causes sounds to get cut off at the last segment (which sometimes even causes lower-priority digitized sounds to never play again).
- Fixed a classic bug where the engine tries (and fails) to play back both digitized and non-digitized PC Speaker sounds simultaneously. What for? See the next fix. ;)
- If no Disney Sound Source is present (or if the "noss" command-line parameter is supplied), its menu option is now replaced with the glorious PC Speaker option. For DOSBox users, it is recommended to set the volume of the emulated PC Speaker device to 40 (or below) using the DOSBox command line "mixer spkr 40" for proper loudness relative to the emulated SB and FM devices.
- Door sound effects now revert to their non-digitized counterparts when playing on PC Speaker because the samples are too noisy and unintelligible (and play very often in a level); hopefully this makes PC Speaker digitized sound much more tolerable.

1.42 (2021-10-29)
-----------------
- Now detects whether proper data files have been provided for the EXE used.
- Fixed Change View dialog becoming unresponsive to user input during low-memory conditions.
- Now allows MPU-401 support even if AdLib is manually disabled via the "noal" command-line parameter. (The MPU code is partially dependent on the AdLib code, and previous wolfdosmpu versions disabled music altogether if "noal" was used.)
- Now does not disable the Sound Blaster when "noal" is specified (which the vanilla Wolf3D did); it now disables just the OPL2 sound effects.
- Now allows BLASTER environment variable to specify the MPU port even if the Sound Blaster is manually disabled via the "nosb" command-line parameter.
- Now sets "nopro" internally if BLASTER variable is present and does not have the T option (or is set to 0, 1, or 3). This way, DMA-less SB-compatible cards are given the option to enable Pro extensions.
- Added "always run" option for modern-controls version. Beware: Always-running in Wolf3D is a lot less fluid than always-running in later games like Doom or Quake (due to the lack of acceleration/deceleration), and is not recommended for people with motion sickness.

1.41 (2021-10-26)
-----------------
- Added support for DMA-less Sound-Blaster-compatible cards and Covox-Speech-Thing-compatible parallel port sound devices. See the [tips section](#dont-hurt-midi) for details. (Thanks to Bondi for testing!)

1.40 (2021-10-24)
-----------------
- Now sets MAXVISABLE to MAXACTORS (instead of a paltry hardcoded 50), so that all sprites render properly in extreme cases such as E4M8 (in the swastika-lights area) and E4M10 (where you can kill all 75 officers through a secret opening). Also fixed the "visability" algorithm so that animating actors always get drawn first. These fixes ensure that all active enemies and projectiles will never go invisible, even in custom levels that maximize the MAXACTORS limit. (Statics and corpses may still become invisible depending on their layout, but that has always happened in regular Wolf3D and it is still less of a problem with the increased MAXVISABLE limit.) _This bugfix can be disabled with new COMP flag 16._ (In case a mod uses the sprite limit to forcibly hide attackers, or for people who like the weirdness of fighting invisible officers on E4M10.)
- Added a 100-tick delay at the start of a song, to ensure that a previous turn-off-notes command is fully received by the MIDI device (and ensure that the music does not skip time at the beginning).
- Automap now has 4 modes: Normal, Show secrets if clear (if there are no enemies remaining (that can be killed)), Always show secrets (if you have seen their respective tiles), and Full map (also gives away enemy positions). I found myself wanting the in-between options because the full map gives too much away. "Show secrets if clear" is my preferred option for testing my memory of the maps, but "Always show secrets" can be handy if you don't like spamming the Open key on walls while desperately looking for medkits or ammo.
- Automap now assumes that a door is open if a corpse is blocking it. (Useful for E4M7's intended solution as described in the official hint manual.)
- Fixed completion stats display to not display garbage above 999; it will display the least significant digits instead like other score counters (this can theoretically happen when spam-killing SoD's spectres).

1.39 RC (2021-10-18)
--------------------
- Added music volume control, to allow users to balance the loudness of the music with the sound effects (suggested by mOBSCENE). Note that sound effects volume (both digitized and FM) can be changed through your sound card's mixer app. The MPU-401 device's volume, on the other hand, usually has no readily-available software-based control, justifying this feature.
- Made Tab key functions a separate menu -- it was previously unclear that the menu option cycles through four possibilities.
- Now supports negative parameters for COMP, to disable the specified options (and enable everything else).
- Player respawn now properly marks pushwalls and bonuses that have been previously pushed/seen. This modifies the savegame format -- older savegames will be automatically updated to the new format, but newer savegames will not work properly with older versions of wolfdosmpu (which you should generally avoid anyway).
- Added a special case to automap viscone rendering so that it "sees through" pillars and other block objects even if the area behind them is inaccessible. (Items in these inaccessible areas are still rendered in darker color.)
- Added a special case for rendering E3M10's ghosts in the automap (since they are not shootable but they're quite dangerous).

1.38 RC (2021-10-14)
--------------------
- Fixed more memory issues and crashing related to stats/map display, and reimplemented stats dialog to avoid using id's Message() routines (which seem to leak memory -- infamously, SoD's Tab+G+F10 God Mode cheat could crash the system randomly). I think I've eliminated the crashes for good, but if you still experience crashes, I would appreciate it if you contact me with steps to reproduce and savegame if possible (your system's memory configuration as reported by MEM will also be handy).
- Automap now pulsates the player's viewing area (via palette-cycling) for easier navigation.
- Automap now does not give away the status of doors that the player currently cannot see.
- Fix to a minor pushwall special case bug after grabbing the spear. (Should not actually appear on unmodified SoD.)
- Fixed COMP 0 being interpreted the same as COMP.

1.37 RC (2021-10-12)
--------------------
- Now enforces a 257000-byte memory minimum to the Wolf3D EXEs (previously only applied to the SoD EXEs), to help mitigate memory issues introduced by bug fixes. You can override this by using the debug mode command-line parameter "goobers". (SoD has always allowed overriding the memory minimum via "debugmode".)
- Even better item pickup code, should now not miss anything when running straight through a one-tile-wide row of items. (You can see this in action in the first attract-mode demo of Wolf3D registered/commercial, where one ammo clip in the middle of the row was skipped over on the vanilla EXE.)
- Improved Robert's Jukebox to show all tracks available in the game. To access Robert's Jukebox: Just after entering the command line to start the game (e.g. WOLF3DCM), the screen blanks and immediately shows the sign-on screen (the one with all the memory and hardware info). You have to press and hold M the moment the sign-on screen appears. (This also works on the vanilla EXEs, but will show a smaller list of tracks.)

1.36 RC (2021-10-11)
--------------------
- Added compatibility flags to purposely enable classic bugs/limitations of the engine (primarily for mods that assume these bugs/limitations). See the note on the COMP parameter in the [FAQ section](#faq).
- Fixed map fog not getting reset when getting the spear.
- Fixed phantom secret walls in the map after loading a savegame.
- Now marks all special doors and elevator tiles as magenta to easily spot them on the map.
- Implemented a workaround to reverse the engine's behavior of shrinking the play window in low memory conditions.

1.35 RC (2021-10-09)
--------------------
- Implemented mapping support for the modern-controls version. You can still load savegames between modern-controls and MPU-only versions, but the unlocked areas of the map will be lost if you load a modern-controls savegame on the MPU-only version and subsequently save it (since the MPU-only version ignores any extra data).
- Fixed pushwall glitch preventing 100% completion on many levels (on both modern-controls and MPU-only versions). With this fix, it should now be theoretically possible to complete all levels with 100% kills/secrets/treasures, with the sole exceptions of E2M8 and E6M10 (due to unreachable secrets). Please let me know if I missed any more bugs related to game completion; I have not fully tested all levels yet.

1.32 RC (2021-10-03)
--------------------
- Achieved savegame compatibility between the modern-controls and MPU-only versions (and even savegame compatibility between registered and commercial wolfdosmpu!), but this entails breaking savegame compatibility with previous versions of wolfdosmpu one last time. Hope you'll forgive me for this.
- Added cmd scripts for verifying MAP files to ensure that the DATASEG layout is preserved. (These cmd scripts uses unix commands internally -- unxutils or equivalent must be installed on your system.)
- Fixed a memory issue with the SoD God Mode cheat (Tab+G+F10). In turn, the same code was applied to the completion stats display, eliminating flickering when it is being used.

1.31 BETA (2021-10-02)
----------------------
- Fixed Tab option setting being ignored.

1.30 BETA (2021-10-01)
----------------------
- Support for WASD-style modern controls is added. These options can be turned off in the Options menu. Alternatively, you can download a version that does not have these options at all, if you consider these "cheating".

1.25 BETA (2021-09-30)
----------------------
- Now prevents id's memory manager from messing with wolfdosmpu's buffer, increasing compatibility with AWEUTIL (the NOXMS parameter is not needed anymore) as well as EMM386 and QEMM.

1.24 BETA (2021-09-28)
----------------------
- REALLY fixed the regression this time. (Thanks to Gmlb256 for testing!)

1.23 BETA (2021-09-28)
----------------------
- Fixed a regression in MPU-401 detection code.

1.22 BETA (2021-09-27)
----------------------
- Fixed Read This support in WOLF3DRM.EXE and WOLF3DSM.EXE so that they don't shrink the view window every time (and cause a crash once in a while). This is due to the help screens taking up a lot of memory, which Wolf3D compensates for by releasing some memory used by the view window.

1.21 BETA (2021-09-26)
----------------------
- Partial compatibility with Sound Blaster AWEUTIL (MPU-401 port emulation) in DOS. The game must be started with the NOXMS parameter to prevent crashing. (Thanks to Gmlb256 for testing!)

1.20 BETA (2021-09-25)
----------------------
- Rewrote MPU code to fully reuse the existing OPL sequencer state variables for MPU playback.
- Currently testing the removal of the clear interrupt flag instruction in the inner assembly loop. Maybe that will fix issues for some hardware. Maybe not. Maybe it's worse. Please test.
- Altered PRJ file to strip debugging symbols for better LZEXE compression. (decided after discussion with Akuma and Gmlb256)
- Replaced SIGNON.OBJ and GAMEPAL.OBJ with ones from the [Wolf3D game source recreation project](https://bitbucket.org/gamesrc-ver-recreation/wolf3d/). For the Wolf3D screen, I personally dislike the weird colors of the Activision logo, so I opted for the id logo for neutrality. :)
- Moved all the license files to the README section. I believe the general consensus is that Wolf3D's source code came under dual licenses -- limited non-commercial license and GPL). Like many other mods, I defer the option to the user.

1.10 BETA (2021-09-24)
----------------------
- Added support for Apogee registered version 1.4/1.4g by using techniques to reduce data segment usage (e.g., by allocating strings as individual characters in the code segment).
- Corrected a very rare edge case where midiTick (later versions: mpuTick) may get executed before the length of the MIDI file has been calculated, and the song counter could potentially go out-of-bounds.
- Limited the MPU port selection in BLASTER environment variable to the range 2xx-3xx hex because poking some ports by accident can potentially kill your system!
- Changed sound options menu to display "MPU-401 (General MIDI)" instead of "AdLib/Sound Blaster". (suggested by Gmlb256)
- Added Fabrice Bellard's [LZEXE](https://bellard.org/lzexe.html) compressor in the build workflow. This is the original compressor used by id to fit the shareware game on a single floppy disk. (suggested by Gmlb256)
- Revamped build workflow to allow building the original OPL2 versions (essentially "vanilla" Wolf3D) by specifying any argument in the version selection batch files.

1.00 BETA (2021-09-22)
----------------------
- Initial release.


