// WL_MAIN.C

#include <conio.h>
#include "WL_DEF.H"
#pragma hdrstop


/*
=============================================================================

						   WOLFENSTEIN 3-D

					  An Id Software production

						   by John Carmack

=============================================================================
*/

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/


#define FOCALLENGTH     (0x5700l)               // in global coordinates
#define VIEWGLOBAL      0x10000                 // globals visable flush to wall

#define VIEWWIDTH       256                     // size of view window
#define VIEWHEIGHT      144

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

char            str[80],str2[20];
int				tedlevelnum;
boolean         tedlevel;
boolean         nospr;
boolean         IsA386;
int                     dirangle[9] = {0,ANGLES/8,2*ANGLES/8,3*ANGLES/8,4*ANGLES/8,
	5*ANGLES/8,6*ANGLES/8,7*ANGLES/8,ANGLES};

//
// proejection variables
//
fixed           focallength;
unsigned        screenofs;
int             viewwidth;
int             viewheight;
int             centerx;
int             shootdelta;                     // pixels away from centerx a target can be
fixed           scale,maxslope;
long            heightnumerator;
int                     minheightdiv;


void            Quit (char *error);

boolean         startgame,loadedgame,virtualreality;
int             mouseadjustment;

#ifdef WASD
char	configname[13]="CONFIG_.";
#else  // WASD
char	configname[13]="CONFIG.";
#endif // WASD


/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/


/*
====================
=
= ReadConfig
=
====================
*/

void ReadConfig(void)
{
	int                     file;
	SDMode          sd;
	SMMode          sm;
	SDSMode         sds;


	if ( (file = open(configname,O_BINARY | O_RDONLY)) != -1)
	{
	//
	// valid config file
	//
		read(file,Scores,sizeof(HighScore) * MaxScores);

		read(file,&sd,sizeof(sd));
		read(file,&sm,sizeof(sm));
		read(file,&sds,sizeof(sds));

		read(file,&mouseenabled,sizeof(mouseenabled));
		read(file,&joystickenabled,sizeof(joystickenabled));
		read(file,&joypadenabled,sizeof(joypadenabled));
		read(file,&joystickprogressive,sizeof(joystickprogressive));
		read(file,&joystickport,sizeof(joystickport));

		read(file,&dirscan,sizeof(dirscan));
		read(file,&buttonscan,sizeof(buttonscan));
		read(file,&buttonmouse,sizeof(buttonmouse));
		read(file,&buttonjoy,sizeof(buttonjoy));

		read(file,&viewsize,sizeof(viewsize));
		read(file,&mouseadjustment,sizeof(mouseadjustment));

#ifdef WASD
		{
			byte value;
			read(file,&value,sizeof(value));
			keysalwaysstrafe = value;
			if (keysalwaysstrafe > 1)
				keysalwaysstrafe = 1;
			read(file,&value,sizeof(value));
			alwaysrun = value;
			if (alwaysrun > 1)
				alwaysrun = 1;
			read(file,&value,sizeof(value));
			mouseturningonly = value;
			if (mouseturningonly > 1)
				mouseturningonly = 1;
			read(file,&value,sizeof(value));
			// skip one byte for compatibility
			read(file,&value,sizeof(value));
			tabfunction = value;
			if (tabfunction > 2)
				tabfunction = 2;
			read(file,&value,sizeof(value));
			automapmode = value;
			if (automapmode > 3)
				automapmode = 3;
		}
#endif // WASD

#ifdef WOLFDOSMPU
		{
			// if the config file does not have the midi volume value yet, set it to max
			int value, bytesread;
			bytesread = read(file,&value,sizeof(value));
			if (bytesread < sizeof(value))
				midivolume = 10;
			else
			{
				midivolume = value;
				if (midivolume < 0)
					midivolume = 0;
				if (midivolume > 10)
					midivolume = 10;
			}
		}
		close(file);

		if (! opl2IsEnabled() && sd == sdm_AdLib)
			sd = sdm_PC;

		if (! SoundBlasterPresent && sds == sds_SoundBlaster)
		{
			if (SoundSourcePresent)
				sds = sds_SoundSource;
			else
				sds = sds_PC;
		}

		if (! SoundSourcePresent && sds == sds_SoundSource)
		{
			if (SoundBlasterPresent)
				sds = sds_SoundBlaster;
			else
				sds = sds_PC;
		}

		if (SoundSourcePresent && sds == sds_PC)
			sds = sds_SoundSource;

		if (! AdLibPresent && sm == smm_AdLib)	// really "if no music device is present"
			sm = smm_Off;
#else  // WOLFDOSMPU
		close(file);

		if (sd == sdm_AdLib && !AdLibPresent && !SoundBlasterPresent)
		{
			sd = sdm_PC;
			sd = smm_Off;
		}

		if ((sds == sds_SoundBlaster && !SoundBlasterPresent) ||
			(sds == sds_SoundSource && !SoundSourcePresent))
			sds = sds_Off;
#endif // WOLFDOSMPU

		if (!MousePresent)
			mouseenabled = false;
		if (!JoysPresent[joystickport])
			joystickenabled = false;

		MainMenu[6].active=1;
		MainItems.curpos=0;
	}
	else
	{
	//
	// no config file, so select by hardware
	//
#ifdef WOLFDOSMPU
		sd = sdm_PC;
		if (opl2IsEnabled())
			sd = sdm_AdLib;

		sds = sds_PC;
		if (SoundSourcePresent)
			sds = sds_SoundSource;
		if (SoundBlasterPresent)
			sds = sds_SoundBlaster;

		sm = smm_Off;
		if (AdLibPresent)	// really "if any music device is present"
			sm = smm_AdLib;
#else  // WOLFDOSMPU
		if (SoundBlasterPresent || AdLibPresent)
		{
			sd = sdm_AdLib;
			sm = smm_AdLib;
		}
		else
		{
			sd = sdm_PC;
			sm = smm_Off;
		}

		if (SoundBlasterPresent)
			sds = sds_SoundBlaster;
		else if (SoundSourcePresent)
			sds = sds_SoundSource;
		else
			sds = sds_Off;
#endif // WOLFDOSMPU

		if (MousePresent)
			mouseenabled = true;

		joystickenabled = false;
		joypadenabled = false;
		joystickport = 0;
		joystickprogressive = false;

		viewsize = 15;
		mouseadjustment=5;

#ifdef WASD
		viewsize = 19;
		keysalwaysstrafe = true;
		alwaysrun = false;
		mouseturningonly = true;
		tabfunction = 1;
		automapmode = 0;
#endif // WASD

#ifdef WOLFDOSMPU
		midivolume = 10;
#endif // WOLFDOSMPU
	}

	SD_SetMusicMode (sm);
	SD_SetSoundMode (sd);
	SD_SetDigiDevice (sds);

}


/*
====================
=
= WriteConfig
=
====================
*/

void WriteConfig(void)
{
	int                     file;

	file = open(configname,O_CREAT | O_BINARY | O_WRONLY,
				S_IREAD | S_IWRITE | S_IFREG);

	if (file != -1)
	{
		write(file,Scores,sizeof(HighScore) * MaxScores);

		write(file,&SoundMode,sizeof(SoundMode));
		write(file,&MusicMode,sizeof(MusicMode));
		write(file,&DigiMode,sizeof(DigiMode));

		write(file,&mouseenabled,sizeof(mouseenabled));
		write(file,&joystickenabled,sizeof(joystickenabled));
		write(file,&joypadenabled,sizeof(joypadenabled));
		write(file,&joystickprogressive,sizeof(joystickprogressive));
		write(file,&joystickport,sizeof(joystickport));

		write(file,&dirscan,sizeof(dirscan));
		write(file,&buttonscan,sizeof(buttonscan));
		write(file,&buttonmouse,sizeof(buttonmouse));
		write(file,&buttonjoy,sizeof(buttonjoy));

#ifdef WOLFDOSMPU
		// save what the user believes the viewsize currently is
		viewsize = savedviewsize;
#endif // WOLFDOSMPU
		write(file,&viewsize,sizeof(viewsize));
		write(file,&mouseadjustment,sizeof(mouseadjustment));

#ifdef WASD
		{
			byte value;
			value = keysalwaysstrafe;
			write(file,&value,sizeof(value));
			value = alwaysrun;
			write(file,&value,sizeof(value));
			value = mouseturningonly;
			write(file,&value,sizeof(value));
			value = 0;
			write(file,&value,sizeof(value));
			value = tabfunction;
			write(file,&value,sizeof(value));
			value = automapmode;
			write(file,&value,sizeof(value));
		}
#endif // WASD

#ifdef WOLFDOSMPU
		{
			// this will be ignored by the original executable and would not actually change its value
			int value;
			value = midivolume;
			write(file,&value,sizeof(value));
		}
#endif // WOLFDOSMPU

		close(file);
	}
}


//===========================================================================


/*
========================
=
= Patch386
=
= Patch ldiv to use 32 bit instructions
=
========================
*/

char    *JHParmStrings[] = {"no386",nil};
void Patch386 (void)
{
extern void far jabhack2(void);
extern int far  CheckIs386(void);

	int     i;

	for (i = 1;i < _argc;i++)
		if (US_CheckParm(_argv[i],JHParmStrings) == 0)
		{
			IsA386 = false;
			return;
		}

	if (CheckIs386())
	{
		IsA386 = true;
		jabhack2();
	}
	else
		IsA386 = false;
}

//===========================================================================

/*
=====================
=
= NewGame
=
= Set up new game to start from the beginning
=
=====================
*/

void NewGame (int difficulty,int episode)
{
#ifdef WOLFDOSMPU
	ResetToDefault();
#endif // WOLFDOSMPU
	memset (&gamestate,0,sizeof(gamestate));
	gamestate.difficulty = difficulty;
	gamestate.weapon = gamestate.bestweapon
		= gamestate.chosenweapon = wp_pistol;
	gamestate.health = 100;
	gamestate.ammo = STARTAMMO;
	gamestate.lives = 3;
	gamestate.nextextra = EXTRAPOINTS;
	gamestate.episode=episode;

	startgame = true;
}

//===========================================================================

#ifdef WOLFDOSMPU
// data segment locations of important savegame variables
// order is objlist, spotvis, statobjlist, s_player
# ifdef SPEAR
unsigned far dspos[6][4] =
{
	{ 0xB312, 0xA30C, 0x853C, 0x17AC },		// activision
	{ 0xA224, 0x921E, 0x765A, 0x14B4 },		// formgen demo
	{ 0xA4CC, 0x94C6, 0x78BA, 0x1714 },		// formgen 1.0
	{ 0xA4BA, 0x94B4, 0x78AA, 0x1712 },		// formgen 1.4
	{ 0xA78A, 0x9784, 0x7B7A, 0x16E2 },		// old wolfdosmpu
	{ 0xA4E2, 0x94DC, 0x791A, 0x1482 }		// old wolfdosmpu demo
};
#  ifdef SPEARDEMO
#   define DEFAULT_DSPOS 1
#  else
#   define DEFAULT_DSPOS 0
#  endif
# else
unsigned far dspos[6][4] =
{
	{ 0xB51C, 0xA516, 0x893C, 0x190C },		// activision
	{ 0xAC4E, 0x9C48, 0x803A, 0x18F8 },		// apogee registered
	{ 0xAB6E, 0x9B68, 0x7F5A, 0x181C },		// apogee shareware
	{ 0xAC46, 0x9C40, 0x804A, 0x190A },		// gt/id
	{ 0xAC2E, 0x9C28, 0x801A, 0x18DA },		// old wolfdosmpu
	{ 0xAB4E, 0x9B48, 0x7F3A, 0x17FE }		// old wolfdosmpu shareware
};
#  ifdef UPLOAD
#   define DEFAULT_DSPOS 2
#  else
#   ifdef GOODTIMES
#    define DEFAULT_DSPOS 0
#   else
#    define DEFAULT_DSPOS 1
#   endif
#  endif
# endif
int far current_dspos = DEFAULT_DSPOS;

int ResetToDefault()
{
	current_dspos = DEFAULT_DSPOS;
	return 0;
}

// fix data structures to preserve savegame compatibility with the current game version
void FixSaving()
{
	extern statetype s_player;
	int i, j;
	statobj_t *statptr;
	objtype *ob;

# ifdef SPEAR
	// do not save a game beyond level 2 in demo format
	if (current_dspos == 1 && gamestate.mapon > 1)
		ResetToDefault();
# endif

	for (i = 0; i < 64; i++)
	{
		for (j = 0; j < 64; j++)
		{
			if ((unsigned) actorat[i][j] > 256)
				(unsigned) actorat[i][j] = (unsigned) actorat[i][j] - (unsigned) &objlist[0] + dspos[current_dspos][0];
		}
	}
	for (statptr = &statobjlist[0]; statptr != laststatobj; statptr++)
		(unsigned) statptr->visspot = (unsigned) statptr->visspot - (unsigned) &spotvis[0][0] + dspos[current_dspos][1];
	(unsigned) laststatobj = (unsigned) laststatobj - (unsigned) &statobjlist[0] + dspos[current_dspos][2];
	for (ob = player; ob; ob = ob->next)
		(unsigned) ob->state = (unsigned) ob->state - (unsigned) &s_player + dspos[current_dspos][3];
	(unsigned) objlist[0].next = dspos[current_dspos][0] + sizeof(objtype);		// NOTE: this would break if there are no kills!
}

// fix data structures after loading a savegame
void FixLoading()
{
	extern statetype s_player;
	int i, j;
	statobj_t *statptr;
	objtype *ob;

	for (i = 0; i < 64; i++)
	{
		for (j = 0; j < 64; j++)
		{
			if ((unsigned) actorat[i][j] > 256)
				(unsigned) actorat[i][j] = (unsigned) actorat[i][j] + (unsigned) &objlist[0] - dspos[current_dspos][0];
		}
	}
	(unsigned) laststatobj = (unsigned) laststatobj + (unsigned) &statobjlist[0] - dspos[current_dspos][2];
	for (statptr = &statobjlist[0]; statptr != laststatobj; statptr++)
		(unsigned) statptr->visspot = (unsigned) statptr->visspot + (unsigned) &spotvis[0][0] - dspos[current_dspos][1];
	objlist[0].next = &objlist[1];		// NOTE: this would break if there are no kills!
	for (ob = player; ob; ob = ob->next)
		(unsigned) ob->state = (unsigned) ob->state + (unsigned) &s_player - dspos[current_dspos][3];

	// if loading an old wolfdosmpu savegame, automatically convert it to the version used by the current EXE
	if (current_dspos >= 4)
		ResetToDefault();
}

#endif // WOLFDOSMPU

void DiskFlopAnim(int x,int y)
{
 static char which=0;
 if (!x && !y)
   return;
 VWB_DrawPic(x,y,C_DISKLOADING1PIC+which);
 VW_UpdateScreen();
 which^=1;
}


long DoChecksum(byte far *source,unsigned size,long checksum)
{
 unsigned i;

 for (i=0;i<size-1;i++)
   checksum += source[i]^source[i+1];

 return checksum;
}


/*
==================
=
= SaveTheGame
=
==================
*/

boolean SaveTheGame(int file,int x,int y)
{
	struct diskfree_t dfree;
	long avail,size,checksum;
	objtype *ob,nullobj;


	if (_dos_getdiskfree(0,&dfree))
	  Quit("Error in _dos_getdiskfree call");

	avail = (long)dfree.avail_clusters *
			dfree.bytes_per_sector *
			dfree.sectors_per_cluster;

	size = 0;
	for (ob = player; ob ; ob=ob->next)
	  size += sizeof(*ob);
	size += sizeof(nullobj);

	size += sizeof(gamestate) +
			sizeof(LRstruct)*8 +
			sizeof(tilemap) +
			sizeof(actorat) +
			sizeof(laststatobj) +
			sizeof(statobjlist) +
			sizeof(doorposition) +
			sizeof(pwallstate) +
			sizeof(pwallx) +
			sizeof(pwally) +
			sizeof(pwalldir) +
			sizeof(pwallpos);

#ifdef WOLFDOSMPU
	// account for bytes that the original code did not consider
	size += 32;	// savegame name, written outside the call to SaveTheGame
#ifdef SPEAR
	size += sizeof(LRstruct)*12;
#endif
	size += sizeof(doorobjlist) + sizeof(checksum);
#endif // WOLFDOSMPU
#ifdef WASD
	size += sizeof(int) + sizeof(spotvis);
#endif // WASD

	if (avail < size)
	{
	 Message(STR_NOSPACE1"\n"
			 STR_NOSPACE2);
	 return false;
	}

#ifdef WOLFDOSMPU
	FixSaving();
#endif // WOLFDOSMPU

	checksum = 0;


	DiskFlopAnim(x,y);
	CA_FarWrite (file,(void far *)&gamestate,sizeof(gamestate));
	checksum = DoChecksum((byte far *)&gamestate,sizeof(gamestate),checksum);

	DiskFlopAnim(x,y);
#ifdef SPEAR
#ifdef WOLFDOSMPU
	{
		// FormGen savegames only save the first 8 levels' ratios
		int multiplier = 20;
		if (current_dspos >= 1 && current_dspos <= 3)
			multiplier = 8;
		CA_FarWrite(file, (void far *) &LevelRatios[0], sizeof(LRstruct) * multiplier);
		checksum = DoChecksum((byte far *) &LevelRatios[0], sizeof(LRstruct) * multiplier, checksum);
	}
#else  // WOLFDOSMPU
	CA_FarWrite (file,(void far *)&LevelRatios[0],sizeof(LRstruct)*20);
	checksum = DoChecksum((byte far *)&LevelRatios[0],sizeof(LRstruct)*20,checksum);
#endif // WOLFDOSMPU
#else
	CA_FarWrite (file,(void far *)&LevelRatios[0],sizeof(LRstruct)*8);
	checksum = DoChecksum((byte far *)&LevelRatios[0],sizeof(LRstruct)*8,checksum);
#endif

	DiskFlopAnim(x,y);
	CA_FarWrite (file,(void far *)tilemap,sizeof(tilemap));
	checksum = DoChecksum((byte far *)tilemap,sizeof(tilemap),checksum);
	DiskFlopAnim(x,y);
	CA_FarWrite (file,(void far *)actorat,sizeof(actorat));
	checksum = DoChecksum((byte far *)actorat,sizeof(actorat),checksum);

	CA_FarWrite (file,(void far *)areaconnect,sizeof(areaconnect));
	CA_FarWrite (file,(void far *)areabyplayer,sizeof(areabyplayer));

#ifdef WOLFDOSMPU
	DiskFlopAnim(x, y);
	CA_FarWrite(file, (void far *) player, sizeof(*player));
	for (ob = &objlist[1]; ob; ob = ob->next)
#else  // WOLFDOSMPU
	for (ob = player ; ob ; ob=ob->next)
#endif // WOLFDOSMPU
	{
	 DiskFlopAnim(x,y);
	 CA_FarWrite (file,(void far *)ob,sizeof(*ob));
	}
	nullobj.active = ac_badobject;          // end of file marker
	DiskFlopAnim(x,y);
	CA_FarWrite (file,(void far *)&nullobj,sizeof(nullobj));



	DiskFlopAnim(x,y);
	CA_FarWrite (file,(void far *)&laststatobj,sizeof(laststatobj));
	checksum = DoChecksum((byte far *)&laststatobj,sizeof(laststatobj),checksum);
	DiskFlopAnim(x,y);
	CA_FarWrite (file,(void far *)statobjlist,sizeof(statobjlist));
	checksum = DoChecksum((byte far *)statobjlist,sizeof(statobjlist),checksum);

	DiskFlopAnim(x,y);
	CA_FarWrite (file,(void far *)doorposition,sizeof(doorposition));
	checksum = DoChecksum((byte far *)doorposition,sizeof(doorposition),checksum);
	DiskFlopAnim(x,y);
	CA_FarWrite (file,(void far *)doorobjlist,sizeof(doorobjlist));
	checksum = DoChecksum((byte far *)doorobjlist,sizeof(doorobjlist),checksum);

	DiskFlopAnim(x,y);
	CA_FarWrite (file,(void far *)&pwallstate,sizeof(pwallstate));
	checksum = DoChecksum((byte far *)&pwallstate,sizeof(pwallstate),checksum);
	CA_FarWrite (file,(void far *)&pwallx,sizeof(pwallx));
	checksum = DoChecksum((byte far *)&pwallx,sizeof(pwallx),checksum);
	CA_FarWrite (file,(void far *)&pwally,sizeof(pwally));
	checksum = DoChecksum((byte far *)&pwally,sizeof(pwally),checksum);
	CA_FarWrite (file,(void far *)&pwalldir,sizeof(pwalldir));
	checksum = DoChecksum((byte far *)&pwalldir,sizeof(pwalldir),checksum);
	CA_FarWrite (file,(void far *)&pwallpos,sizeof(pwallpos));
	checksum = DoChecksum((byte far *)&pwallpos,sizeof(pwallpos),checksum);

	//
	// WRITE OUT CHECKSUM
	//
	CA_FarWrite (file,(void far *)&checksum,sizeof(checksum));

#ifdef WASD
	// try to save further information at the end of the file
	{
		int extraData = 1;
		CA_FarWrite(file, (void far *) &extraData, sizeof(extraData));
		WriteSpotVis(file);
	}
#endif // WASD

#ifdef WOLFDOSMPU
	FixLoading();
#endif // WOLFDOSMPU

	return(true);
}

//===========================================================================

/*
==================
=
= LoadTheGame
=
==================
*/

boolean LoadTheGame(int file,int x,int y)
{
	long checksum,oldchecksum;
	objtype *ob,nullobj;
#ifdef WOLFDOSMPU
# ifdef SPEAR
	int i;
# endif
#endif // WOLFDOSMPU


	checksum = 0;

	DiskFlopAnim(x,y);
	CA_FarRead (file,(void far *)&gamestate,sizeof(gamestate));
	checksum = DoChecksum((byte far *)&gamestate,sizeof(gamestate),checksum);

	DiskFlopAnim(x,y);
#ifdef SPEAR
	CA_FarRead (file,(void far *)&LevelRatios[0],sizeof(LRstruct)*20);
#ifdef WOLFDOSMPU
	for (i = 8; i < 20; i++)
	{
		// if the treasure ratio is outside the range [0, 100], the game is trying
		// to load a FormGen savegame, which only saves the first 8 levels' ratios
		// (this hack depends on the fact that the tilemap would be partially loaded
		// into the LevelRatio struct, and the first row of bytes would inevitably
		// be nonzero due to the wall tiles required to enclose the level)
		// (also note that the kill and secret ratios are ignored, because it is possible
		// to exceed 100 percent when there are spectres or reactivating pushwalls)
		// NOTE: While very unlikely, a user map can specifically break this code!
		if (LevelRatios[i].treasure < 0 || LevelRatios[i].treasure > 100)
			break;
	}
	if (i < 20)
		checksum = DoChecksum((byte far *) &LevelRatios[0], sizeof(LRstruct) * 8, checksum);
	else
#endif // WOLFDOSMPU
	checksum = DoChecksum((byte far *)&LevelRatios[0],sizeof(LRstruct)*20,checksum);
#else
	CA_FarRead (file,(void far *)&LevelRatios[0],sizeof(LRstruct)*8);
	checksum = DoChecksum((byte far *)&LevelRatios[0],sizeof(LRstruct)*8,checksum);
#endif

	DiskFlopAnim(x,y);
	SetupGameLevel ();

	DiskFlopAnim(x,y);
#ifdef WOLFDOSMPU
# ifdef SPEAR
	if (i < 20)
	{
		// recover the tilemap bits that were mistakenly placed into LevelRatios
		memcpy(tilemap, &LevelRatios[8], sizeof(LRstruct) * 12);
		memset(&LevelRatios[8], 0, sizeof(LRstruct) * 12);
		CA_FarRead(file, (void far *) (void *) ((unsigned) tilemap + sizeof(LRstruct) * 12), sizeof(tilemap) - sizeof(LRstruct) * 12);
	}
	else
# endif
#endif // WOLFDOSMPU
	CA_FarRead (file,(void far *)tilemap,sizeof(tilemap));
	checksum = DoChecksum((byte far *)tilemap,sizeof(tilemap),checksum);
	DiskFlopAnim(x,y);
	CA_FarRead (file,(void far *)actorat,sizeof(actorat));
	checksum = DoChecksum((byte far *)actorat,sizeof(actorat),checksum);

	CA_FarRead (file,(void far *)areaconnect,sizeof(areaconnect));
	CA_FarRead (file,(void far *)areabyplayer,sizeof(areabyplayer));



	InitActorList ();
	DiskFlopAnim(x,y);
	CA_FarRead (file,(void far *)player,sizeof(*player));

#ifdef WOLFDOSMPU
	// determine game version
	for (current_dspos = 0; current_dspos < 6; current_dspos++)
	{
		if ((unsigned) objlist[0].next == dspos[current_dspos][0] + sizeof(objtype))	// NOTE: this would break if there are no kills!
			break;
	}
	if (current_dspos == 6)
		Quit("Bad savegame");
#endif // WOLFDOSMPU

	while (1)
	{
	 DiskFlopAnim(x,y);
		CA_FarRead (file,(void far *)&nullobj,sizeof(nullobj));
		if (nullobj.active == ac_badobject)
			break;
		GetNewActor ();
	 // don't copy over the links
		memcpy (new,&nullobj,sizeof(nullobj)-4);
	}



	DiskFlopAnim(x,y);
	CA_FarRead (file,(void far *)&laststatobj,sizeof(laststatobj));
	checksum = DoChecksum((byte far *)&laststatobj,sizeof(laststatobj),checksum);
	DiskFlopAnim(x,y);
	CA_FarRead (file,(void far *)statobjlist,sizeof(statobjlist));
	checksum = DoChecksum((byte far *)statobjlist,sizeof(statobjlist),checksum);

	DiskFlopAnim(x,y);
	CA_FarRead (file,(void far *)doorposition,sizeof(doorposition));
	checksum = DoChecksum((byte far *)doorposition,sizeof(doorposition),checksum);
	DiskFlopAnim(x,y);
	CA_FarRead (file,(void far *)doorobjlist,sizeof(doorobjlist));
	checksum = DoChecksum((byte far *)doorobjlist,sizeof(doorobjlist),checksum);

	DiskFlopAnim(x,y);
	CA_FarRead (file,(void far *)&pwallstate,sizeof(pwallstate));
	checksum = DoChecksum((byte far *)&pwallstate,sizeof(pwallstate),checksum);
	CA_FarRead (file,(void far *)&pwallx,sizeof(pwallx));
	checksum = DoChecksum((byte far *)&pwallx,sizeof(pwallx),checksum);
	CA_FarRead (file,(void far *)&pwally,sizeof(pwally));
	checksum = DoChecksum((byte far *)&pwally,sizeof(pwally),checksum);
	CA_FarRead (file,(void far *)&pwalldir,sizeof(pwalldir));
	checksum = DoChecksum((byte far *)&pwalldir,sizeof(pwalldir),checksum);
	CA_FarRead (file,(void far *)&pwallpos,sizeof(pwallpos));
	checksum = DoChecksum((byte far *)&pwallpos,sizeof(pwallpos),checksum);

	CA_FarRead (file,(void far *)&oldchecksum,sizeof(oldchecksum));

	if (oldchecksum != checksum)
	{
	 Message(STR_SAVECHT1"\n"
			 STR_SAVECHT2"\n"
			 STR_SAVECHT3"\n"
			 STR_SAVECHT4);

	 IN_ClearKeysDown();
	 IN_Ack();

	 gamestate.score = 0;
	 gamestate.lives = 1;
	 gamestate.weapon =
	   gamestate.chosenweapon =
	   gamestate.bestweapon = wp_pistol;
	 gamestate.ammo = 8;
	}

#ifdef WASD
	// try to load further information from the end of the file
	{
		int extraData = 0;
		CA_FarRead(file, (void far *) &extraData, sizeof(extraData));
		if (extraData >= 1)
			ReadSpotVis(file);
		else
			ResetSpotVis();
	}
#endif // WASD
#ifdef WOLFDOSMPU
	FixLoading();
	FixAreaTiles();
#endif // WOLFDOSMPU

	return true;
}

//===========================================================================

/*
==========================
=
= ShutdownId
=
= Shuts down all ID_?? managers
=
==========================
*/

void ShutdownId (void)
{
	US_Shutdown ();
	SD_Shutdown ();
	PM_Shutdown ();
	IN_Shutdown ();
	VW_Shutdown ();
	CA_Shutdown ();
	MM_Shutdown ();
#ifdef WOLFDOSMPU
	mpuDestroy();
#endif // WOLFDOSMPU
}


//===========================================================================

/*
==================
=
= BuildTables
=
= Calculates:
=
= scale                 projection constant
= sintable/costable     overlapping fractional tables
=
==================
*/

const   float   radtoint = (float)FINEANGLES/2/PI;

void BuildTables (void)
{
  int           i;
  float         angle,anglestep;
  double        tang;
  fixed         value;


//
// calculate fine tangents
//

	for (i=0;i<FINEANGLES/8;i++)
	{
		tang = tan( (i+0.5)/radtoint);
		finetangent[i] = tang*TILEGLOBAL;
		finetangent[FINEANGLES/4-1-i] = 1/tang*TILEGLOBAL;
	}

//
// costable overlays sintable with a quarter phase shift
// ANGLES is assumed to be divisable by four
//
// The low word of the value is the fraction, the high bit is the sign bit,
// bits 16-30 should be 0
//

  angle = 0;
  anglestep = PI/2/ANGLEQUAD;
  for (i=0;i<=ANGLEQUAD;i++)
  {
	value=GLOBAL1*sin(angle);
	sintable[i]=
	  sintable[i+ANGLES]=
	  sintable[ANGLES/2-i] = value;
	sintable[ANGLES-i]=
	  sintable[ANGLES/2+i] = value | 0x80000000l;
	angle += anglestep;
  }

}

//===========================================================================


/*
====================
=
= CalcProjection
=
= Uses focallength
=
====================
*/

void CalcProjection (long focal)
{
	int             i;
	long            intang;
	float   angle;
	double  tang;
	double  planedist;
	double  globinhalf;
	int             halfview;
	double  halfangle,facedist;


	focallength = focal;
	facedist = focal+MINDIST;
	halfview = viewwidth/2;                                 // half view in pixels

//
// calculate scale value for vertical height calculations
// and sprite x calculations
//
	scale = halfview*facedist/(VIEWGLOBAL/2);

//
// divide heightnumerator by a posts distance to get the posts height for
// the heightbuffer.  The pixel height is height>>2
//
	heightnumerator = (TILEGLOBAL*scale)>>6;
	minheightdiv = heightnumerator/0x7fff +1;

//
// calculate the angle offset from view angle of each pixel's ray
//

	for (i=0;i<halfview;i++)
	{
	// start 1/2 pixel over, so viewangle bisects two middle pixels
		tang = (long)i*VIEWGLOBAL/viewwidth/facedist;
		angle = atan(tang);
		intang = angle*radtoint;
		pixelangle[halfview-1-i] = intang;
		pixelangle[halfview+i] = -intang;
	}

//
// if a point's abs(y/x) is greater than maxslope, the point is outside
// the view area
//
	maxslope = finetangent[pixelangle[0]];
	maxslope >>= 8;
}



//===========================================================================

/*
===================
=
= SetupWalls
=
= Map tile values to scaled pics
=
===================
*/

void SetupWalls (void)
{
	int     i;

	for (i=1;i<MAXWALLTILES;i++)
	{
		horizwall[i]=(i-1)*2;
		vertwall[i]=(i-1)*2+1;
	}
}

//===========================================================================

/*
==========================
=
= SignonScreen
=
==========================
*/

void SignonScreen (void)                        // VGA version
{
	unsigned        segstart,seglength;

	VL_SetVGAPlaneMode ();
	VL_TestPaletteSet ();
	VL_SetPalette (&gamepal);

	if (!virtualreality)
	{
		VW_SetScreen(0x8000,0);
		VL_MungePic (&introscn,320,200);
		VL_MemToScreen (&introscn,320,200,0,0);
		VW_SetScreen(0,0);
	}

//
// reclaim the memory from the linked in signon screen
//
	segstart = FP_SEG(&introscn);
	seglength = 64000/16;
	if (FP_OFF(&introscn))
	{
		segstart++;
		seglength--;
	}
	MML_UseSpace (segstart,seglength);
}


/*
==========================
=
= FinishSignon
=
==========================
*/

void FinishSignon (void)
{
#ifdef WOLFDOSMPU

	char sz[32];
	sprintf(sz, "wolfdosmpu v%d.%02d - Press a key", (int) VERSION, ((int) ((VERSION + 0.001) * 100)) % 100);
#ifdef SPEAR
	VW_Bar(0, 187, 320, 2, 159);
	VW_Bar(0, 189, 320, 11, 157);
#else
	VW_Bar(0, 187, 320, 2, 47);
	VW_Bar(0, 189, 320, 11, 45);
#endif
	WindowX = 0;
	WindowW = 320;
	PrintY = 190;
	SETFONTCOLOR(14, 4);
	US_CPrint(sz);
	if (! NoWait)
		IN_Ack();
#ifdef SPEAR
	VW_Bar(0, 189, 320, 11, 157);
#else
	VW_Bar(0, 189, 320, 11, 45);
#endif
	PrintY = 190;
	US_CPrint("Working...");

#else  // WOLFDOSMPU

#ifndef SPEAR
	VW_Bar (0,189,300,11,peekb(0xa000,0));
	WindowX = 0;
	WindowW = 320;
	PrintY = 190;

	#ifndef JAPAN
	SETFONTCOLOR(14,4);

	#ifdef SPANISH
	US_CPrint ("Oprima una tecla");
	#else
	US_CPrint ("Press a key");
	#endif

	#endif

	if (!NoWait)
		IN_Ack ();

	#ifndef JAPAN
	VW_Bar (0,189,300,11,peekb(0xa000,0));

	PrintY = 190;
	SETFONTCOLOR(10,4);

	#ifdef SPANISH
	US_CPrint ("pensando...");
	#else
	US_CPrint ("Working...");
	#endif

	#endif

	SETFONTCOLOR(0,15);
#else
	if (!NoWait)
		VW_WaitVBL(3*70);
#endif

#endif // WOLFDOSMPU
}

//===========================================================================

/*
=================
=
= MS_CheckParm
=
=================
*/

boolean MS_CheckParm (char far *check)
{
	int             i;
	char    *parm;

	for (i = 1;i<_argc;i++)
	{
		parm = _argv[i];

		while ( !isalpha(*parm) )       // skip - / \ etc.. in front of parm
			if (!*parm++)
				break;                          // hit end of string without an alphanum

		if ( !_fstricmp(check,parm) )
			return true;
	}

	return false;
}

//===========================================================================

/*
=====================
=
= InitDigiMap
=
=====================
*/

static  int     wolfdigimap[] =
		{
			// These first sounds are in the upload version
#ifndef SPEAR
			HALTSND,                0,
			DOGBARKSND,             1,
			CLOSEDOORSND,           2,
			OPENDOORSND,            3,
			ATKMACHINEGUNSND,       4,
			ATKPISTOLSND,           5,
			ATKGATLINGSND,          6,
			SCHUTZADSND,            7,
			GUTENTAGSND,            8,
			MUTTISND,               9,
			BOSSFIRESND,            10,
			SSFIRESND,              11,
			DEATHSCREAM1SND,        12,
			DEATHSCREAM2SND,        13,
			DEATHSCREAM3SND,        13,
			TAKEDAMAGESND,          14,
			PUSHWALLSND,            15,

			LEBENSND,               20,
			NAZIFIRESND,            21,
			SLURPIESND,             22,

			YEAHSND,				32,

#ifndef UPLOAD
			// These are in all other episodes
			DOGDEATHSND,            16,
			AHHHGSND,               17,
			DIESND,                 18,
			EVASND,                 19,

			TOT_HUNDSND,            23,
			MEINGOTTSND,            24,
			SCHABBSHASND,           25,
			HITLERHASND,            26,
			SPIONSND,               27,
			NEINSOVASSND,           28,
			DOGATTACKSND,           29,
			LEVELDONESND,           30,
			MECHSTEPSND,			31,

			SCHEISTSND,				33,
			DEATHSCREAM4SND,		34,		// AIIEEE
			DEATHSCREAM5SND,		35,		// DEE-DEE
			DONNERSND,				36,		// EPISODE 4 BOSS DIE
			EINESND,				37,		// EPISODE 4 BOSS SIGHTING
			ERLAUBENSND,			38,		// EPISODE 6 BOSS SIGHTING
			DEATHSCREAM6SND,		39,		// FART
			DEATHSCREAM7SND,		40,		// GASP
			DEATHSCREAM8SND,		41,		// GUH-BOY!
			DEATHSCREAM9SND,		42,		// AH GEEZ!
			KEINSND,				43,		// EPISODE 5 BOSS SIGHTING
			MEINSND,				44,		// EPISODE 6 BOSS DIE
			ROSESND,				45,		// EPISODE 5 BOSS DIE

#endif
#else
//
// SPEAR OF DESTINY DIGISOUNDS
//
			HALTSND,                0,
			CLOSEDOORSND,           2,
			OPENDOORSND,            3,
			ATKMACHINEGUNSND,       4,
			ATKPISTOLSND,           5,
			ATKGATLINGSND,          6,
			SCHUTZADSND,            7,
			BOSSFIRESND,            8,
			SSFIRESND,              9,
			DEATHSCREAM1SND,        10,
			DEATHSCREAM2SND,        11,
			TAKEDAMAGESND,          12,
			PUSHWALLSND,            13,
			AHHHGSND,               15,
			LEBENSND,               16,
			NAZIFIRESND,            17,
			SLURPIESND,             18,
			LEVELDONESND,           22,
			DEATHSCREAM4SND,		23,		// AIIEEE
			DEATHSCREAM3SND,        23,		// DOUBLY-MAPPED!!!
			DEATHSCREAM5SND,		24,		// DEE-DEE
			DEATHSCREAM6SND,		25,		// FART
			DEATHSCREAM7SND,		26,		// GASP
			DEATHSCREAM8SND,		27,		// GUH-BOY!
			DEATHSCREAM9SND,		28,		// AH GEEZ!
			GETGATLINGSND,			38,		// Got Gat replacement

#ifndef SPEARDEMO
			DOGBARKSND,             1,
			DOGDEATHSND,            14,
			SPIONSND,               19,
			NEINSOVASSND,           20,
			DOGATTACKSND,           21,
			TRANSSIGHTSND,			29,		// Trans Sight
			TRANSDEATHSND,			30,		// Trans Death
			WILHELMSIGHTSND,		31,		// Wilhelm Sight
			WILHELMDEATHSND,		32,		// Wilhelm Death
			UBERDEATHSND,			33,		// Uber Death
			KNIGHTSIGHTSND,			34,		// Death Knight Sight
			KNIGHTDEATHSND,			35,		// Death Knight Death
			ANGELSIGHTSND,			36,		// Angel Sight
			ANGELDEATHSND,			37,		// Angel Death
			GETSPEARSND,			39,		// Got Spear replacement
#endif
#endif
			LASTSOUND
		};


void InitDigiMap (void)
{
	int                     *map;

	for (map = wolfdigimap;*map != LASTSOUND;map += 2)
		DigiMap[map[0]] = map[1];


}


#ifdef WOLFDOSMPU
#pragma warn -aus
CP_iteminfo	MusicItems={24,55,6,0,32};
CP_itemtype far MusicMenu[]=
	{
#ifndef SPEAR
		{1,"Theme Music",0},
		{1,"Episode 1",0},
		{1,"Episode 2",0},
		{1,"Episode 3",0},
		{1,"Secret Levels",0},
		{1,"Unused Music",0},

		{1,"Horst-Wessel-Lied",0},
		{1,"Wondering About Loved Ones",0},
		{1,"End of Level",0},
		{1,"U R A Hero",0},
		{1,"Roster",0},
		{1,"< Back",0},

		{1,"Get Them!",0},
		{1,"Searching",0},
		{1,"P.O.W.",0},
		{1,"Suspense",0},
		{1,"War March",0},
		{1,"< Back",0},

		{1,"Nazi Anthem",0},
		{1,"Lurking...",0},
		{1,"Going After Hitler",0},
		{1,"Pounding Headache",0},
		{1,"War March",0},
		{1,"< Back",0},

		{1,"Kill the S.O.B.",0},
		{1,"The Nazi Rap",0},
		{1,"Twelfth Hour",0},
		{1,"Zero Hour",0},
		{1,"Ultimate Conquest",0},
		{1,"< Back",0},

		{1,"Around The Corner!",0},
		{1,"Into the Dungeons",0},
		{1,"Wolfpack",0},
		{1,"Funk You!",0},
		{0,"",0},
		{1,"< Back",0},

		{1,"Hitler Waltz",0},
		{1,"Salute",0},
		{1,"Victors",0},
		{1,"Victory March",0},
		{0,"",0},
		{1,"< Back",0},
#else
		{1,"Theme Music",0},
		{1,"Chapter 1",0},
		{1,"Chapter 2",0},
		{1,"Chapter 3",0},
		{1,"Chapter 4",0},
		{1,"Unused Music",0},

		{1,"Towering Above",0},
		{1,"Wondering About Loved Ones",0},
		{1,"End of Level",0},
		{1,"U R A Hero",0},
		{1,"Award Ceremony",0},
		{1,"< Back",0},

		{1,"Tiptoeing Around",0},
		{1,"Funky Colonel Bill",0},
		{1,"Death To The Nazis",0},
		{1,"The SS Gonna Get You",0},
		{1,"Jazzin' Them Nazis",0},
		{1,"< Back",0},

		{1,"Into the Dungeons",0},
		{1,"Going After Hitler",0},
		{1,"P.O.W.",0},
		{1,"Twelfth Hour",0},
		{1,"Ultimate Conquest",0},
		{1,"< Back",0},

		{1,"Nazi Anthem",0},
		{1,"Get Them!",0},
		{1,"Suspense",0},
		{1,"Searching",0},
		{1,"Zero Hour",0},
		{1,"< Back",0},

		{1,"Puttin' It To The Enemy",0},
		{1,"Ultimate Conquest",0},
		{1,"Evil Incarnate",0},
		{1,"Is This THE END?",0},
		{0,"",0},
		{1,"< Back",0},

		{1,"Hitler Waltz",0},
		{0,"",0},
		{0,"",0},
		{0,"",0},
		{0,"",0},
		{1,"< Back",0},
#endif
	};
unsigned far MusicTracks[]=
	{
#ifndef SPEAR
		0,
		0,
		0,
		0,
		0,
		0,

		NAZI_NOR_MUS,
		WONDERIN_MUS,
		ENDLEVEL_MUS,
		URAHERO_MUS,
		ROSTER_MUS,
		0,

		GETTHEM_MUS,
		SEARCHN_MUS,
		POW_MUS,
		SUSPENSE_MUS,
		WARMARCH_MUS,
		0,

		NAZI_OMI_MUS,
		PREGNANT_MUS,
		GOINGAFT_MUS,
		HEADACHE_MUS,
		WARMARCH_MUS,
		0,

		INTROCW3_MUS,
		NAZI_RAP_MUS,
		TWELFTH_MUS,
		ZEROHOUR_MUS,
		ULTIMATE_MUS,
		0,

		CORNER_MUS,
		DUNGEON_MUS,
		PACMAN_MUS,
		FUNKYOU_MUS,
		0,
		0,

		HITLWLTZ_MUS,
		SALUTE_MUS,
		VICTORS_MUS,
		VICMARCH_MUS,
		0,
		0
#else
		0,
		0,
		0,
		0,
		0,
		0,

		XTOWER2_MUS,
		WONDERIN_MUS,
		ENDLEVEL_MUS,
		URAHERO_MUS,
		XAWARD_MUS,
		0,

		XTIPTOE_MUS,
		XFUNKIE_MUS,
		XDEATH_MUS,
		XGETYOU_MUS,
		XJAZNAZI_MUS,
		0,

		DUNGEON_MUS,
		GOINGAFT_MUS,
		POW_MUS,
		TWELFTH_MUS,
		ULTIMATE_MUS,
		0,

		NAZI_OMI_MUS,
		GETTHEM_MUS,
		SUSPENSE_MUS,
		SEARCHN_MUS,
		ZEROHOUR_MUS,
		0,

		XPUTIT_MUS,
		ULTIMATE_MUS,
		XEVIL_MUS,
		XTHEEND_MUS,
		0,
		0,

		COPYPRO_MUS,
		0,
		0,
		0,
		0,
		0
#endif
	};
#else  // WOLFDOSMPU
#ifndef SPEAR
CP_iteminfo	MusicItems={CTL_X,CTL_Y,6,0,32};
CP_itemtype far MusicMenu[]=
	{
		{1,"Get Them!",0},
		{1,"Searching",0},
		{1,"P.O.W.",0},
		{1,"Suspense",0},
		{1,"War March",0},
		{1,"Around The Corner!",0},

		{1,"Nazi Anthem",0},
		{1,"Lurking...",0},
		{1,"Going After Hitler",0},
		{1,"Pounding Headache",0},
		{1,"Into the Dungeons",0},
		{1,"Ultimate Conquest",0},

		{1,"Kill the S.O.B.",0},
		{1,"The Nazi Rap",0},
		{1,"Twelfth Hour",0},
		{1,"Zero Hour",0},
		{1,"Ultimate Conquest",0},
		{1,"Wolfpack",0}
	};
#else
CP_iteminfo MusicItems={CTL_X,CTL_Y-20,9,0,32};
CP_itemtype far MusicMenu[]=
   {
		{1,"Funky Colonel Bill",0},
		{1,"Death To The Nazis",0},
		{1,"Tiptoeing Around",0},
		{1,"Is This THE END?",0},
		{1,"Evil Incarnate",0},
		{1,"Jazzin' Them Nazis",0},
		{1,"Puttin' It To The Enemy",0},
		{1,"The SS Gonna Get You",0},
		{1,"Towering Above",0}
	};
#endif
#endif // WOLFDOSMPU

#ifndef SPEARDEMO
void DoJukebox(void)
{
	int which,lastsong=-1;
	unsigned start,songs[]=
		{
#ifndef SPEAR
			GETTHEM_MUS,
			SEARCHN_MUS,
			POW_MUS,
			SUSPENSE_MUS,
			WARMARCH_MUS,
			CORNER_MUS,

			NAZI_OMI_MUS,
			PREGNANT_MUS,
			GOINGAFT_MUS,
			HEADACHE_MUS,
			DUNGEON_MUS,
			ULTIMATE_MUS,

			INTROCW3_MUS,
			NAZI_RAP_MUS,
			TWELFTH_MUS,
			ZEROHOUR_MUS,
			ULTIMATE_MUS,
			PACMAN_MUS
#else
			XFUNKIE_MUS,             // 0
			XDEATH_MUS,              // 2
			XTIPTOE_MUS,             // 4
			XTHEEND_MUS,             // 7
			XEVIL_MUS,               // 17
			XJAZNAZI_MUS,            // 18
			XPUTIT_MUS,              // 21
			XGETYOU_MUS,             // 22
			XTOWER2_MUS              // 23
#endif
		};
	struct dostime_t time;



	IN_ClearKeysDown();
#ifdef WOLFDOSMPU
	if (! AdLibPresent)		// really "if no music device is present"
#else  // WOLFDOSMPU
	if (!AdLibPresent && !SoundBlasterPresent)
#endif // WOLFDOSMPU
		return;


	MenuFadeOut();

#ifdef WOLFDOSMPU
	start = 0;
#else  // WOLFDOSMPU
#ifndef SPEAR
#ifndef UPLOAD
	_dos_gettime(&time);
	start = (time.hsecond%3)*6;
#else
	start = 0;
#endif
#else
	start = 0;
#endif
#endif // WOLFDOSMPU


	CA_CacheGrChunk (STARTFONT+1);
#ifdef SPEAR
	CacheLump (BACKDROP_LUMP_START,BACKDROP_LUMP_END);
#else
	CacheLump (CONTROLS_LUMP_START,CONTROLS_LUMP_END);
#endif
	CA_LoadAllSounds ();

	fontnumber=1;
	ClearMScreen ();
	VWB_DrawPic(112,184,C_MOUSELBACKPIC);
	DrawStripes (10);
	SETFONTCOLOR (TEXTCOLOR,BKGDCOLOR);

#ifdef WOLFDOSMPU
	DrawWindow (18,49,292,13*7,BKGDCOLOR);
#else  // WOLFDOSMPU
#ifndef SPEAR
	DrawWindow (CTL_X-2,CTL_Y-6,280,13*7,BKGDCOLOR);
#else
	DrawWindow (CTL_X-2,CTL_Y-26,280,13*10,BKGDCOLOR);
#endif
#endif // WOLFDOSMPU

	DrawMenu (&MusicItems,&MusicMenu[start]);

	SETFONTCOLOR (READHCOLOR,BKGDCOLOR);
	PrintY=15;
	WindowX = 0;
	WindowY = 320;
	US_CPrint ("Robert's Jukebox");

	SETFONTCOLOR (TEXTCOLOR,BKGDCOLOR);
	VW_UpdateScreen();
	MenuFadeIn();

	do
	{
		which = HandleMenu(&MusicItems,&MusicMenu[start],NULL);
		if (which>=0)
		{
#ifdef WOLFDOSMPU
			if (start == 0)
			{
				start = (which + 1) * 6;
				MusicItems.curpos = 0;
				DrawWindow (18,49,292,13*7,BKGDCOLOR);
			}
			else if (which == 5)
			{
				MusicItems.curpos = start / 6 - 1;
				start = 0;
				DrawWindow (18,49,292,13*7,BKGDCOLOR);
			}
			else
			{
				if (lastsong != -1)
					MusicMenu[lastsong].active = 1;
				lastsong = start + which;
				StartCPMusic(MusicTracks[lastsong]);
				MusicMenu[lastsong].active = 2;
			}
			DrawMenu (&MusicItems,&MusicMenu[start]);
			VW_UpdateScreen();
#else  // WOLFDOSMPU
			if (lastsong >= 0)
				MusicMenu[start+lastsong].active = 1;

			StartCPMusic(songs[start + which]);
			MusicMenu[start+which].active = 2;
			DrawMenu (&MusicItems,&MusicMenu[start]);
			VW_UpdateScreen();
			lastsong = which;
#endif // WOLFDOSMPU
		}
	} while(which>=0);

	MenuFadeOut();
	IN_ClearKeysDown();
#ifdef SPEAR
	UnCacheLump (BACKDROP_LUMP_START,BACKDROP_LUMP_END);
#else
	UnCacheLump (CONTROLS_LUMP_START,CONTROLS_LUMP_END);
#endif
}
#endif
#ifdef WOLFDOSMPU
#pragma warn .aus
#endif // WOLFDOSMPU


/*
==========================
=
= InitGame
=
= Load a few things right away
=
==========================
*/

void InitGame (void)
{
	int                     i,x,y;
	unsigned        *blockstart;
#ifdef WOLFDOSMPU
	int skipSignon = 0;
#endif // WOLFDOSMPU

	if (MS_CheckParm ("virtual"))
		virtualreality = true;
	else
		virtualreality = false;

#ifdef WOLFDOSMPU
	mpuInit();
#endif // WOLFDOSMPU

	MM_Startup ();                  // so the signon screen can be freed

	SignonScreen ();

	VW_Startup ();
	IN_Startup ();
	PM_Startup ();
	PM_UnlockMainMem ();
	SD_Startup ();
	CA_Startup ();
	US_Startup ();


#ifdef WOLFDOSMPU
	if (mminfo.mainmem < MAINMEMSIZE + BUFFERSIZE && ! nomain)		// additional memory needed for automap
#else  // WOLFDOSMPU
#ifndef SPEAR
	if (mminfo.mainmem < 235000L)
#else
	if (mminfo.mainmem < 257000L && !MS_CheckParm("debugmode"))
#endif
#endif // WOLFDOSMPU
	{
		memptr screen;

		CA_CacheGrChunk (ERRORSCREEN);
		screen = grsegs[ERRORSCREEN];
		ShutdownId();
		movedata ((unsigned)screen,7+7*160,0xb800,0,17*160);
		gotoxy (1,23);
		exit(1);
	}


//
// build some tables
//
	InitDigiMap ();

	for (i=0;i<MAPSIZE;i++)
	{
		nearmapylookup[i] = &tilemap[0][0]+MAPSIZE*i;
		farmapylookup[i] = i*64;
	}

	for (i=0;i<PORTTILESHIGH;i++)
		uwidthtable[i] = UPDATEWIDE*i;

	blockstart = &blockstarts[0];
	for (y=0;y<UPDATEHIGH;y++)
		for (x=0;x<UPDATEWIDE;x++)
			*blockstart++ = SCREENWIDTH*16*y+x*TILEWIDTH;

	updateptr = &update[0];

	bufferofs = 0;
	displayofs = 0;
	ReadConfig ();


//
// HOLDING DOWN 'M' KEY?
//
#ifndef SPEARDEMO
	if (Keyboard[sc_M])
#ifdef WOLFDOSMPU
	{
		skipSignon = 1;
#endif // WOLFDOSMPU
	  DoJukebox();
#ifdef WOLFDOSMPU
	}
#endif // WOLFDOSMPU
	else
#endif
//
// draw intro screen stuff
//
	if (!virtualreality)
		IntroScreen ();

//
// load in and lock down some basic chunks
//

	CA_CacheGrChunk(STARTFONT);
	MM_SetLock (&grsegs[STARTFONT],true);

	LoadLatchMem ();
	BuildTables ();          // trig tables
	SetupWalls ();

#if 0
{
int temp,i;
temp = viewsize;
	profilehandle = open("SCALERS.TXT", O_CREAT | O_WRONLY | O_TEXT);
for (i=1;i<20;i++)
	NewViewSize(i);
viewsize = temp;
close(profilehandle);
}
#endif

#ifdef WOLFDOSMPU
	// shrinking screens fix
	savedviewsize = viewsize;
#endif // WOLFDOSMPU

	NewViewSize (viewsize);


//
// initialize variables
//
	InitRedShifts ();
	if (!virtualreality)
#ifdef WOLFDOSMPU
	if (! skipSignon)
#endif // WOLFDOSMPU
		FinishSignon();

	displayofs = PAGE1START;
	bufferofs = PAGE2START;

	if (virtualreality)
	{
		NoWait = true;
		geninterrupt(0x60);
	}
}

//===========================================================================

/*
==========================
=
= SetViewSize
=
==========================
*/

boolean SetViewSize (unsigned width, unsigned height)
{
	viewwidth = width&~15;                  // must be divisable by 16
	viewheight = height&~1;                 // must be even
	centerx = viewwidth/2-1;
	shootdelta = viewwidth/10;
	screenofs = ((200-STATUSLINES-viewheight)/2*SCREENWIDTH+(320-viewwidth)/8);

//
// calculate trace angles and projection constants
//
	CalcProjection (FOCALLENGTH);

//
// build all needed compiled scalers
//
//	MM_BombOnError (false);
	SetupScaling (viewwidth*1.5);
#if 0
	MM_BombOnError (true);
	if (mmerror)
	{
		Quit ("Can't build scalers!");
		mmerror = false;
		return false;
	}
#endif
	return true;
}


void ShowViewSize (int width)
{
	int     oldwidth,oldheight;

	oldwidth = viewwidth;
	oldheight = viewheight;

	viewwidth = width*16;
	viewheight = width*16*HEIGHTRATIO;
	DrawPlayBorder ();

	viewheight = oldheight;
	viewwidth = oldwidth;
}


void NewViewSize (int width)
{
	CA_UpLevel ();
	MM_SortMem ();
	viewsize = width;
	SetViewSize (width*16,width*16*HEIGHTRATIO);
	CA_DownLevel ();
}



//===========================================================================

/*
==========================
=
= Quit
=
==========================
*/

void Quit (char *error)
{
	unsigned        finscreen;
	memptr	screen;

#ifdef WOLFDOSMPU
#ifdef MEMDEBUG
	// save the error to a file
	if (error)
		LogMemory(error);
#endif // MEMDEBUG
	if (compflags == 0x8000)
	{
		// error happened before subsystem initialization finished
		ShutdownId();
		puts(error);
		exit(1);
	}
#endif // WOLFDOSMPU

	if (virtualreality)
		geninterrupt(0x61);

	ClearMemory ();
	if (!*error)
	{
	 #ifndef JAPAN
	 CA_CacheGrChunk (ORDERSCREEN);
	 screen = grsegs[ORDERSCREEN];
	 #endif
	 WriteConfig ();
	}
	else
	{
	 CA_CacheGrChunk (ERRORSCREEN);
	 screen = grsegs[ERRORSCREEN];
	}

	ShutdownId ();

	if (error && *error)
	{
	  movedata ((unsigned)screen,7,0xb800,0,7*160);
	  gotoxy (10,4);
	  puts(error);
	  gotoxy (1,8);
	  exit(1);
	}
	else
	if (!error || !(*error))
	{
		clrscr();
		#ifndef JAPAN
		movedata ((unsigned)screen,7,0xb800,0,4000);
		gotoxy(1,24);
		#endif
//asm	mov	bh,0
//asm	mov	dh,23	// row
//asm	mov	dl,0	// collumn
//asm	mov ah,2
//asm	int	0x10
	}

	exit(0);
}

//===========================================================================



/*
=====================
=
= DemoLoop
=
=====================
*/

static  char *ParmStrings[] = {"baby","easy","normal","hard",""};

void    DemoLoop (void)
{
	static int LastDemo;
	int     i,level;
	long nsize;
	memptr	nullblock;

//
// check for launch from ted
//
	if (tedlevel)
	{
		NoWait = true;
		NewGame(1,0);

		for (i = 1;i < _argc;i++)
		{
			if ( (level = US_CheckParm(_argv[i],ParmStrings)) != -1)
			{
			 gamestate.difficulty=level;
			 break;
			}
		}

#ifndef SPEAR
		gamestate.episode = tedlevelnum/10;
		gamestate.mapon = tedlevelnum%10;
#else
		gamestate.episode = 0;
		gamestate.mapon = tedlevelnum;
#endif
		GameLoop();
		Quit (NULL);
	}


//
// main game cycle
//


//	nsize = (long)40*1024;
//	MM_GetPtr(&nullblock,nsize);

#ifndef DEMOTEST

/*
	#ifndef UPLOAD

		#ifndef GOODTIMES
		#ifndef SPEAR
		#ifndef JAPAN
		if (!NoWait)
			NonShareware();
		#endif
		#else

			#ifndef GOODTIMES
			#ifndef SPEARDEMO
			CopyProtection();
			#endif
			#endif

		#endif
		#endif
	#endif
*/

	StartCPMusic(INTROSONG);

#ifndef JAPAN
	if (!NoWait)
		PG13 ();
#endif

#endif

	while (1)
	{
		while (!NoWait)
		{
//
// title page
//
			MM_SortMem ();
#ifndef DEMOTEST

#ifdef SPEAR
			CA_CacheGrChunk (TITLEPALETTE);

			CA_CacheGrChunk (TITLE1PIC);
			VWB_DrawPic (0,0,TITLE1PIC);
			UNCACHEGRCHUNK (TITLE1PIC);

			CA_CacheGrChunk (TITLE2PIC);
			VWB_DrawPic (0,80,TITLE2PIC);
			UNCACHEGRCHUNK (TITLE2PIC);
			VW_UpdateScreen ();
			VL_FadeIn(0,255,grsegs[TITLEPALETTE],30);

			UNCACHEGRCHUNK (TITLEPALETTE);
#else
			CA_CacheScreen (TITLEPIC);
			VW_UpdateScreen ();
			VW_FadeIn();
#endif
			if (IN_UserInput(TickBase*15))
				break;
			VW_FadeOut();
//
// credits page
//
			CA_CacheScreen (CREDITSPIC);
			VW_UpdateScreen();
			VW_FadeIn ();
			if (IN_UserInput(TickBase*10))
				break;
			VW_FadeOut ();
//
// high scores
//
			DrawHighScores ();
			VW_UpdateScreen ();
			VW_FadeIn ();

			if (IN_UserInput(TickBase*10))
				break;
#endif
//
// demo
//

			#ifndef SPEARDEMO
			PlayDemo (LastDemo++%4);
			#else
/*
			PlayDemo (0);
*/
			PlayDemo (LastDemo);
			#endif

			if (playstate == ex_abort)
				break;
			StartCPMusic(INTROSONG);
		}

		VW_FadeOut ();

#ifndef SPEAR
		if (Keyboard[sc_Tab] && MS_CheckParm("goobers"))
#else
		if (Keyboard[sc_Tab] && MS_CheckParm("debugmode"))
#endif
			RecordDemo ();
		else
			US_ControlPanel (0);

		if (startgame || loadedgame)
		{
			GameLoop ();
			VW_FadeOut();
			StartCPMusic(INTROSONG);
		}
	}
}


//===========================================================================


/*
==========================
=
= main
=
==========================
*/

char    *nosprtxt[] = {"nospr",nil};

void main (void)
{
	int     i;


#ifdef BETA
	//
	// THIS IS FOR BETA ONLY!
	//
	struct dosdate_t d;

	_dos_getdate(&d);
	if (d.year > YEAR ||
		(d.month >= MONTH && d.day >= DAY))
	{
	 printf("Sorry, BETA-TESTING is over. Thanks for you help.\n");
	 exit(1);
	}
#endif

	CheckForEpisodes();

	Patch386 ();

	InitGame ();

	DemoLoop();

	Quit("Demo loop exited???");
}

