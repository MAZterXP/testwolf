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

#ifdef WOLFDOSMPU

int			far	midivolume;
word		far	compflags = 0x8000;
int			far	savedviewsize;

#endif // WOLFDOSMPU

#ifdef WASD

int			far	controlmouse;
int			far	tabstate;

boolean		far	keysalwaysstrafe;
boolean		far	alwaysrun;
boolean		far	mouseturningonly;
byte		far	tabfunction;
byte		far	automapmode;

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
 ULTIMATE_MUS,	// Trans Grosse

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
	// pressing left/right automatically engages alwaysstrafe
	// (doing it this way preserves demo compatibility)
	if (keysalwaysstrafe && (Keyboard[dirscan[di_west]] || Keyboard[dirscan[di_east]]))
		buttonstate[bt_strafe] = true;
#endif // WASD

#ifdef WASD
	if (alwaysrun ? ! buttonstate[bt_run] : buttonstate[bt_run])
#else  // WASD
	if (buttonstate[bt_run])
#endif // WASD
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
	// if alwaysstrafe is engaged, store mouse movement in aux variable so we can still account for it later
	if (keysalwaysstrafe && (Keyboard[dirscan[di_west]] || Keyboard[dirscan[di_east]]))
	{
		// but if recording a demo, filter the mouse movement out (to prevent "cancelling" between keyboard and mouse strafing)
		if (! demorecord && ! (compflags & COMPFLAG_NO_CIRCLE_STRAFE))
			controlmouse += mousexmove*10/(13-mouseadjustment);
	}
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
#ifdef WASD
	else if (alwaysrun ? ! buttonstate[bt_run] : buttonstate[bt_run])
#else  // WASD
	else if (buttonstate[bt_run])
#endif // WASD
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

#define STACKITEM(x, y, z) (((z) << 10) | ((x) << 6) | (y))

void PushTile(byte x, byte y, byte z, controldir_t dir, unsigned far **stackptr)
{
	boolean pw = (pwallstate && x == pwallx && y == pwally);
	unsigned actor;
	byte *visspot = &spotvis[x][y];

	// do not flood through tiles we cannot improve
	if ((*visspot | z) == *visspot)
		return;

	// allow flooding through unseen tiles but remove the unfog bit
	// (this cuts off visibility when a room has multiple entrances)
	if (automapmode < 3 && ! (*visspot & 0x02))
		z &= ~0x10;

	actor = (unsigned) actorat[x][y];
	if (actor == 1 && ! tilemap[x][y])
	{
		// blocking static objects have an actorat value of 1 but do not contain anything in the tilemap
		*visspot |= 0x60 | (z & ~0x08);		// special case: never accessible

		// flood through (but without accessibility bit)
		*(*stackptr)++ = STACKITEM(x, y, z & ~0x08);
	}
	else if (pw || (*(mapsegs[1] + farmapylookup[y] + x) == PUSHABLETILE && tilemap[x][y]))
	{
		// secret (does not flood through)
		// (also note that current flood mode must have accessibility bit to activate a secret)
		if (pw || z & 0x08 && (dir == di_west && ! actorat[x - 1][y]
								|| dir == di_east && ! actorat[x + 1][y]
								|| dir == di_north && ! actorat[x][y - 1]
								|| dir == di_south && ! actorat[x][y + 1]))
		{
			*visspot |= z;				// mark tile as an active secret
		}
		else
			*visspot |= (z & ~0x08);	// mark tile as inaccessible (but try other directions)
	}
	else if (tilemap[x][y] & 0x80)
	{
		// doors
		int doornum = tilemap[x][y] & ~0x80;
		int lock = doorobjlist[doornum].lock;

		if (actor >= (unsigned) objlist && ! (((objtype *) actor)->state->think
											  || ((objtype *) actor)->state->action
											  || ((objtype *) actor)->state->next != ((objtype *) actor)->state)
			|| (! actor || actor >= (unsigned) objlist) && (automapmode == 3 || *visspot & 0x01))
		{
			// door is open and is either occupied by a corpse or currently visible; treat as blank space
			*visspot |= z;

			// flood through (whether accessible or visible)
			*(*stackptr)++ = STACKITEM(x, y, z);
		}
		else
		{
			// mark the door
			if (lock == dr_normal)
				*visspot |= 0x20 | z;
			else
				*visspot |= 0x40 | z;

			// flood through door only if it is unlocked or player has its key
			if (lock < dr_lock1 || lock > dr_lock4 || gamestate.keys & (1 << (lock - dr_lock1)))
			{
				if (*visspot & 0x80 && automapmode < 3)				// if player has not seen through the door...
					*(*stackptr)++ = STACKITEM(x, y, z & ~0x10);	// flood through, but put it in fog
				else
					*(*stackptr)++ = STACKITEM(x, y, z);
			}
		}
	}
	else if (! actor || actor >= (unsigned) objlist)
	{
		// blank spaces or actors
		*visspot |= z;

		// flood through (whether accessible or visible)
		*(*stackptr)++ = STACKITEM(x, y, z);
	}
	else if (tilemap[x][y] == ELEVATORTILE)
	{
		// elevator (does not flood through)
		*visspot |= 0x48 | z;	// special case: always accessible
	}
	else
	{
		// wall (does not flood through)
		*visspot |= 0x68 | z;	// special case: always accessible
	}
}

void CheckAccessible()
{
	objtype *obj;
	statobj_t *statptr;
	unsigned far *stackptr;
	memptr tempmem = MM_GetBuffer();
	unsigned current;
	byte x, y, z;
	unsigned killstationary = gamestate.killcount;

	stackptr = (unsigned far *) tempmem;

	killaccessible = gamestate.killcount;
	secretaccessible = gamestate.secretcount;
	treasureaccessible = gamestate.treasurecount;

	x = player->tilex;
	y = player->tiley;
	spotvis[x][y] = (spotvis[x][y] & 0x80) | 0x1f;
	*stackptr++ = STACKITEM(x, y, 0x1c);
	while (stackptr != (unsigned far *) tempmem)
	{
		current = *--stackptr;
		x = (current >> 6) & 0x3f;
		y = current & 0x3f;
		z = (current >> 10) & 0x3c;

		if (x > 0)	PushTile(x - 1, y, z, di_west, &stackptr);
		if (x < 63)	PushTile(x + 1, y, z, di_east, &stackptr);
		if (y > 0)	PushTile(x, y - 1, z, di_north, &stackptr);
		if (y < 63)	PushTile(x, y + 1, z, di_south, &stackptr);

		// tempmem should never get exhausted, but if it ever does, it's a sign of something worse
		if (stackptr >= (unsigned far *) tempmem + BUFFERSIZE / sizeof(unsigned))
		{
			char sz[2];
			sz[0] = ' ';
			sz[1] = 0;
			Quit(sz);
		}
	}

	// count treasure
	for (statptr = &statobjlist[0]; statptr != laststatobj; statptr++)
	{
		if (statptr->shapenum != -1 && statptr->flags & FL_BONUS)
		{
			byte n = statptr->itemnumber;
			byte w = tilemap[statptr->tilex][statptr->tiley];

			// a secret is disabled if there is a "bonus" barrel inside
			boolean secret = (*(mapsegs[1] + farmapylookup[statptr->tiley] + statptr->tilex) == PUSHABLETILE && w);
			if (! (compflags & COMPFLAG_REACTIVATING_PUSHWALLS) && n == block && secret)
				*statptr->visspot |= 0x68;	// turn into regular wall

			// if the tile is occupied by a wall, bonus is inaccessible
			if (w && ! (w & 0x80) && n != block)  // ignore "bonus" barrels
				*statptr->visspot &= ~0x08;

			if (n == bo_cross || n == bo_chalice || n == bo_bible || n == bo_crown || n == bo_fullheal)
			{
				// if it can be physically reached, treasure is accessible
				if (*statptr->visspot & 0x08)
					treasureaccessible++;
			}
		}
	}

	// count accessible and stationary kills
	for (obj = player->next; obj; obj = obj->next)
	{
		extern void T_Stand (objtype *ob);
		byte *visspot = &spotvis[obj->x >> TILESHIFT][obj->y >> TILESHIFT];

		if (*visspot & 0x04 && obj->flags & FL_SHOOTABLE)
		{
			// if it is visible, it can be shot, ergo, enemy is accessible
			killaccessible++;

			// if the enemy is just standing on a spot that the player has already seen, note it so they can be marked on the map if they are all that remain
			// (the player should have seen all accessible stationary enemy spots first)
			if ((*visspot & 0x12) == 0x12 && obj->state->think == T_Stand)
				killstationary++;
		}
	}

	// count secrets and repurpose unfog bit as the blue bit
	for (y = 0; y < 64; y++)
	{
		for (x = 0; x < 64; x++)
		{
			byte *visspot = &spotvis[x][y];
			boolean secret = (*(mapsegs[1] + farmapylookup[y] + x) == PUSHABLETILE && tilemap[x][y]);
			boolean pw = (pwallstate && x == pwallx && y == pwally);

			// check if secret is disabled due to bonus barrel
			if (secret && (*visspot & 0x68) == 0x68)
				secret = false;

			// if tile has been marked as an active secret, secret is accessible
			if (! pw && *visspot & 0x08 && secret)
				secretaccessible++;

			// remove the accessibility and visibility bits of anything in fog, for display purposes
			if (! (*visspot & 0x10))
				*visspot &= ~0x0c;

			// if tile is marked as a door, special door, or wall/block, their blue bit is already set (which is what we want);
			// but if tile is marked as a blank space or a secret...
			if (! (*visspot & 0x60))
			{
				// ... tile needs to have the blue bit cleared
				*visspot &= ~0x10;

				// and if it is a secret...
				if (pw || secret)
				{
					// if tile is a currently-moving secret,
					// or in always hints mode,
					// or in clear map mode and no accessible enemies are left,
					// or if tile is a previously-discovered secret
					if (pw || automapmode >= 2 || automapmode == 1 && killaccessible == gamestate.killcount || *visspot & 0x80)
					{
						// show secret in map if player can possibly see it (player visibility history will cull it later)
						if (automapmode == 3 || *visspot & 0x0c)
							*visspot = (*visspot & 0x8f) | 0x24;
					}
					else
					{
						// the secret is not discovered yet;
						// if player can possibly see it, masquerade it as a wall
						if (*visspot & 0x0c)
							*visspot |= 0x7c;
					}
				}
			}
		}
	}

	// mark treasure and bonuses
	for (statptr = &statobjlist[0]; statptr != laststatobj; statptr++)
	{
		if (statptr->shapenum != -1 && statptr->flags & FL_BONUS)
		{
			byte n = statptr->itemnumber;

			// show item in map if player can possibly see it (player visibility history will cull it later)
			if (automapmode == 3 || *statptr->visspot & 0x0c)
			{
				if (n == bo_cross || n == bo_chalice || n == bo_bible || n == bo_crown || n == bo_fullheal || n == bo_key1 || n == bo_key2 || n == bo_spear)
					*statptr->visspot = (*statptr->visspot & 0x8f) | 0x64;	// mark the treasure (including key items)
				else if (n != block)
					*statptr->visspot = (*statptr->visspot & 0x8f) | 0x14;	// mark the bonus (except for barrels that mark pushwall spots that can't be reused)
			}
		}
	}

	// mark enemies
	for (obj = player->next; obj; obj = obj->next)
	{
		// anything that is animating is marked on the map as an enemy
		// (this includes special cases like ghosts and projectiles)
		if (obj->state->think || obj->state->action || obj->state->next != obj->state)
		{
			byte *visspot = &spotvis[obj->x >> TILESHIFT][obj->y >> TILESHIFT];

			// if enemy is within player's viscone, or in full map mode,
			// or in clear map mode and only standing enemies are left (but don't give away ghosts!)
			if ((*visspot & 0x01
				 || automapmode == 3
				 || *visspot & 0x0c && automapmode >= 1 && killstationary == killaccessible && obj->obclass != ghostobj))
			{
				// special case: enemy color is always dark to distinguish from player, and is drawn over everything else except player
				*visspot = (*visspot & 0x83) | 0x44;
			}
		}
	}

	// mark the player
	spotvis[player->tilex][player->tiley] = (spotvis[player->tilex][player->tiley] & 0x80) | 0x4f;
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
		CA_CacheGrChunk(STARTFONT+1);
		ClearSplitVWB ();
		VW_ScreenToScreen (displayofs,bufferofs,80,160);
#else  // WOLFDOSMPU
		WindowH = 160;
#endif // WOLFDOSMPU
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
		UNCACHEGRCHUNK(STARTFONT+1);
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
#ifdef WOLFDOSMPU
		CA_CacheGrChunk(STARTFONT+1);
#endif // WOLFDOSMPU
		US_ControlPanel(scan);
#ifdef WOLFDOSMPU
		UNCACHEGRCHUNK(STARTFONT+1);
#endif // WOLFDOSMPU

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
			extern byte far palette1[256][3];
			extern byte far palette2[256][3];
			char sz[16];
			int i;
			byte x, y;
			unsigned width, height;

			tabstate = 2;	// do not allow next tab press to show KST stats again

			CheckAccessible();

#define PRINTCOUNT(c) {						\
	if ((c) >= 100)							\
		sz[i++] = '0' + ((c) / 100) % 10;	\
	if ((c) >= 10)							\
		sz[i++] = '0' + ((c) / 10) % 10;	\
	sz[i++] = '0' + ((c) % 10);				\
}

			memset(sz, ' ', sizeof(sz));

			if (tabfunction == 1)
			{
				VW_ScreenToScreen(displayofs,bufferofs,80,160);

#define DW 40
#define DH 36
				// create dialog window
				VWB_Bar(160 - DW, 80 - DH, DW * 2, DH * 2, 127);

				VWB_Hlin(160 - DW, 160 + DW - 1, 80 - DH - 1, 125);
				VWB_Hlin(160 - DW - 1, 160 + DW, 80 + DH, 0);
				VWB_Vlin(80 - DH - 1, 80 + DH - 1, 160 - DW - 1, 125);
				VWB_Vlin(80 - DH - 1, 80 + DH - 1, 160 + DW, 0);
			}
			else
			{
				// clear screen except for floor display
				VWB_Bar(0, 0, 320, 163, 127);
				VWB_Vlin(163, 197, 42, 126);
				VWB_Bar(43, 163, 277, 35, 127);
			}

#if 0
			// layout testing code
			{
				static int count = 0;
				count = (count + 1) % 10;
				gamestate.treasurecount = 100 + count * 10 + count;
				treasureaccessible = gamestate.treasurecount;
				gamestate.treasuretotal = treasureaccessible + 1;
			}
#endif

			fontnumber = 0;
			fontcolor = 0;
			for (x = 1; x <= 1; x--)
			{
				i = 0;
				sz[i++] = 'K';
				sz[i++] = ':';
				sz[i++] = 0;
				if (x == 0)
					fontcolor = 16;
				if (tabfunction == 1)
				{
					px = x + 128;
					py = x + 49;
				}
				else
				{
					px = x + 6;
					py = x + 4;
				}
				VWB_DrawPropString(sz);
				i = 0;
				PRINTCOUNT(gamestate.killcount);
				sz[i++] = 0;
				if (x == 0)
					fontcolor = 12;
				VW_MeasurePropString(sz, &width, &height);
				px = x + (tabfunction == 1 ? 165 : 27) - width;
				py += (tabfunction == 1 ? 0 : 12);
				VWB_DrawPropString(sz);
				i = 0;
				sz[i++] = '/';
				sz[i++] = 0;
				if (x == 0)
					fontcolor -= 8;
				px -= 2;
				py += 6;
				VWB_DrawPropString(sz);
				i = 0;
				PRINTCOUNT(killaccessible);
				if (killaccessible < gamestate.killtotal)
					sz[i++] = '^';
				sz[i++] = 0;
				px -= 2;
				py += 5;
				VWB_DrawPropString(sz);

				i = 0;
				sz[i++] = 'S';
				sz[i++] = ':';
				sz[i++] = 0;
				if (x == 0)
					fontcolor = 16;
				if (tabfunction == 1)
				{
					px = x + 128;
					py += 10;
				}
				else
				{
					px = x + 6;
					py += 24;
				}
				VWB_DrawPropString(sz);
				i = 0;
				PRINTCOUNT(gamestate.secretcount);
				sz[i++] = 0;
				if (x == 0)
					fontcolor = 10;
				VW_MeasurePropString(sz, &width, &height);
				px = x + (tabfunction == 1 ? 165 : 27) - width;
				py += (tabfunction == 1 ? 0 : 12);
				VWB_DrawPropString(sz);
				i = 0;
				sz[i++] = '/';
				sz[i++] = 0;
				if (x == 0)
					fontcolor -= 8;
				px -= 2;
				py += 6;
				VWB_DrawPropString(sz);
				i = 0;
				PRINTCOUNT(secretaccessible);
				if (secretaccessible < gamestate.secrettotal)
					sz[i++] = '^';
				sz[i++] = 0;
				px -= 2;
				py += 5;
				VWB_DrawPropString(sz);

				i = 0;
				sz[i++] = 'T';
				sz[i++] = ':';
				sz[i++] = 0;
				if (x == 0)
					fontcolor = 16;
				if (tabfunction == 1)
				{
					px = x + 128;
					py += 10;
				}
				else
				{
					px = x + 6;
					py += 24;
				}
				VWB_DrawPropString(sz);
				i = 0;
				PRINTCOUNT(gamestate.treasurecount);
				sz[i++] = 0;
				if (x == 0)
					fontcolor = 14;
				VW_MeasurePropString(sz, &width, &height);
				px = x + (tabfunction == 1 ? 165 : 27) - width;
				py += (tabfunction == 1 ? 0 : 12);
				VWB_DrawPropString(sz);
				i = 0;
				sz[i++] = '/';
				sz[i++] = 0;
				if (x == 0)
					fontcolor -= 8;
				px -= 2;
				py += 6;
				VWB_DrawPropString(sz);
				i = 0;
				PRINTCOUNT(treasureaccessible);
				if (treasureaccessible < gamestate.treasuretotal)
					sz[i++] = '^';
				sz[i++] = 0;
				px -= 2;
				py += 5;
				VWB_DrawPropString(sz);
			}

			if (tabfunction == 2)
			{
				// draw the map itself
				for (y = 0; y < 64; y++)
				{
					for (x = 0; x < 64; x++)
					{
						byte *visspot = &spotvis[x][y];
						if (*visspot & 0x0c)
						{
							byte color = (*visspot & 0x70);
							byte glow;
							if (! color)
							{
								// blank tiles should always be glowing if within automap viscone (so automap can properly show "see through" pillar-blocked areas)
								glow = (*visspot & 0x01) << 3;
							}
							else if (color == 0x20)
							{
								// secrets should always be glowing if they are active
								glow = (*visspot & 0x08);
							}
							else if ((color == 0x30 || color == 0x50) && tilemap[x][y] & 0x80)	// exclude wall elevator tiles
							{
								// doors should always be glowing if they have never been seen opened
								glow = (*visspot & 0x80) >> 4;
							}
							else
							{
								// make player's viscone glow (except for special cases earlier marked by CheckAccessible
								glow = (*visspot & 0x08) & ((*visspot & 0x01) << 3);
							}
							VWB_Bar(x * 4 + 60, y * 3 + 4, 4, 3, (color >> 4) | glow);
						}
					}
				}
			}

			// temporarily move to the new screen
			asm	cli
			asm	mov	cx,[bufferofs]
			asm	mov	dx,3d4h		// CRTC address register
			asm	mov	al,0ch		// start address high register
			asm	out	dx,al
			asm	inc	dx
			asm	mov	al,ch
			asm	out	dx,al   	// set the high byte
			asm	sti

			if (tabfunction == 1)
			{
				// should not do palette-cycling on stats mode because some graphics depend on the colors we touch
				IN_Ack();
			}
			else
			{
				// palette-cycling
				VL_GetPalette(&palette2[0][0]);
				_fmemcpy(palette1,palette2,768);
				IN_StartAck();
				x = 0;
				while (! IN_CheckAck())
				{
#define PULSERATE 35
					int factor;
					x = (x + 1) % (PULSERATE * 2);
					if (x < PULSERATE)
						factor = (PULSERATE - x) * 255 / PULSERATE;
					else
						factor = (x - PULSERATE) * 255 / PULSERATE;

					for (y = 0; y < 8; y++)
					{
						palette1[8 + y][0] = ((255 - factor) * (int) palette2[y][0] + factor * (int) palette2[8 + y][0]) / 255;
						palette1[8 + y][1] = ((255 - factor) * (int) palette2[y][1] + factor * (int) palette2[8 + y][1]) / 255;
						palette1[8 + y][2] = ((255 - factor) * (int) palette2[y][2] + factor * (int) palette2[8 + y][2]) / 255;
					}
					VL_WaitVBL(1);
					VL_SetPalette(&palette1[0][0]);
				}
				VL_WaitVBL(1);
				VL_SetPalette(&palette2[0][0]);
			}

			if (Keyboard[sc_Escape])
				IN_ClearKeysDown();		// don't allow Escape to trigger menu

			// reshow the old screen
			asm	cli
			asm	mov	cx,[displayofs]
			asm	mov	dx,3d4h		// CRTC address register
			asm	mov	al,0ch		// start address high register
			asm	out	dx,al
			asm	inc	dx
			asm	mov	al,ch
			asm	out	dx,al   	// set the high byte
			asm	sti

			// copy the old screen over the new screen (overwriting the map)
			VL_WaitVBL(1);
			VW_ScreenToScreen(displayofs,bufferofs,80,200);

			if (MousePresent)
				Mouse(MDelta);	// Clear accumulated mouse movement
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

