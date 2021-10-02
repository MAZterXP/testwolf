@echo off
if not "%2"=="" goto :proceed
echo Specify two .MAP files to compare.
goto :exit

:proceed
echo ___
grep "DATASEG" %1 | head -c6 > tmp1
for /f "tokens=*" %%f in (tmp1) do grep "^ %%f" %1 | cut -d: -f2- | sort | uniq > %1.tmp
del tmp1
grep "DATASEG" %2 | head -c6 > tmp2
for /f "tokens=*" %%f in (tmp2) do grep "^ %%f" %2 | cut -d: -f2- | sort | uniq > %2.tmp
del tmp2
diff -u %1.tmp %2.tmp
del %1.tmp %2.tmp

:exit