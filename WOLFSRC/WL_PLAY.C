// WL_PLAY.C

#include "WL_DEF.H"
#pragma hdrstop


/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define sc_Question	0x35

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

boolean		madenoise;					// true when shooting or screaming

exit_t		playstate;

int			DebugOk;

objtype 	objlist[MAXACTORS],*new,*obj,*player,*lastobj,
			*objfreelist,*killerobj;

unsigned	farmapylookup[MAPSIZE];
byte		*nearmapylookup[MAPSIZE];

boolean		singlestep,godmode,noclip;
int			extravbls;

byte		tilemap[MAPSIZE][MAPSIZE];	// wall values only
byte		spotvis[MAPSIZE][MAPSIZE];
objtype		*actorat[MAPSIZE][MAPSIZE];

//
// replacing refresh manager
//
unsigned	mapwidth,mapheight,tics;
boolean		compatability;
byte		*updateptr;
unsigned	mapwidthtable[64];
unsigned	uwidthtable[UPDATEHIGH];
unsigned	blockstarts[UPDATEWIDE*UPDATEHIGH];
byte		update[UPDATESIZE];

//
// control info
//
boolean		mouseenabled,joystickenabled,joypadenabled,joystickprogressive;
int			joystickport;
#ifdef WASD
int			dirscan[4] = {sc_W,sc_D,sc_S,sc_A};
int			buttonscan[NUMBUTTONS] =
			{sc_Control,sc_Alt,sc_LShift,sc_Space,sc_1,sc_2,sc_3,sc_4};
#else  // WASD
int			dirscan[4] = {sc_UpArrow,sc_RightArrow,sc_DownArrow,sc_LeftArrow};
int			buttonscan[NUMBUTTONS] =
			{sc_Control,sc_Alt,sc_RShift,sc_Space,sc_1,sc_2,sc_3,sc_4};
#endif // WASD
int			buttonmouse[4]={bt_attack,bt_strafe,bt_use,bt_nobutton};
int			buttonjoy[4]={bt_attack,bt_strafe,bt_use,bt_run};

int			viewsize;

boolean		buttonheld[NUMBUTTONS];

boolean		demorecord,demoplayback;
char		far *demoptr, far *lastdemoptr;
memptr		demobuffer;

//
// curent user input
//
int			controlx,controly;		// range from -100 to 100 per tic
boolean		buttonstate[NUMBUTTONS];

#ifdef WASD
int			far	controlmouse;
int			far	tabstate;

boolean		far	keysalwaysstrafe;
boolean		far	mouseturningonly;
int			far	tabfunction;

unsigned	far	killaccessible;
unsigned	far	secretaccessible;
unsigned	far	treasureaccessible;

#endif // WASD


//===========================================================================


void	CenterWindow(word w,word h);
void 	InitObjList (void);
void 	RemoveObj (objtype *gone);
void 	PollControls (void);
void 	StopMusic(void);
void 	StartMusic(void);
void	PlayLoop (void);

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/


objtype dummyobj;

//
// LIST OF SONGS FOR EACH VERSION
//
int songs[]=
{
#ifndef SPEAR
 //
 // Episode One
 //
 GETTHEM_MUS,
 SEARCHN_MUS,
 POW_MUS,
 SUSPENSE_MUS,
 GETTHEM_MUS,
 SEARCHN_MUS,
 POW_MUS,
 SUSPENSE_MUS,

 WARMARCH_MUS,	// Boss level
 CORNER_MUS,	// Secret level

 //
 // Episode Two
 //
 NAZI_OMI_MUS,
 PREGNANT_MUS,
 GOINGAFT_MUS,
 HEADACHE_MUS,
 NAZI_OMI_MUS,
 PREGNANT_MUS,
 HEADACHE_MUS,
 GOINGAFT_MUS,

 WARMARCH_MUS,	// Boss level
 DUNGEON_MUS,	// Secret level

 //
 // Episode Three
 //
 INTROCW3_MUS,
 NAZI_RAP_MUS,
 TWELFTH_MUS,
 ZEROHOUR_MUS,
 INTROCW3_MUS,
 NAZI_RAP_MUS,
 TWELFTH_MUS,
 ZEROHOUR_MUS,

 ULTIMATE_MUS,	// Boss level
 PACMAN_MUS,	// Secret level

 //
 // Episode Four
 //
 GETTHEM_MUS,
 SEARCHN_MUS,
 POW_MUS,
 SUSPENSE_MUS,
 GETTHEM_MUS,
 SEARCHN_MUS,
 POW_MUS,
 SUSPENSE_MUS,

 WARMARCH_MUS,	// Boss level
 CORNER_MUS,	// Secret level

 //
 // Episode Five
 //
 NAZI_OMI_MUS,
 PREGNANT_MUS,
 GOINGAFT_MUS,
 HEADACHE_MUS,
 NAZI_OMI_MUS,
 PREGNANT_MUS,
 HEADACHE_MUS,
 GOINGAFT_MUS,

 WARMARCH_MUS,	// Boss level
 DUNGEON_MUS,	// Secret level

 //
 // Episode Six
 //
 INTROCW3_MUS,
 NAZI_RAP_MUS,
 TWELFTH_MUS,
 ZEROHOUR_MUS,
 INTROCW3_MUS,
 NAZI_RAP_MUS,
 TWELFTH_MUS,
 ZEROHOUR_MUS,

 ULTIMATE_MUS,	// Boss level
 FUNKYOU_MUS		// Secret level
#else

 //////////////////////////////////////////////////////////////
 //
 // SPEAR OF DESTINY TRACKS
 //
 //////////////////////////////////////////////////////////////
 XTIPTOE_MUS,
 XFUNKIE_MUS,
 XDEATH_MUS,
 XGETYOU_MUS,		// DON'T KNOW
 ULTIMATE_MUS,	// Trans Gr”sse

 DUNGEON_MUS,
 GOINGAFT_MUS,
 POW_MUS,
 TWELFTH_MUS,
 ULTIMATE_MUS,	// Barnacle Wilhelm BOSS

 NAZI_OMI_MUS,
 GETTHEM_MUS,
 SUSPENSE_MUS,
 SEARCHN_MUS,
 ZEROHOUR_MUS,
 ULTIMATE_MUS,	// Super Mutant BOSS

 XPUTIT_MUS,
 ULTIMATE_MUS,	// Death Knight BOSS

 XJAZNAZI_MUS,	// Secret level
 XFUNKIE_MUS,	// Secret level (DON'T KNOW)

 XEVIL_MUS		// Angel of Death BOSS

#endif
};


/*
=============================================================================

						  USER CONTROL

=============================================================================
*/


#define BASEMOVE		35
#define RUNMOVE			70
#define BASETURN		35
#define RUNTURN			70

#define JOYSCALE		2

/*
===================
=
= PollKeyboardButtons
=
===================
*/

void PollKeyboardButtons (void)
{
	int		i;

	for (i=0;i<NUMBUTTONS;i++)
		if (Keyboard[buttonscan[i]])
			buttonstate[i] = true;
}


/*
===================
=
= PollMouseButtons
=
===================
*/

void PollMouseButtons (void)
{
	int	buttons;

	buttons = IN_MouseButtons ();

	if (buttons&1)
		buttonstate[buttonmouse[0]] = true;
	if (buttons&2)
		buttonstate[buttonmouse[1]] = true;
	if (buttons&4)
		buttonstate[buttonmouse[2]] = true;
}



/*
===================
=
= PollJoystickButtons
=
===================
*/

void PollJoystickButtons (void)
{
	int	buttons;

	buttons = IN_JoyButtons ();

	if (joystickport && !joypadenabled)
	{
		if (buttons&4)
			buttonstate[buttonjoy[0]] = true;
		if (buttons&8)
			buttonstate[buttonjoy[1]] = true;
	}
	else
	{
		if (buttons&1)
			buttonstate[buttonjoy[0]] = true;
		if (buttons&2)
			buttonstate[buttonjoy[1]] = true;
		if (joypadenabled)
		{
			if (buttons&4)
				buttonstate[buttonjoy[2]] = true;
			if (buttons&8)
				buttonstate[buttonjoy[3]] = true;
		}
	}
}


/*
===================
=
= PollKeyboardMove
=
===================
*/

void PollKeyboardMove (void)
{
#ifdef WASD
	// pressing left/right automatically engages strafe
	// (doing it this way preserves demo compatibility)
	if (keysalwaysstrafe && (Keyboard[dirscan[di_west]] || Keyboard[dirscan[di_east]]))
		buttonstate[bt_strafe] = true;
#endif // WASD

	if (buttonstate[bt_run])
	{
		if (Keyboard[dirscan[di_north]])
			controly -= RUNMOVE*tics;
		if (Keyboard[dirscan[di_south]])
			controly += RUNMOVE*tics;
		if (Keyboard[dirscan[di_west]])
			controlx -= RUNMOVE*tics;
		if (Keyboard[dirscan[di_east]])
			controlx += RUNMOVE*tics;
	}
	else
	{
		if (Keyboard[dirscan[di_north]])
			controly -= BASEMOVE*tics;
		if (Keyboard[dirscan[di_south]])
			controly += BASEMOVE*tics;
		if (Keyboard[dirscan[di_west]])
			controlx -= BASEMOVE*tics;
		if (Keyboard[dirscan[di_east]])
			controlx += BASEMOVE*tics;
	}
}


/*
===================
=
= PollMouseMove
=
===================
*/

void PollMouseMove (void)
{
	int	mousexmove,mouseymove;

	Mouse(MDelta);
	mousexmove = _CX;
	mouseymove = _DX;

#ifdef WASD
	// if key-strafe is engaged, and we're not recording a demo, store mouse movement in aux variable so we can still account for it later
	if (keysalwaysstrafe && (Keyboard[dirscan[di_west]] || Keyboard[dirscan[di_east]]) && ! demorecord)
		controlmouse += mousexmove*10/(13-mouseadjustment);
	else
#endif // WASD
	controlx += mousexmove*10/(13-mouseadjustment);
#ifdef WASD
	if (! mouseturningonly || buttonstate[bt_strafe] && ! (keysalwaysstrafe && (Keyboard[dirscan[di_west]] || Keyboard[dirscan[di_east]])))
#endif // WASD
	controly += mouseymove*20/(13-mouseadjustment);
}



/*
===================
=
= PollJoystickMove
=
===================
*/

void PollJoystickMove (void)
{
	int	joyx,joyy;

	INL_GetJoyDelta(joystickport,&joyx,&joyy);

	if (joystickprogressive)
	{
		if (joyx > 64)
			controlx += (joyx-64)*JOYSCALE*tics;
		else if (joyx < -64)
			controlx -= (-joyx-64)*JOYSCALE*tics;
		if (joyy > 64)
			controlx += (joyy-64)*JOYSCALE*tics;
		else if (joyy < -64)
			controly -= (-joyy-64)*JOYSCALE*tics;
	}
	else if (buttonstate[bt_run])
	{
		if (joyx > 64)
			controlx += RUNMOVE*tics;
		else if (joyx < -64)
			controlx -= RUNMOVE*tics;
		if (joyy > 64)
			controly += RUNMOVE*tics;
		else if (joyy < -64)
			controly -= RUNMOVE*tics;
	}
	else
	{
		if (joyx > 64)
			controlx += BASEMOVE*tics;
		else if (joyx < -64)
			controlx -= BASEMOVE*tics;
		if (joyy > 64)
			controly += BASEMOVE*tics;
		else if (joyy < -64)
			controly -= BASEMOVE*tics;
	}
}


/*
===================
=
= PollControls
=
= Gets user or demo input, call once each frame
=
= controlx		set between -100 and 100 per tic
= controly
= buttonheld[]	the state of the buttons LAST frame
= buttonstate[]	the state of the buttons THIS frame
=
===================
*/

void PollControls (void)
{
	int		max,min,i;
	byte	buttonbits;

//
// get timing info for last frame
//
	if (demoplayback)
	{
		while (TimeCount<lasttimecount+DEMOTICS)
		;
		TimeCount = lasttimecount + DEMOTICS;
		lasttimecount += DEMOTICS;
		tics = DEMOTICS;
	}
	else if (demorecord)			// demo recording and playback needs
	{								// to be constant
//
// take DEMOTICS or more tics, and modify Timecount to reflect time taken
//
		while (TimeCount<lasttimecount+DEMOTICS)
		;
		TimeCount = lasttimecount + DEMOTICS;
		lasttimecount += DEMOTICS;
		tics = DEMOTICS;
	}
	else
		CalcTics ();

	controlx = 0;
	controly = 0;
#ifdef WASD
	controlmouse = 0;
#endif // WASD
	memcpy (buttonheld,buttonstate,sizeof(buttonstate));
	memset (buttonstate,0,sizeof(buttonstate));

	if (demoplayback)
	{
	//
	// read commands from demo buffer
	//
		buttonbits = *demoptr++;
		for (i=0;i<NUMBUTTONS;i++)
		{
			buttonstate[i] = buttonbits&1;
			buttonbits >>= 1;
		}

		controlx = *demoptr++;
		controly = *demoptr++;

		if (demoptr == lastdemoptr)
			playstate = ex_completed;		// demo is done

		controlx *= (int)tics;
		controly *= (int)tics;

		return;
	}


//
// get button states
//
	PollKeyboardButtons ();

	if (mouseenabled)
		PollMouseButtons ();

	if (joystickenabled)
		PollJoystickButtons ();

//
// get movements
//
	PollKeyboardMove ();

	if (mouseenabled)
		PollMouseMove ();

	if (joystickenabled)
		PollJoystickMove ();

//
// bound movement to a maximum
//
	max = 100*tics;
	min = -max;
	if (controlx > max)
		controlx = max;
	else if (controlx < min)
		controlx = min;

	if (controly > max)
		controly = max;
	else if (controly < min)
		controly = min;

	if (demorecord)
	{
	//
	// save info out to demo buffer
	//
		controlx /= (int)tics;
		controly /= (int)tics;

		buttonbits = 0;

		for (i=NUMBUTTONS-1;i>=0;i--)
		{
			buttonbits <<= 1;
			if (buttonstate[i])
				buttonbits |= 1;
		}

		*demoptr++ = buttonbits;
		*demoptr++ = controlx;
		*demoptr++ = controly;

		if (demoptr >= lastdemoptr)
			Quit ("Demo buffer overflowed!");

		controlx *= (int)tics;
		controly *= (int)tics;
	}
}



//==========================================================================



///////////////////////////////////////////////////////////////////////////
//
//	CenterWindow() - Generates a window of a given width & height in the
//		middle of the screen
//
///////////////////////////////////////////////////////////////////////////

#define MAXX	320
#define MAXY	160

void	CenterWindow(word w,word h)
{
	FixOfs ();
	US_DrawWindow(((MAXX / 8) - w) / 2,((MAXY / 8) - h) / 2,w,h);
}

//===========================================================================


#ifdef WASD

#define STACKITEM(x, y, z) ((z << 10) + (x << 6) + y)

void PushCell(unsigned x, unsigned y, unsigned z, controldir_t dir, unsigned far **stackptr)
{
	unsigned actor;

	// do not flood through already-visited cells
	if ((spotvis[x][y] & 0x0c) >= z)
		return;

	actor = (unsigned) actorat[x][y];
	if (actor == 1 && tilemap[x][y] == 0)
	{
		// blocking static objects have an actorat value of 1 but do not contain anything in the tilemap
		spotvis[x][y] |= 0x74;

		// flood through (but only as visible)
		*(*stackptr)++ = STACKITEM(x, y, 0x04);
	}
	else if (! actor || actor > 255)
	{
		// blank spaces or actors
		spotvis[x][y] |= z;

		// flood through (whether accessible or visible)
		*(*stackptr)++ = STACKITEM(x, y, z);
	}
	else if (actor > 127)
	{
		int doornum = tilemap[x][y] & ~0x80;
		int lock = doorobjlist[doornum].lock;

		// mark the door and whether it is locked
		if (lock < dr_lock1 || lock > dr_lock4)
			spotvis[x][y] |= 0x30 + z;
		else
			spotvis[x][y] |= 0x50 + z;

		// flood through door only if accessible and unlocked or player has the key
		if (z == 0x0c && (lock < dr_lock1 || lock > dr_lock4 || gamestate.keys & (1 << (lock - dr_lock1))))
			*(*stackptr)++ = STACKITEM(x, y, z);
	}
	else if (*(mapsegs[1] + farmapylookup[y] + x) == PUSHABLETILE)
	{
		// secret (does not flood through)
		if (z == 0x0c
			&& ! (spotvis[x][y] & 0x80)		// count an accessible secret only once
			&& (dir == di_west && ! actorat[x - 1][y]
				|| dir == di_east && ! actorat[x + 1][y]
				|| dir == di_north && ! actorat[x][y - 1]
				|| dir == di_south && ! actorat[x][y + 1]))
		{
			spotvis[x][y] |= 0x80;			// mark secret as accessible (must use different bit because the normal accessible bit will give the secret away)
		}
	}
	else if (tilemap[x][y] == ELEVATORTILE && (dir == di_west || dir == di_east))
	{
		// elevator (does not flood through)
		spotvis[x][y] |= 0x50 + z;
	}
	else
	{
		// wall (does not flood through)
		spotvis[x][y] |= 0x7c;				// special case for walls: accessible bit is always 1 (that is, always bright)
	}
}

void CheckAccessible()
{
	objtype *obj;
	statobj_t *statptr;
	unsigned far *stackptr;
	memptr tempmem;
	unsigned current;
	byte x, y, z;

	MM_GetPtr(&tempmem, 64 * 64 * sizeof(unsigned));
	stackptr = (unsigned far *) tempmem;

	killaccessible = gamestate.killcount;
	secretaccessible = gamestate.secretcount;
	treasureaccessible = gamestate.treasurecount;

	*stackptr++ = STACKITEM(player->tilex, player->tiley, 0x0c);
	spotvis[player->tilex][player->tiley] = 0x4d;
	while (stackptr != (unsigned far *) tempmem)
	{
		current = *--stackptr;
		x = (current >> 6) & 0x3f;
		y = current & 0x3f;
		z = (current >> 10) & 0x0c;

		if (x > 0)	PushCell(x - 1, y, z, di_west, &stackptr);
		if (x < 63)	PushCell(x + 1, y, z, di_east, &stackptr);
		if (y > 0)	PushCell(x, y - 1, z, di_north, &stackptr);
		if (y < 63)	PushCell(x, y + 1, z, di_south, &stackptr);
	}

	for (statptr = &statobjlist[0]; statptr != laststatobj; statptr++)
	{
		if (statptr->shapenum != -1 && statptr->flags & FL_BONUS)
		{
			byte n = statptr->itemnumber;
			if (n == bo_cross || n == bo_chalice || n == bo_bible || n == bo_crown || n == bo_fullheal || n == bo_key1 || n == bo_key2)
			{
				if (*statptr->visspot & 0x08)
				{
					// keys get marked as treasure for convenience, but don't count to total
					if (n != bo_key1 && n != bo_key2)
						treasureaccessible++;
				}
				else if (tabfunction == 3)
					*statptr->visspot |= 0x04;	// treasure on unseen tiles still gets marked in full map mode

				if (*statptr->visspot != 0x4d)	// if player isn't there...
					*statptr->visspot |= 0x60;	// mark the treasure
			}
			else if (n != block && n != dressing)
			{
				if (tabfunction == 3)
					*statptr->visspot |= 0x04;	// bonuses on unseen tiles still gets marked in full map mode

				if (*statptr->visspot != 0x4d)	// if player isn't there...
					*statptr->visspot |= 0x10;	// mark the bonus
			}
		}
	}

	for (y = 0; y < 64; y++)
	{
		for (x = 0; x < 64; x++)
		{
			if (*(mapsegs[1] + farmapylookup[y] + x) == PUSHABLETILE)
			{
				if (spotvis[x][y] & 0x80)
				{
					if (tabfunction == 3)
						spotvis[x][y] |= 0x28;	// mark the secret
					else
						spotvis[x][y] |= 0x7c;	// masquerade secret as a wall
					secretaccessible++;
				}
				else if (tabfunction == 3)
					spotvis[x][y] |= 0x24;	// secrets on unseen tiles still get marked in full map mode
				else
					spotvis[x][y] |= 0x7c;	// masquerade secret as a wall
			}
		}
	}

	for (obj = player->next; obj; obj = obj->next)
	{
		if (obj->flags & FL_SHOOTABLE)
		{
			byte *visspot = &spotvis[obj->tilex][obj->tiley];

			// enemies are accessible when visible
			if (*visspot & 0x04)
				killaccessible++;

			*visspot &= ~0x08;		// enemy color is always dark red

			if (tabfunction == 3)
				*visspot |= 0x04;	// enemy on unseen tiles still gets marked in full map mode

			if (tabfunction == 3 || *visspot & 0x01)	// if full map mode or player directly sees the enemy...
			{
				*visspot &= 0x0f;	// always draw enemy over treasure
				*visspot |= 0x40;	// mark the enemy
			}
		}
	}

	MM_FreePtr(&tempmem);
}

#endif // WASD


/*
=====================
=
= CheckKeys
=
=====================
*/

void CheckKeys (void)
{
	int		i;
	byte	scan;
	unsigned	temp;


	if (screenfaded || demoplayback)	// don't do anything with a faded screen
		return;

	scan = LastScan;


	#ifdef SPEAR
	//
	// SECRET CHEAT CODE: TAB-G-F10
	//
	if (Keyboard[sc_Tab] &&
		Keyboard[sc_G] &&
		Keyboard[sc_F10])
	{
#ifdef WOLFDOSMPU
		ClearMemory();
		VW_ScreenToScreen (displayofs,bufferofs,80,160);
#endif // WOLFDOSMPU
		WindowH = 160;
		if (godmode)
		{
			Message ("God mode OFF");
			SD_PlaySound (NOBONUSSND);
		}
		else
		{
			Message ("God mode ON");
			SD_PlaySound (ENDBONUS2SND);
		}

		IN_Ack();
#ifdef WOLFDOSMPU
		PM_CheckMainMem();
#endif // WOLFDOSMPU
		godmode ^= 1;
		DrawAllPlayBorderSides ();
		IN_ClearKeysDown();
		return;
	}
	#endif


	//
	// SECRET CHEAT CODE: 'MLI'
	//
	if (Keyboard[sc_M] &&
		Keyboard[sc_L] &&
		Keyboard[sc_I])
	{
		gamestate.health = 100;
		gamestate.ammo = 99;
		gamestate.keys = 3;
		gamestate.score = 0;
		gamestate.TimeCount += 42000L;
		GiveWeapon (wp_chaingun);

		DrawWeapon();
		DrawHealth();
		DrawKeys();
		DrawAmmo();
		DrawScore();

		ClearMemory ();
		CA_CacheGrChunk (STARTFONT+1);
		ClearSplitVWB ();
		VW_ScreenToScreen (displayofs,bufferofs,80,160);

		Message(STR_CHEATER1"\n"
				STR_CHEATER2"\n\n"
				STR_CHEATER3"\n"
				STR_CHEATER4"\n"
				STR_CHEATER5);

		UNCACHEGRCHUNK(STARTFONT+1);
		PM_CheckMainMem ();
		IN_ClearKeysDown();
		IN_Ack();

		DrawAllPlayBorder ();
	}

	//
	// OPEN UP DEBUG KEYS
	//
#ifndef SPEAR
	if (Keyboard[sc_BackSpace] &&
		Keyboard[sc_LShift] &&
		Keyboard[sc_Alt] &&
		MS_CheckParm("goobers"))
#else
	if (Keyboard[sc_BackSpace] &&
		Keyboard[sc_LShift] &&
		Keyboard[sc_Alt] &&
		MS_CheckParm("debugmode"))
#endif
	{
	 ClearMemory ();
	 CA_CacheGrChunk (STARTFONT+1);
	 ClearSplitVWB ();
	 VW_ScreenToScreen (displayofs,bufferofs,80,160);

	 Message("Debugging keys are\nnow available!");
	 UNCACHEGRCHUNK(STARTFONT+1);
	 PM_CheckMainMem ();
	 IN_ClearKeysDown();
	 IN_Ack();

	 DrawAllPlayBorderSides ();
	 DebugOk=1;
	}

	//
	// TRYING THE KEEN CHEAT CODE!
	//
	if (Keyboard[sc_B] &&
		Keyboard[sc_A] &&
		Keyboard[sc_T])
	{
	 ClearMemory ();
	 CA_CacheGrChunk (STARTFONT+1);
	 ClearSplitVWB ();
	 VW_ScreenToScreen (displayofs,bufferofs,80,160);

	 Message("Commander Keen is also\n"
			 "available from Apogee, but\n"
			 "then, you already know\n"
			 "that - right, Cheatmeister?!");

	 UNCACHEGRCHUNK(STARTFONT+1);
	 PM_CheckMainMem ();
	 IN_ClearKeysDown();
	 IN_Ack();

	 DrawAllPlayBorder ();
	}

//
// pause key weirdness can't be checked as a scan code
//
	if (Paused)
	{
		bufferofs = displayofs;
		LatchDrawPic (20-4,80-2*8,PAUSEDPIC);
		SD_MusicOff();
		IN_Ack();
		IN_ClearKeysDown ();
		SD_MusicOn();
		Paused = false;
		if (MousePresent)
			Mouse(MDelta);	// Clear accumulated mouse movement
		return;
	}


//
// F1-F7/ESC to enter control panel
//
	if (
#ifndef DEBCHECK
		scan == sc_F10 ||
#endif
		scan == sc_F9 ||
		scan == sc_F7 ||
		scan == sc_F8)			// pop up quit dialog
	{
		ClearMemory ();
		ClearSplitVWB ();
		VW_ScreenToScreen (displayofs,bufferofs,80,160);
		US_ControlPanel(scan);

		 DrawAllPlayBorderSides ();

		if (scan == sc_F9)
		  StartMusic ();

		PM_CheckMainMem ();
		SETFONTCOLOR(0,15);
		IN_ClearKeysDown();
		return;
	}

	if ( (scan >= sc_F1 && scan <= sc_F9) || scan == sc_Escape)
	{
		StopMusic ();
		ClearMemory ();
		VW_FadeOut ();

		US_ControlPanel(scan);

		SETFONTCOLOR(0,15);
		IN_ClearKeysDown();
		DrawPlayScreen ();
		if (!startgame && !loadedgame)
		{
			VW_FadeIn ();
			StartMusic ();
		}
		if (loadedgame)
			playstate = ex_abort;
		lasttimecount = TimeCount;
		if (MousePresent)
			Mouse(MDelta);	// Clear accumulated mouse movement
		PM_CheckMainMem ();
		return;
	}

//
// TAB-? debug keys
//
#ifdef WASD
	if (tabstate == 2)	// only if Tab was pressed before and *another* key is pressed...
#endif // WASD
	if (Keyboard[sc_Tab] && DebugOk)
	{
		CA_CacheGrChunk (STARTFONT);
		fontnumber=0;
		SETFONTCOLOR(0,15);
		DebugKeys();
		if (MousePresent)
			Mouse(MDelta);	// Clear accumulated mouse movement
		lasttimecount = TimeCount;
		return;
	}

#ifdef WASD
	if (Keyboard[sc_Tab])
	{
		if (tabstate == 0)
			tabstate = 1;
	}
	else
	{
		if (tabstate == 1 && tabfunction)
		{
			char sz[76];
			int i;
			int oldview = viewwidth;

			tabstate = 2;	// do not allow next tab press to show KST stats again

			// CheckAccessible can potentially use memory that normally goes to the view window;
			// since the memory manager shrinks the view window to accommodate these requests,
			// reclaim the view window
			ClearMemory();
			CheckAccessible();
			MM_SortMem();
			if (oldview != viewwidth)
				NewViewSize(oldview / 16);

#define PRINTCOUNT(c) {						\
	if (c >= 100)							\
		sz[i++] = '0' + c / 100;			\
	if (c >= 10)							\
		sz[i++] = '0' + (c % 100) / 10;		\
	sz[i++] = '0' + (c % 10);				\
}
#define PRINTTOTAL(a, t) {					\
	sz[i++] = '/';							\
	if (a >= 100)							\
		sz[i++] = '0' + a / 100;			\
	if (a >= 10)							\
		sz[i++] = '0' + (a % 100) / 10;		\
	sz[i++] = '0' + (a % 10);				\
	if (a < t)								\
		sz[i++] = '^';						\
}


			memset(sz, ' ', 76);

			if (tabfunction == 1)
			{
				// ref string:  "           Kills: 000/000_:      Secrets: 000/000  :_    Treasures: 000/000"
				// we don't actually declare it as a string though, because BC++ will put it in the data segment
				i = 11;
				sz[i++] = 'K';
				sz[i++] = 'i';
				sz[i++] = 'l';
				sz[i++] = 'l';
				sz[i++] = 's';
				sz[i++] = ':';
				sz[i++] = ' ';
				PRINTCOUNT(gamestate.killcount);
				PRINTTOTAL(killaccessible, gamestate.killtotal);
				i = 25;
				sz[i++] = '\n';
				sz[i++] = ':';
				i = 33;
				sz[i++] = 'S';
				sz[i++] = 'e';
				sz[i++] = 'c';
				sz[i++] = 'r';
				sz[i++] = 'e';
				sz[i++] = 't';
				sz[i++] = 's';
				sz[i++] = ':';
				sz[i++] = ' ';
				PRINTCOUNT(gamestate.secretcount);
				PRINTTOTAL(secretaccessible, gamestate.secrettotal);
				i = 51;
				sz[i++] = ':';
				sz[i++] = '\n';
				i = 57;
				sz[i++] = 'T';
				sz[i++] = 'r';
				sz[i++] = 'e';
				sz[i++] = 'a';
				sz[i++] = 's';
				sz[i++] = 'u';
				sz[i++] = 'r';
				sz[i++] = 'e';
				sz[i++] = 's';
				sz[i++] = ':';
				sz[i++] = ' ';
				PRINTCOUNT(gamestate.treasurecount);
				PRINTTOTAL(treasureaccessible, gamestate.treasuretotal);
				sz[75] = 0;

				ClearMemory();
				VW_ScreenToScreen(displayofs,bufferofs,80,160);
				WindowH = 160;
				Message(sz);
				IN_Ack();
				if (Keyboard[sc_Escape])
					IN_ClearKeysDown();		// don't allow Escape to trigger menu
				PM_CheckMainMem();
				DrawAllPlayBorderSides();
				if (MousePresent)
					Mouse(MDelta);	// Clear accumulated mouse movement
			}
			else
			{
				byte x, y;
				byte mask = 0x0c;
				if (tabfunction < 3)
					mask = 0x02;

				ClearMemory();

				// clear screen except for floor display
				VWB_Bar(0, 0, 320, 163, 127);
				VWB_Bar(42, 163, 1, 35, 126);
				VWB_Bar(43, 163, 277, 35, 127);

				CA_CacheGrChunk(STARTFONT);
				fontnumber = 0;
				fontcolor = 12;
				px = 6;
				py = 4;
				i = 0;
				sz[i++] = 'K';
				sz[i++] = ':';
				sz[i++] = 0;
				VWB_DrawPropString(sz);
				px = 14;
				py = 16;
				i = 0;
				PRINTCOUNT(gamestate.killcount);
				sz[i++] = 0;
				VWB_DrawPropString(sz);
				px = 22;
				py = 28;
				i = 0;
				PRINTTOTAL(killaccessible, gamestate.killtotal);
				sz[i++] = 0;
				VWB_DrawPropString(sz);
				fontcolor = 10;
				px = 6;
				py = 52;
				i = 0;
				sz[i++] = 'S';
				sz[i++] = ':';
				sz[i++] = 0;
				VWB_DrawPropString(sz);
				px = 14;
				py = 64;
				i = 0;
				PRINTCOUNT(gamestate.secretcount);
				sz[i++] = 0;
				VWB_DrawPropString(sz);
				px = 22;
				py = 76;
				i = 0;
				PRINTTOTAL(secretaccessible, gamestate.secrettotal);
				sz[i++] = 0;
				VWB_DrawPropString(sz);
				fontcolor = 14;
				px = 6;
				py = 100;
				i = 0;
				sz[i++] = 'T';
				sz[i++] = ':';
				sz[i++] = 0;
				VWB_DrawPropString(sz);
				px = 14;
				py = 112;
				i = 0;
				PRINTCOUNT(gamestate.treasurecount);
				sz[i++] = 0;
				VWB_DrawPropString(sz);
				px = 22;
				py = 124;
				i = 0;
				PRINTTOTAL(treasureaccessible, gamestate.treasuretotal);
				sz[i++] = 0;
				VWB_DrawPropString(sz);

				for (y = 0; y < 64; y++)
				{
					for (x = 0; x < 64; x++)
					{
						if ((spotvis[x][y] ^ 0x02) & mask)
							VWB_Bar(x * 4 + 60, y * 3 + 4, 4, 3, ((spotvis[x][y] & 0x70) >> 4) | (spotvis[x][y] & 0x08) & ((spotvis[x][y] & 0x01) << 3));
					}
				}

				VW_UpdateScreen();
				IN_Ack();
				if (Keyboard[sc_Escape])
					IN_ClearKeysDown();		// don't allow Escape to trigger menu
				temp = bufferofs;
				CA_CacheGrChunk (STATUSBARPIC);
				for (i=0;i<3;i++)
				{
					bufferofs = screenloc[i];
					DrawPlayBorder ();
					VWB_DrawPic (0,200-STATUSLINES,STATUSBARPIC);
				}
				bufferofs = temp;
				UNCACHEGRCHUNK (STATUSBARPIC);
				DrawFace ();
				DrawHealth ();
				DrawLives ();
				DrawLevel ();
				DrawAmmo ();
				DrawKeys ();
				DrawWeapon ();
				DrawScore ();
				PM_CheckMainMem();
				if (MousePresent)
					Mouse(MDelta);	// Clear accumulated mouse movement
			}
		}
		else
			tabstate = 0;
	}
#endif // WASD
}


//===========================================================================

/*
#############################################################################

				  The objlist data structure

#############################################################################

objlist containt structures for every actor currently playing.  The structure
is accessed as a linked list starting at *player, ending when ob->next ==
NULL.  GetNewObj inserts a new object at the end of the list, meaning that
if an actor spawn another actor, the new one WILL get to think and react the
same frame.  RemoveObj unlinks the given object and returns it to the free
list, but does not damage the objects ->next pointer, so if the current object
removes itself, a linked list following loop can still safely get to the
next element.

<backwardly linked free list>

#############################################################################
*/


/*
=========================
=
= InitActorList
=
= Call to clear out the actor object lists returning them all to the free
= list.  Allocates a special spot for the player.
=
=========================
*/

int	objcount;

void InitActorList (void)
{
	int	i;

//
// init the actor lists
//
	for (i=0;i<MAXACTORS;i++)
	{
		objlist[i].prev = &objlist[i+1];
		objlist[i].next = NULL;
	}

	objlist[MAXACTORS-1].prev = NULL;

	objfreelist = &objlist[0];
	lastobj = NULL;

	objcount = 0;

//
// give the player the first free spots
//
	GetNewActor ();
	player = new;

}

//===========================================================================

/*
=========================
=
= GetNewActor
=
= Sets the global variable new to point to a free spot in objlist.
= The free spot is inserted at the end of the liked list
=
= When the object list is full, the caller can either have it bomb out ot
= return a dummy object pointer that will never get used
=
=========================
*/

void GetNewActor (void)
{
	if (!objfreelist)
		Quit ("GetNewActor: No free spots in objlist!");

	new = objfreelist;
	objfreelist = new->prev;
	memset (new,0,sizeof(*new));

	if (lastobj)
		lastobj->next = new;
	new->prev = lastobj;	// new->next is allready NULL from memset

	new->active = false;
	lastobj = new;

	objcount++;
}

//===========================================================================

/*
=========================
=
= RemoveObj
=
= Add the given object back into the free list, and unlink it from it's
= neighbors
=
=========================
*/

void RemoveObj (objtype *gone)
{
	objtype **spotat;

	if (gone == player)
		Quit ("RemoveObj: Tried to remove the player!");

	gone->state = NULL;

//
// fix the next object's back link
//
	if (gone == lastobj)
		lastobj = (objtype *)gone->prev;
	else
		gone->next->prev = gone->prev;

//
// fix the previous object's forward link
//
	gone->prev->next = gone->next;

//
// add it back in to the free list
//
	gone->prev = objfreelist;
	objfreelist = gone;

	objcount--;
}

/*
=============================================================================

						MUSIC STUFF

=============================================================================
*/


/*
=================
=
= StopMusic
=
=================
*/

void StopMusic(void)
{
	int	i;

	SD_MusicOff();
	for (i = 0;i < LASTMUSIC;i++)
		if (audiosegs[STARTMUSIC + i])
		{
			MM_SetPurge(&((memptr)audiosegs[STARTMUSIC + i]),3);
			MM_SetLock(&((memptr)audiosegs[STARTMUSIC + i]),false);
		}
}

//==========================================================================


/*
=================
=
= StartMusic
=
=================
*/

void StartMusic(void)
{
	musicnames	chunk;

	SD_MusicOff();
	chunk = songs[gamestate.mapon+gamestate.episode*10];

//	if ((chunk == -1) || (MusicMode != smm_AdLib))
//DEBUG control panel		return;

	MM_BombOnError (false);
	CA_CacheAudioChunk(STARTMUSIC + chunk);
	MM_BombOnError (true);
	if (mmerror)
		mmerror = false;
	else
	{
		MM_SetLock(&((memptr)audiosegs[STARTMUSIC + chunk]),true);
		SD_StartMusic((MusicGroup far *)audiosegs[STARTMUSIC + chunk]);
	}
}


/*
=============================================================================

					PALETTE SHIFTING STUFF

=============================================================================
*/

#define NUMREDSHIFTS	6
#define REDSTEPS		8

#define NUMWHITESHIFTS	3
#define WHITESTEPS		20
#define WHITETICS		6


byte	far redshifts[NUMREDSHIFTS][768];
byte	far whiteshifts[NUMREDSHIFTS][768];

int		damagecount,bonuscount;
boolean	palshifted;

extern 	byte	far	gamepal;

/*
=====================
=
= InitRedShifts
=
=====================
*/

void InitRedShifts (void)
{
	byte	far *workptr, far *baseptr;
	int		i,j,delta;


//
// fade through intermediate frames
//
	for (i=1;i<=NUMREDSHIFTS;i++)
	{
		workptr = (byte far *)&redshifts[i-1][0];
		baseptr = &gamepal;

		for (j=0;j<=255;j++)
		{
			delta = 64-*baseptr;
			*workptr++ = *baseptr++ + delta * i / REDSTEPS;
			delta = -*baseptr;
			*workptr++ = *baseptr++ + delta * i / REDSTEPS;
			delta = -*baseptr;
			*workptr++ = *baseptr++ + delta * i / REDSTEPS;
		}
	}

	for (i=1;i<=NUMWHITESHIFTS;i++)
	{
		workptr = (byte far *)&whiteshifts[i-1][0];
		baseptr = &gamepal;

		for (j=0;j<=255;j++)
		{
			delta = 64-*baseptr;
			*workptr++ = *baseptr++ + delta * i / WHITESTEPS;
			delta = 62-*baseptr;
			*workptr++ = *baseptr++ + delta * i / WHITESTEPS;
			delta = 0-*baseptr;
			*workptr++ = *baseptr++ + delta * i / WHITESTEPS;
		}
	}
}


/*
=====================
=
= ClearPaletteShifts
=
=====================
*/

void ClearPaletteShifts (void)
{
	bonuscount = damagecount = 0;
}


/*
=====================
=
= StartBonusFlash
=
=====================
*/

void StartBonusFlash (void)
{
	bonuscount = NUMWHITESHIFTS*WHITETICS;		// white shift palette
}


/*
=====================
=
= StartDamageFlash
=
=====================
*/

void StartDamageFlash (int damage)
{
	damagecount += damage;
}


/*
=====================
=
= UpdatePaletteShifts
=
=====================
*/

void UpdatePaletteShifts (void)
{
	int	red,white;

	if (bonuscount)
	{
		white = bonuscount/WHITETICS +1;
		if (white>NUMWHITESHIFTS)
			white = NUMWHITESHIFTS;
		bonuscount -= tics;
		if (bonuscount < 0)
			bonuscount = 0;
	}
	else
		white = 0;


	if (damagecount)
	{
		red = damagecount/10 +1;
		if (red>NUMREDSHIFTS)
			red = NUMREDSHIFTS;

		damagecount -= tics;
		if (damagecount < 0)
			damagecount = 0;
	}
	else
		red = 0;

	if (red)
	{
		VW_WaitVBL(1);
		VL_SetPalette (redshifts[red-1]);
		palshifted = true;
	}
	else if (white)
	{
		VW_WaitVBL(1);
		VL_SetPalette (whiteshifts[white-1]);
		palshifted = true;
	}
	else if (palshifted)
	{
		VW_WaitVBL(1);
		VL_SetPalette (&gamepal);		// back to normal
		palshifted = false;
	}
}


/*
=====================
=
= FinishPaletteShifts
=
= Resets palette to normal if needed
=
=====================
*/

void FinishPaletteShifts (void)
{
	if (palshifted)
	{
		palshifted = 0;
		VW_WaitVBL(1);
		VL_SetPalette (&gamepal);
	}
}


/*
=============================================================================

						CORE PLAYLOOP

=============================================================================
*/


/*
=====================
=
= DoActor
=
=====================
*/

void DoActor (objtype *ob)
{
	void (*think)(objtype *);

	if (!ob->active && !areabyplayer[ob->areanumber])
		return;

	if (!(ob->flags&(FL_NONMARK|FL_NEVERMARK)) )
		actorat[ob->tilex][ob->tiley] = NULL;

//
// non transitional object
//

	if (!ob->ticcount)
	{
		think =	ob->state->think;
		if (think)
		{
			think (ob);
			if (!ob->state)
			{
				RemoveObj (ob);
				return;
			}
		}

		if (ob->flags&FL_NEVERMARK)
			return;

		if ( (ob->flags&FL_NONMARK) && actorat[ob->tilex][ob->tiley])
			return;

		actorat[ob->tilex][ob->tiley] = ob;
		return;
	}

//
// transitional object
//
	ob->ticcount-=tics;
	while ( ob->ticcount <= 0)
	{
		think = ob->state->action;			// end of state action
		if (think)
		{
			think (ob);
			if (!ob->state)
			{
				RemoveObj (ob);
				return;
			}
		}

		ob->state = ob->state->next;

		if (!ob->state)
		{
			RemoveObj (ob);
			return;
		}

		if (!ob->state->tictime)
		{
			ob->ticcount = 0;
			goto think;
		}

		ob->ticcount += ob->state->tictime;
	}

think:
	//
	// think
	//
	think =	ob->state->think;
	if (think)
	{
		think (ob);
		if (!ob->state)
		{
			RemoveObj (ob);
			return;
		}
	}

	if (ob->flags&FL_NEVERMARK)
		return;

	if ( (ob->flags&FL_NONMARK) && actorat[ob->tilex][ob->tiley])
		return;

	actorat[ob->tilex][ob->tiley] = ob;
}

//==========================================================================


/*
===================
=
= PlayLoop
=
===================
*/
long funnyticount;


void PlayLoop (void)
{
	int		give;
	int	helmetangle;

	playstate = TimeCount = lasttimecount = 0;
	frameon = 0;
	running = false;
	anglefrac = 0;
	facecount = 0;
	funnyticount = 0;
	memset (buttonstate,0,sizeof(buttonstate));
	ClearPaletteShifts ();

	if (MousePresent)
		Mouse(MDelta);	// Clear accumulated mouse movement

	if (demoplayback)
		IN_StartAck ();

	do
	{
		if (virtualreality)
		{
			helmetangle = peek (0x40,0xf0);
			player->angle += helmetangle;
			if (player->angle >= ANGLES)
				player->angle -= ANGLES;
		}


		PollControls();

//
// actor thinking
//
		madenoise = false;

		MoveDoors ();
		MovePWalls ();

		for (obj = player;obj;obj = obj->next)
			DoActor (obj);

		UpdatePaletteShifts ();

		ThreeDRefresh ();

		//
		// MAKE FUNNY FACE IF BJ DOESN'T MOVE FOR AWHILE
		//
		#ifdef SPEAR
		funnyticount += tics;
		if (funnyticount > 30l*70)
		{
			funnyticount = 0;
			StatusDrawPic (17,4,BJWAITING1PIC+(US_RndT()&1));
			facecount = 0;
		}
		#endif

		gamestate.TimeCount+=tics;

		SD_Poll ();
		UpdateSoundLoc();	// JAB

		if (screenfaded)
			VW_FadeIn ();

		CheckKeys();

//
// debug aids
//
		if (singlestep)
		{
			VW_WaitVBL(14);
			lasttimecount = TimeCount;
		}
		if (extravbls)
			VW_WaitVBL(extravbls);

		if (demoplayback)
		{
			if (IN_CheckAck ())
			{
				IN_ClearKeysDown ();
				playstate = ex_abort;
			}
		}


		if (virtualreality)
		{
			player->angle -= helmetangle;
			if (player->angle < 0)
				player->angle += ANGLES;
		}

	}while (!playstate && !startgame);

	if (playstate != ex_died)
		FinishPaletteShifts ();
}

