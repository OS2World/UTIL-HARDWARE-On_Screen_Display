#
# Makefile for use with Openwatcom, requires WATCOM environment variable to be set
#

cc     = wcc386
cflags = -i=$(%watcom)\h;$(%watcom)\h\os2 -w4 -e25 -sg -zq -od -d2 -6s -bt=os2 -mf

.c.obj : .AUTODEPEND $(gendep)
      $(cc) $[*.c $(cflags)

gendep = makefile

bmpdep = pmtpdual.bmp pmtplcd.bmp  pmtpvga.bmp  &
         pmtplson.bmp pmtploff.bmp pmtplvon.bmp &
         pmtpsoff.bmp pmtpson.bmp  pmtposd2.bmp

all : ..\pmtposd2.exe ..\pmtptest.exe

..\pmtposd2.exe : pmtposd2.obj iopl32.lib pmtposd2.res  $(gendep)
 @%write pmtposd2.lk1 FIL pmtposd2.obj
 @%append pmtposd2.lk1 LIBR iopl32.lib
 *wlib -q -n -b pmtposd2.imp
 @%append pmtposd2.lk1 LIBR pmtposd2.imp
 *wlink name ..\pmtposd2 SYS os2v2 pm op m op maxe=25 op q @pmtposd2.lk1
 rc  -i $(%watcom)\h\os2 pmtposd2.res ..\pmtposd2.exe


pmtposd2.res : pmtposd2.rc pmtposd2.dlg $(gendep) $(bmpdep)
 rc  -i $(%watcom)\h\os2 -r pmtposd2.rc pmtposd2.res

..\pmtptest.exe : pmtptest.obj iopl32.lib  $(gendep)
 @%write  pmtptest.lk1 FIL pmtptest.obj
 @%append pmtptest.lk1 LIBR iopl32.lib
 *wlib -q -n -b pmtptest.imp
 @%append pmtposd2.lk1 LIBR pmtptest.imp
 *wlink name ..\pmtptest SYS os2v2 @pmtptest.lk1

clean : .SYMBOLIC
        @if exist *.obj del *.obj
        @if exist *.lk1 del *.lk1
        @if exist *.map del *.map
        @if exist *.res del *.res
        @if exist *.sym del *.sym
        @if exist *.err del *.err
