rem
rem Script to retrieve the fptex distribution
rem
rem Author: F. Popineau
rem Date: 2001/07/20
rem
rem fpTeX target: 0.5 series
rem
rem This script relies on the wget.exe utility
rem You can find this tool at
rem ftp://ftp.dante.de/pub/fptex/utilities/wget.exe
@echo off
wget -N -nH -np -t 0 --retr-symlinks ftp://ftp.dante.de/pub/fptex/current/TeXSetup.exe
wget -N -nH -np -t 0 --retr-symlinks ftp://ftp.dante.de/pub/fptex/current/tpm.zip
wget -N -nH -np -r -t 0 --cut-dirs=3 ftp://ftp.dante.de/pub/fptex/current/binary
wget -N -nH -np -r -t 0 --cut-dirs=3 ftp://ftp.dante.de/pub/fptex/current/collection
wget -N -nH -np -r -t 0 --cut-dirs=3 ftp://ftp.dante.de/pub/fptex/current/package
wget -N -nH -np -r -t 0 --cut-dirs=3 ftp://ftp.dante.de/pub/fptex/current/setupw32
wget -N -nH -np -r -t 0 --cut-dirs=3 ftp://ftp.dante.de/pub/fptex/current/support
:end
echo 'Done!'
