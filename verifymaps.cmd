@echo off
rem Compare DATASEG layout of commercial versions
rem (Ignoring vanilla because we don't apply the "registered .WL6" savegame compatibility hack on vanilla)
call %~dp0\diffmaps.cmd %~dp0\WOLFSRC\WOLF3DCM.MAP %~dp0\WOLFSRC\WOLF3DCW.MAP

rem Compare DATASEG layout of registered versions
call %~dp0\diffmaps.cmd %~dp0\WOLFSRC\WOLF3DRV.MAP %~dp0\WOLFSRC\WOLF3DRM.MAP
call %~dp0\diffmaps.cmd %~dp0\WOLFSRC\WOLF3DRM.MAP %~dp0\WOLFSRC\WOLF3DRW.MAP

rem Compare DATASEG layout of shareware versions
call %~dp0\diffmaps.cmd %~dp0\WOLFSRC\WOLF3DSV.MAP %~dp0\WOLFSRC\WOLF3DSM.MAP
call %~dp0\diffmaps.cmd %~dp0\WOLFSRC\WOLF3DSM.MAP %~dp0\WOLFSRC\WOLF3DSW.MAP

rem Compare DATASEG layout of SoD commercial versions
call %~dp0\diffmaps.cmd %~dp0\WOLFSRC\SPEARCV.MAP %~dp0\WOLFSRC\SPEARCM.MAP
call %~dp0\diffmaps.cmd %~dp0\WOLFSRC\SPEARCM.MAP %~dp0\WOLFSRC\SPEARCW.MAP

rem Compare DATASEG layout of SoD demo versions
call %~dp0\diffmaps.cmd %~dp0\WOLFSRC\SPEARDV.MAP %~dp0\WOLFSRC\SPEARDM.MAP
call %~dp0\diffmaps.cmd %~dp0\WOLFSRC\SPEARDM.MAP %~dp0\WOLFSRC\SPEARDW.MAP

rem Compare DATASEG layout of commercial and registered versions
rem (only difference should be "idle" tag in _helpfilename)
call %~dp0\diffmaps.cmd %~dp0\WOLFSRC\WOLF3DCM.MAP %~dp0\WOLFSRC\WOLF3DRM.MAP
call %~dp0\diffmaps.cmd %~dp0\WOLFSRC\WOLF3DCW.MAP %~dp0\WOLFSRC\WOLF3DRW.MAP
