@ECHO OFF

REM Command file for publishing to pypi

set DISTDIR=dist
set SETUPFILE=setup.py
set USERNAME=RickyMyo

py %SETUPFILE% sdist
twine upload %DISTDIR%/*