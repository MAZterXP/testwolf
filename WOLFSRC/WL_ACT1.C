// WL_ACT1.C

#include "WL_DEF.H"
#pragma hdrstop

/*
=============================================================================

							STATICS

=============================================================================
*/


statobj_t	statobjlist[MAXSTATS],*laststatobj;


struct
{
	int		picnum;
	stat_t	type;
} statinfo[] =
{
{SPR_STAT_0},					// puddle          spr1v
{SPR_STAT_1,block},				// Green Barrel    "
{SPR_STAT_2,block},				// Table/chairs    "
{SPR_STAT_3,block},				// Floor lamp      "
{SPR_STAT_4},					// Chandelier      "
{SPR_STAT_5,block},				// Hanged man      "
{SPR_STAT_6,bo_alpo},			// Bad food        "
{SPR_STAT_7,block},				// Red pillar      "
//
// NEW PAGE
//
{SPR_STAT_8,block},				// Tree            spr2v
{SPR_STAT_9},					// Skeleton flat   "
{SPR_STAT_10,block},			// Sink            " (SOD:gibs)
{SPR_STAT_11,block},			// Potted plant    "
{SPR_STAT_12,block},			// Urn             "
{SPR_STAT_13,block},			// Bare table      "
{SPR_STAT_14},					// Ceiling light   "
#ifndef SPEAR
{SPR_STAT_15},					// Kitchen stuff   "
#else
{SPR_STAT_15,block},			// Gibs!
#endif
//
// NEW PAGE
//
{SPR_STAT_16,block},			// suit of armor   spr3v
{SPR_STAT_17,block},			// Hanging cage    "
{SPR_STAT_18,block},			// SkeletoninCage  "
{SPR_STAT_19},					// Skeleton relax  "
{SPR_STAT_20,bo_key1},			// Key 1           "
{SPR_STAT_21,bo_key2},			// Key 2           "
{SPR_STAT_22,block},			// stuff				(SOD:gibs)
{SPR_STAT_23},					// stuff
//
// NEW PAGE
//
{SPR_STAT_24,bo_food}, 			// Good food       spr4v
{SPR_STAT_25,bo_firstaid},		// First aid       "
{SPR_STAT_26,bo_clip},			// Clip            "
{SPR_STAT_27,bo_machinegun},	// Machine gun     "
{SPR_STAT_28,bo_chaingun},		// Gatling gun     "
{SPR_STAT_29,bo_cross},			// Cross           "
{SPR_STAT_30,bo_chalice},		// Chalice         "
{SPR_STAT_31,bo_bible},			// Bible           "
//
// NEW PAGE
//
{SPR_STAT_32,bo_crown},			// crown           spr5v
{SPR_STAT_33,bo_fullheal},		// one up          "
{SPR_STAT_34,bo_gibs},			// gibs            "
{SPR_STAT_35,block},			// barrel          "
{SPR_STAT_36,block},			// well            "
{SPR_STAT_37,block},			// Empty well      "
{SPR_STAT_38,bo_gibs},			// Gibs 2          "
{SPR_STAT_39,block},			// flag				"
//
// NEW PAGE
//
#ifndef SPEAR
{SPR_STAT_40,block},			// Call Apogee		spr7v
#else
{SPR_STAT_40},					// Red light
#endif
//
// NEW PAGE
//
{SPR_STAT_41},					// junk            "
{SPR_STAT_42},					// junk 		   "
{SPR_STAT_43},					// junk            "
#ifndef SPEAR
{SPR_STAT_44},					// pots            "
#else
{SPR_STAT_44,block},			// Gibs!
#endif
{SPR_STAT_45,block},			// stove           " (SOD:gibs)
{SPR_STAT_46,block},			// spears          " (SOD:gibs)
{SPR_STAT_47},					// vines			"
//
// NEW PAGE
//
#ifdef SPEAR
{SPR_STAT_48,block},			// marble pillar
{SPR_STAT_49,bo_25clip},		// bonus 25 clip
{SPR_STAT_50,block},			// truck
{SPR_STAT_51,bo_spear},			// SPEAR OF DESTINY!
#endif

{SPR_STAT_26,bo_clip2},			// Clip            "
{-1}							// terminator
};

/*
===============
=
= InitStaticList
=
===============
*/

void InitStaticList (void)
{
	laststatobj = &statobjlist[0];
}



/*
===============
=
= SpawnStatic
=
===============
*/

void SpawnStatic (int tilex, int tiley, int type)
{
	laststatobj->shapenum = statinfo[type].picnum;
	laststatobj->tilex = tilex;
	laststatobj->tiley = tiley;
	laststatobj->visspot = &spotvis[tilex][tiley];

	switch (statinfo[type].type)
	{
	case block:
		(unsigned)actorat[tilex][tiley] = 1;		// consider it a blocking tile
	case dressing:
		laststatobj->flags = 0;
		break;

	case	bo_cross:
	case	bo_chalice:
	case	bo_bible:
	case	bo_crown:
	case	bo_fullheal:
		if (!loadedgame)
		  gamestate.treasuretotal++;

	case	bo_firstaid:
	case	bo_key1:
	case	bo_key2:
	case	bo_key3:
	case	bo_key4:
	case	bo_clip:
	case	bo_25clip:
	case	bo_machinegun:
	case	bo_chaingun:
	case	bo_food:
	case	bo_alpo:
	case	bo_gibs:
	case	bo_spear:
		laststatobj->flags = FL_BONUS;
		laststatobj->itemnumber = statinfo[type].type;
		break;
	}

	laststatobj++;

	if (laststatobj == &statobjlist[MAXSTATS])
		Quit ("Too many static objects!\n");
}


/*
===============
=
= PlaceItemType
=
= Called during game play to drop actors' items.  It finds the proper
= item number based on the item type (bo_???).  If there are no free item
= spots, nothing is done.
=
===============
*/

void PlaceItemType (int itemtype, int tilex, int tiley)
{
	int			type;
	statobj_t	*spot;

//
// find the item number
//
	for (type=0 ;  ; type++)
	{
		if (statinfo[type].picnum == -1)		// end of list
			Quit ("PlaceItemType: couldn't find type!");
		if (statinfo[type].type == itemtype)
			break;
	}

//
// find a spot in statobjlist to put it in
//
	for (spot=&statobjlist[0] ; ; spot++)
	{
		if (spot==laststatobj)
		{
			if (spot == &statobjlist[MAXSTATS])
				return;							// no free spots
			laststatobj++;						// space at end
			break;
		}

		if (spot->shapenum == -1)				// -1 is a free spot
			break;
	}
//
// place it
//
	spot->shapenum = statinfo[type].picnum;
	spot->tilex = tilex;
	spot->tiley = tiley;
	spot->visspot = &spotvis[tilex][tiley];
	spot->flags = FL_BONUS;
	spot->itemnumber = statinfo[type].type;
}



/*
=============================================================================

							DOORS

doorobjlist[] holds most of the information for the doors

doorposition[] holds the amount the door is open, ranging from 0 to 0xffff
	this is directly accessed by AsmRefresh during rendering

The number of doors is limited to 64 because a spot in tilemap holds the
	door number in the low 6 bits, with the high bit meaning a door center
	and bit 6 meaning a door side tile

Open doors conect two areas, so sounds will travel between them and sight
	will be checked when the player is in a connected area.

Areaconnect is incremented/decremented by each door. If >0 they connect

Every time a door opens or closes the areabyplayer matrix gets recalculated.
	An area is true if it connects with the player's current spor.

=============================================================================
*/

#define DOORWIDTH	0x7800
#define OPENTICS	300

doorobj_t	doorobjlist[MAXDOORS],*lastdoorobj;
int			doornum;

unsigned	doorposition[MAXDOORS];		// leading edge of door 0=closed
										// 0xffff = fully open

byte		far areaconnect[NUMAREAS][NUMAREAS];

boolean		areabyplayer[NUMAREAS];


/*
==============
=
= ConnectAreas
=
= Scans outward from playerarea, marking all connected areas
=
==============
*/

void RecursiveConnect (int areanumber)
{
	int	i;

	for (i=0;i<NUMAREAS;i++)
	{
		if (areaconnect[areanumber][i] && !areabyplayer[i])
		{
			areabyplayer[i] = true;
			RecursiveConnect (i);
		}
	}
}


void ConnectAreas (void)
{
	memset (areabyplayer,0,sizeof(areabyplayer));
#ifdef WOLFDOSMPU
	// don't cause memory corruption (but keep compatibility with existing holowall behavior)
	// (yes, we are ignoring the illegal memory accesses, but, hey, at least there's no corruption)
	if (player->areanumber < NUMAREAS)
#endif // WOLFDOSMPU
	areabyplayer[player->areanumber] = true;
	RecursiveConnect (player->areanumber);
}


void InitAreas (void)
{
	memset (areabyplayer,0,sizeof(areabyplayer));
#ifdef WOLFDOSMPU
	// don't cause memory corruption (but keep compatibility with existing holowall behavior)
	// (yes, we are ignoring the illegal memory accesses, but, hey, at least there's no corruption)
	if (player->areanumber < NUMAREAS)
#endif // WOLFDOSMPU
	areabyplayer[player->areanumber] = true;
}



/*
===============
=
= InitDoorList
=
===============
*/

void InitDoorList (void)
{
	memset (areabyplayer,0,sizeof(areabyplayer));
	_fmemset (areaconnect,0,sizeof(areaconnect));

	lastdoorobj = &doorobjlist[0];
	doornum = 0;
}


/*
===============
=
= SpawnDoor
=
===============
*/

void SpawnDoor (int tilex, int tiley, boolean vertical, int lock)
{
	int	areanumber;
	unsigned	far *map;

	if (doornum==64)
		Quit ("64+ doors on level!");

	doorposition[doornum] = 0;		// doors start out fully closed
	lastdoorobj->tilex = tilex;
	lastdoorobj->tiley = tiley;
	lastdoorobj->vertical = vertical;
	lastdoorobj->lock = lock;
	lastdoorobj->action = dr_closed;

	(unsigned)actorat[tilex][tiley] = doornum | 0x80;	// consider it a solid wall

//
// make the door tile a special tile, and mark the adjacent tiles
// for door sides
//
	tilemap[tilex][tiley] = doornum | 0x80;
	map = mapsegs[0] + farmapylookup[tiley]+tilex;
	if (vertical)
	{
		*map = *(map-1);                        // set area number
		tilemap[tilex][tiley-1] |= 0x40;
		tilemap[tilex][tiley+1] |= 0x40;
	}
	else
	{
		*map = *(map-mapwidth);					// set area number
		tilemap[tilex-1][tiley] |= 0x40;
		tilemap[tilex+1][tiley] |= 0x40;
	}

	doornum++;
	lastdoorobj++;
}

//===========================================================================

/*
=====================
=
= OpenDoor
=
=====================
*/

void OpenDoor (int door)
{
#ifdef WOLFDOSMPU
	// play the door sound again when it's being reopened
	if (doorobjlist[door].action == dr_closing)
	{
		int area = *(mapsegs[0] + farmapylookup[doorobjlist[door].tiley]
					+doorobjlist[door].tilex)-AREATILE;
		if (areabyplayer[area])
			PlaySoundLocTile(OPENDOORSND, doorobjlist[door].tilex, doorobjlist[door].tiley);
	}
#endif // WOLFDOSMPU

	if (doorobjlist[door].action == dr_open)
		doorobjlist[door].ticcount = 0;			// reset open time
	else
		doorobjlist[door].action = dr_opening;	// start it opening
}


/*
=====================
=
= CloseDoor
=
=====================
*/

#ifdef WOLFDOSMPU
void MoveCorpseIntoDoor(int doorx, int doory, objtype *actor)
{
	// if a door is being blocked by a corpse, move the corpse in the door
	// without actually changing its rendering position
	// (note that this also fixes a bug where other enemies could move
	// near the door and trigger its CloseDoor because they override the
	// corpse in the actorat table, frustrating players who intentionally
	// placed the corpse there to keep the door open)
	if (Alive(actor))
		return;
	if (actorat[actor->tilex][actor->tiley] == actor)
		actorat[actor->tilex][actor->tiley] = NULL;
	actor->tilex = doorx;
	actor->tiley = doory;
	actorat[doorx][doory] = actor;
}
#endif // WOLFDOSMPU

void CloseDoor (int door)
{
	int	tilex,tiley,area;
	objtype *check;

//
// don't close on anything solid
//
	tilex = doorobjlist[door].tilex;
	tiley = doorobjlist[door].tiley;

	if (actorat[tilex][tiley])
		return;

	if (player->tilex == tilex && player->tiley == tiley)
		return;

	if (doorobjlist[door].vertical)
	{
		if ( player->tiley == tiley )
		{
			if ( ((player->x+MINDIST) >>TILESHIFT) == tilex )
				return;
			if ( ((player->x-MINDIST) >>TILESHIFT) == tilex )
				return;
		}
		check = actorat[tilex-1][tiley];
		if (check && ((check->x+MINDIST) >> TILESHIFT) == tilex )
#ifdef WOLFDOSMPU
		{
			MoveCorpseIntoDoor(tilex, tiley, check);
#endif // WOLFDOSMPU
			return;
#ifdef WOLFDOSMPU
		}
#endif // WOLFDOSMPU
		check = actorat[tilex+1][tiley];
		if (check && ((check->x-MINDIST) >> TILESHIFT) == tilex )
#ifdef WOLFDOSMPU
		{
			MoveCorpseIntoDoor(tilex, tiley, check);
#endif // WOLFDOSMPU
			return;
#ifdef WOLFDOSMPU
		}
#endif // WOLFDOSMPU
	}
	else if (!doorobjlist[door].vertical)
	{
		if (player->tilex == tilex)
		{
			if ( ((player->y+MINDIST) >>TILESHIFT) == tiley )
				return;
			if ( ((player->y-MINDIST) >>TILESHIFT) == tiley )
				return;
		}
		check = actorat[tilex][tiley-1];
		if (check && ((check->y+MINDIST) >> TILESHIFT) == tiley )
#ifdef WOLFDOSMPU
		{
			MoveCorpseIntoDoor(tilex, tiley, check);
#endif // WOLFDOSMPU
			return;
#ifdef WOLFDOSMPU
		}
#endif // WOLFDOSMPU
		check = actorat[tilex][tiley+1];
		if (check && ((check->y-MINDIST) >> TILESHIFT) == tiley )
#ifdef WOLFDOSMPU
		{
			MoveCorpseIntoDoor(tilex, tiley, check);
#endif // WOLFDOSMPU
			return;
#ifdef WOLFDOSMPU
		}
#endif // WOLFDOSMPU
	}


//
// play door sound if in a connected area
//
	area = *(mapsegs[0] + farmapylookup[doorobjlist[door].tiley]
			+doorobjlist[door].tilex)-AREATILE;
	if (areabyplayer[area])
	{
		PlaySoundLocTile(CLOSEDOORSND,doorobjlist[door].tilex,doorobjlist[door].tiley);	// JAB
	}

	doorobjlist[door].action = dr_closing;
//
// make the door space solid
//
	(unsigned)actorat[tilex][tiley]
		= door | 0x80;
}



/*
=====================
=
= OperateDoor
=
= The player wants to change the door's direction
=
=====================
*/

void OperateDoor (int door)
{
	int	lock;

	lock = doorobjlist[door].lock;
	if (lock >= dr_lock1 && lock <= dr_lock4)
	{
		if ( ! (gamestate.keys & (1 << (lock-dr_lock1) ) ) )
		{
			SD_PlaySound (NOWAYSND);		// locked
			return;
		}
	}

	switch (doorobjlist[door].action)
	{
	case dr_closed:
	case dr_closing:
		OpenDoor (door);
		break;
	case dr_open:
	case dr_opening:
		CloseDoor (door);
		break;
	}
}


//===========================================================================

/*
===============
=
= DoorOpen
=
= Close the door after three seconds
=
===============
*/

void DoorOpen (int door)
{
	if ( (doorobjlist[door].ticcount += tics) >= OPENTICS)
		CloseDoor (door);
}



/*
===============
=
= DoorOpening
=
===============
*/

void DoorOpening (int door)
{
	int		area1,area2;
	unsigned	far	*map;
	long	position;

	position = doorposition[door];
	if (!position)
	{
	//
	// door is just starting to open, so connect the areas
	//
		map = mapsegs[0] + farmapylookup[doorobjlist[door].tiley]
			+doorobjlist[door].tilex;

		if (doorobjlist[door].vertical)
		{
			area1 =	*(map+1);
			area2 =	*(map-1);
		}
		else
		{
			area1 =	*(map-mapwidth);
			area2 =	*(map+mapwidth);
		}
		area1 -= AREATILE;
		area2 -= AREATILE;
		areaconnect[area1][area2]++;
		areaconnect[area2][area1]++;
		ConnectAreas ();
		if (areabyplayer[area1])
		{
			PlaySoundLocTile(OPENDOORSND,doorobjlist[door].tilex,doorobjlist[door].tiley);	// JAB
		}
	}

//
// slide the door by an adaptive amount
//
	position += tics<<10;
	if (position >= 0xffff)
	{
	//
	// door is all the way open
	//
		position = 0xffff;
		doorobjlist[door].ticcount = 0;
		doorobjlist[door].action = dr_open;
		actorat[doorobjlist[door].tilex][doorobjlist[door].tiley] = 0;
	}

	doorposition[door] = position;
}


/*
===============
=
= DoorClosing
=
===============
*/

void DoorClosing (int door)
{
	int		area1,area2,move;
	unsigned	far	*map;
	long	position;
	int		tilex,tiley;

	tilex = doorobjlist[door].tilex;
	tiley = doorobjlist[door].tiley;

	if ( ((unsigned)actorat[tilex][tiley] != (door | 0x80))
	|| (player->tilex == tilex && player->tiley == tiley) )
	{			// something got inside the door
		OpenDoor (door);
		return;
	};

	position = doorposition[door];

//
// slide the door by an adaptive amount
//
	position -= tics<<10;
	if (position <= 0)
	{
	//
	// door is closed all the way, so disconnect the areas
	//
		position = 0;

		doorobjlist[door].action = dr_closed;

		map = mapsegs[0] + farmapylookup[doorobjlist[door].tiley]
			+doorobjlist[door].tilex;

		if (doorobjlist[door].vertical)
		{
			area1 =	*(map+1);
			area2 =	*(map-1);
		}
		else
		{
			area1 =	*(map-mapwidth);
			area2 =	*(map+mapwidth);
		}
		area1 -= AREATILE;
		area2 -= AREATILE;
		areaconnect[area1][area2]--;
		areaconnect[area2][area1]--;

		ConnectAreas ();
	}

	doorposition[door] = position;
}




/*
=====================
=
= MoveDoors
=
= Called from PlayLoop
=
=====================
*/

void MoveDoors (void)
{
	int		door;

	if (gamestate.victoryflag)		// don't move door during victory sequence
		return;

	for (door = 0 ; door < doornum ; door++)
		switch (doorobjlist[door].action)
		{
		case dr_open:
			DoorOpen (door);
			break;

		case dr_opening:
			DoorOpening(door);
			break;

		case dr_closing:
			DoorClosing(door);
			break;

#ifdef WOLFDOSMPU
		case dr_closed:
			if (! (compflags & COMPFLAG_PHANTOM_DOORS))
			{
				// if any actor claims the door slot even if it is closed, all other actors can pass through
				// the closed door until it opens; some mods may take advantage of this, but the intended
				// logic is for the door to be opened on level start (and should close naturally after a while)
				if ((unsigned) actorat[doorobjlist[door].tilex][doorobjlist[door].tiley] != (door | 0x80))
				{
					OpenDoor(door);
					DoorOpening(door);		// execute the state once to connect areas
					doorposition[door] = 0xffff;	// fully open it by the next tic
				}
			}
			break;
#endif // WOLFDOSMPU
		}
}


/*
=============================================================================

						PUSHABLE WALLS

=============================================================================
*/

unsigned	pwallstate;
unsigned	pwallpos;			// amount a pushable wall has been moved (0-63)
unsigned	pwallx,pwally;
int			pwalldir;

/*
===============
=
= PushWall
=
===============
*/

void PushWall (int checkx, int checky, int dir)
{
#ifdef WOLFDOSMPU
	statobj_t *statptr;
#endif // WOLFDOSMPU

	int		oldtile;

	if (pwallstate)
#ifdef WOLFDOSMPU
	{
	  // for consistency
	  SD_PlaySound(DONOTHINGSND);
#endif // WOLFDOSMPU
	  return;
#ifdef WOLFDOSMPU
	}
#endif // WOLFDOSMPU


	oldtile = tilemap[checkx][checky];
	if (!oldtile)
#ifdef WOLFDOSMPU
	{
		// for consistency
		SD_PlaySound(DONOTHINGSND);
#endif // WOLFDOSMPU
		return;
#ifdef WOLFDOSMPU
	}
#endif // WOLFDOSMPU

#ifdef WOLFDOSMPU
	for (statptr = &statobjlist[0]; statptr != laststatobj; statptr++)
	{
		if (statptr->shapenum != -1 && statptr->flags & FL_BONUS && statptr->tilex == checkx && statptr->tiley == checky && statptr->itemnumber == block)
		{
			// if there is a "bonus" barrel inside the wall, this is a previously-pushed wall
			if (compflags & COMPFLAG_REACTIVATING_PUSHWALLS)
			{
				// if compatibility flag is enabled, allow pushing, but track this barrel for removal later
				break;
			}
			else
			{
				// do not allow pushing by default
				SD_PlaySound(DONOTHINGSND);
				return;
			}
		}
	}
#endif // WOLFDOSMPU

	switch (dir)
	{
	case di_north:
		if (actorat[checkx][checky-1])
		{
			SD_PlaySound (NOWAYSND);
			return;
		}
		(unsigned)actorat[checkx][checky-1] =
		tilemap[checkx][checky-1] = oldtile;
		break;

	case di_east:
		if (actorat[checkx+1][checky])
		{
			SD_PlaySound (NOWAYSND);
			return;
		}
		(unsigned)actorat[checkx+1][checky] =
		tilemap[checkx+1][checky] = oldtile;
		break;

	case di_south:
		if (actorat[checkx][checky+1])
		{
			SD_PlaySound (NOWAYSND);
			return;
		}
		(unsigned)actorat[checkx][checky+1] =
		tilemap[checkx][checky+1] = oldtile;
		break;

	case di_west:
		if (actorat[checkx-1][checky])
		{
			SD_PlaySound (NOWAYSND);
			return;
		}
		(unsigned)actorat[checkx-1][checky] =
		tilemap[checkx-1][checky] = oldtile;
		break;
	}

	gamestate.secretcount++;
	pwallx = checkx;
	pwally = checky;
	pwalldir = dir;
	pwallstate = 1;
	pwallpos = 0;
	tilemap[pwallx][pwally] |= 0xc0;
#ifdef WOLFDOSMPU
	if (statptr != laststatobj)
	{
		// if we had a "bonus" barrel, remove it (deception!!)
		statptr->shapenum = -1;
	}

	// do not remove pushwall tile info in the old manner below;
	// when the game is reloaded, secret tiles become reenabled!
	// (we handle it by placing a barrel inside a used secret wall instead)
#else  // WOLFDOSMPU
	*(mapsegs[1]+farmapylookup[pwally]+pwallx) = 0;	// remove P tile info
#endif // WOLFDOSMPU

#ifdef WASD
	// mark the wall as a secret in the automap when the player dies
	spotvis[pwallx][pwally] |= 0x80;
#endif // WASD

#ifdef WOLFDOSMPU
	PlaySoundLocTile(PUSHWALLSND, pwallx, pwally);
#else  // WOLFDOSMPU
	SD_PlaySound (PUSHWALLSND);
#endif // WOLFDOSMPU
}



/*
=================
=
= MovePWalls
=
=================
*/

void MovePWalls (void)
{
	int		oldblock,oldtile;

	if (!pwallstate)
		return;

	oldblock = pwallstate/128;

	pwallstate += tics;

	if (pwallstate/128 != oldblock)
	{
	// block crossed into a new block
		oldtile = tilemap[pwallx][pwally] & 63;

		//
		// the tile can now be walked into
		//
		tilemap[pwallx][pwally] = 0;
		(unsigned)actorat[pwallx][pwally] = 0;
#ifdef WOLFDOSMPU
		FixAreaTiles();

		// do not change area tile info in the old manner below; when the game is reloaded,
		// the areanumber will become undefined and further area connectivity checks will
		// cause memory corruption of the horizwall/vertwall arrays, and unfortunate wall
		// tiles get replaced with texture number 1
#else  // WOLFDOSMPU
		*(mapsegs[0]+farmapylookup[pwally]+pwallx) = player->areanumber+AREATILE;
#endif // WOLFDOSMPU

		//
		// see if it should be pushed farther
		//
#ifdef WOLFDOSMPU
		// make sure pwalls deterministically move the number of spaces wanted
		if (pwallstate >= ((compflags & COMPFLAG_3_TILE_PUSHWALLS) ? 3 : 2) * 128)
#else  // WOLFDOSMPU
		if (pwallstate>256)
#endif // WOLFDOSMPU
		{
		//
		// the block has been pushed two tiles
		//
			pwallstate = 0;
#ifdef WOLFDOSMPU
			switch (pwalldir)
			{
			case di_north:
				pwally--;
				break;
			case di_east:
				pwallx++;
				break;
			case di_south:
				pwally++;
				break;
			case di_west:
				pwallx--;
				break;
			}
			// if a pushwall ended up in a spot where another pushwall had been, disable the pushwall by placing a "bonus" barrel inside it
			if (! (compflags & COMPFLAG_REACTIVATING_PUSHWALLS) && *(mapsegs[1]+farmapylookup[pwally]+pwallx) == PUSHABLETILE)
				PlaceItemType(block, pwallx, pwally);
#endif // WOLFDOSMPU
			return;
		}
		else
		{
			switch (pwalldir)
			{
			case di_north:
				pwally--;
				if (actorat[pwallx][pwally-1])
				{
					pwallstate = 0;
#ifdef WOLFDOSMPU
					// if a pushwall ended up in a spot where another pushwall had been, disable the pushwall by placing a "bonus" barrel inside it
					if (! (compflags & COMPFLAG_REACTIVATING_PUSHWALLS) && *(mapsegs[1]+farmapylookup[pwally]+pwallx) == PUSHABLETILE)
						PlaceItemType(block, pwallx, pwally);
#endif // WOLFDOSMPU
					return;
				}
#ifdef WOLFDOSMPU
				// if landing tile is a door side, grab its door side attribute
				oldtile |= tilemap[pwallx][pwally-1] & 0x40;
#endif // WOLFDOSMPU
				(unsigned)actorat[pwallx][pwally-1] =
				tilemap[pwallx][pwally-1] = oldtile;
				break;

			case di_east:
				pwallx++;
				if (actorat[pwallx+1][pwally])
				{
					pwallstate = 0;
#ifdef WOLFDOSMPU
					// if a pushwall ended up in a spot where another pushwall had been, disable the pushwall by placing a "bonus" barrel inside it
					if (! (compflags & COMPFLAG_REACTIVATING_PUSHWALLS) && *(mapsegs[1]+farmapylookup[pwally]+pwallx) == PUSHABLETILE)
						PlaceItemType(block, pwallx, pwally);
#endif // WOLFDOSMPU
					return;
				}
#ifdef WOLFDOSMPU
				// if landing tile is a door side, grab its door side attribute
				oldtile |= tilemap[pwallx+1][pwally] & 0x40;
#endif // WOLFDOSMPU
				(unsigned)actorat[pwallx+1][pwally] =
				tilemap[pwallx+1][pwally] = oldtile;
				break;

			case di_south:
				pwally++;
				if (actorat[pwallx][pwally+1])
				{
					pwallstate = 0;
#ifdef WOLFDOSMPU
					// if a pushwall ended up in a spot where another pushwall had been, disable the pushwall by placing a "bonus" barrel inside it
					if (! (compflags & COMPFLAG_REACTIVATING_PUSHWALLS) && *(mapsegs[1]+farmapylookup[pwally]+pwallx) == PUSHABLETILE)
						PlaceItemType(block, pwallx, pwally);
#endif // WOLFDOSMPU
					return;
				}
#ifdef WOLFDOSMPU
				// if landing tile is a door side, grab its door side attribute
				oldtile |= tilemap[pwallx][pwally+1] & 0x40;
#endif // WOLFDOSMPU
				(unsigned)actorat[pwallx][pwally+1] =
				tilemap[pwallx][pwally+1] = oldtile;
				break;

			case di_west:
				pwallx--;
				if (actorat[pwallx-1][pwally])
				{
					pwallstate = 0;
#ifdef WOLFDOSMPU
					// if a pushwall ended up in a spot where another pushwall had been, disable the pushwall by placing a "bonus" barrel inside it
					if (! (compflags & COMPFLAG_REACTIVATING_PUSHWALLS) && *(mapsegs[1]+farmapylookup[pwally]+pwallx) == PUSHABLETILE)
						PlaceItemType(block, pwallx, pwally);
#endif // WOLFDOSMPU
					return;
				}
#ifdef WOLFDOSMPU
				// if landing tile is a door side, grab its door side attribute
				oldtile |= tilemap[pwallx-1][pwally] & 0x40;
#endif // WOLFDOSMPU
				(unsigned)actorat[pwallx-1][pwally] =
				tilemap[pwallx-1][pwally] = oldtile;
				break;
			}

			tilemap[pwallx][pwally] = oldtile | 0xc0;
		}
	}


	pwallpos = (pwallstate/2)&63;

}

