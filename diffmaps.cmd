@echo off
gcc dsegmap\extract.c -Wall -o extract.exe
tail -n +5 dsegmap\spearac.map > spearac.map
tail -n +5 dsegmap\wolf3dac.map > wolf3dac.map
extract.exe < wolfsrc\spearcm.map | tail -n +5 > spearcm.map
extract.exe < wolfsrc\speardm.map | tail -n +5 > speardm.map
extract.exe < wolfsrc\wolf3dcm.map | tail -n +5 > wolf3dcm.map
extract.exe < wolfsrc\wolf3drm.map | tail -n +5 > wolf3drm.map
extract.exe < wolfsrc\wolf3dsm.map | tail -n +5 > wolf3dsm.map
diff spearac.map spearcm.map
diff spearac.map speardm.map
diff wolf3dac.map wolf3dcm.map
diff wolf3dac.map wolf3drm.map
diff wolf3dac.map wolf3dsm.map
extract.exe < wolfsrc\spearcw.map | tail -n +5 > spearcw.map
extract.exe < wolfsrc\speardw.map | tail -n +5 > speardw.map
extract.exe < wolfsrc\wolf3dcw.map | tail -n +5 > wolf3dcw.map
extract.exe < wolfsrc\wolf3drw.map | tail -n +5 > wolf3drw.map
extract.exe < wolfsrc\wolf3dsw.map | tail -n +5 > wolf3dsw.map
diff spearac.map spearcw.map
diff spearac.map speardw.map
diff wolf3dac.map wolf3dcw.map
diff wolf3dac.map wolf3drw.map
diff wolf3dac.map wolf3dsw.map
del *.map
del extract.exe