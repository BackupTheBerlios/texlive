taskkill /im:ispell.exe /f
rem p4 sync -f //depot/Master/texmf/...
rem rmdir /q/s "c:\program files\texlive\texmf"
rem mkdir "c:\program files\texlive\texmf"
rem xcopy c:\source\texlive\Master\texmf "c:\program files\texlive\texmf" /r/i/e/k
rem perl ./build.pl --distclean=tex
rem perl ./build.pl --distclean=xemtex
rem pushd .. & ( \source\fptex\gnu\diffutils-2.7\dynamic\diff.exe -ruN -X mswin32\diff.exclude -x "*/texk.development/*" c:/source/TeXLive/Build/source/TeX ./ > diffs\tl-source-21.diff ) & popd
rem pushd .. & ( \source\fptex\gnu\diffutils-2.7\dynamic\diff.exe -ruN -X mswin32\diff.exclude c:/source/TeXLive/Build/source.development/TeX/texk ./texk.development > diffs\tl-dev-21.diff ) & popd
rem pushd .. & ( \source\fptex\gnu\diffutils-2.7\dynamic\diff.exe -ruN -X mswin32\diff.exclude ./texk ./texk.development > diffs\dev-10.diff ) & popd
rem perl ./sync.pl --restrict c:/source/texlive/Master/texmf c:/progra~1/texlive/texmf
rem perl ./build.pl --build=tex --log=build.log
rem perl ./build.pl --build=xemtex --log=xemtex.log
rem perl ./build.pl --install=tex --log=install.log --from_scratch
updmap
fmtutil --all
perl ./build.pl --install=tex --log=install.log
perl ./build.pl --install=xemtex --log=xemtex-install.log
perl ./sync.pl c:/progra~1/texlive/bin/win32 c:/source/texlive/Master/bin/win32
perl ./sync.pl --restrict c:/progra~1/texlive/xemtex c:/source/texlive/Master/xemtex
perl ./sync.pl --restrict c:/progra~1/texlive/texmf c:/source/texlive/Master/texmf
perl ./p4update.pl --update --subdir=Master/bin/win32
perl ./p4update.pl --update --subdir=Master/texmf
perl ./p4update.pl --update --subdir=Master/xemtex
pushd \source\texlive\Master\Tools
rem perl ./tpm-factory.pl --clean --frompatterns --arch=all --all
perl ./tpm-factory.pl --clean --arch=all --all
perl ./tpm-factory.pl --check=cov --arch=all > \tmp\cov.log
perl ./tpm-factory.pl --tpm2zip --arch=win32 --all
popd

