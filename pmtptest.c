// PMTPTEST Test program for PMTPOSD2, for (most) IBM ThinkPads
//          showing volume-setting, mute, LCD-brightness and
//          monitor selection external-VGA/LCD or Dual (both)
//
//  Original code Copyright (c) 2003 Serge Sterck, PMTPOSD.ZIP package
//  Enhancements and fixes: (c) 2005 Fsys Software and Jan van Wijk
//
//  =====================================================================
//
//  This package contains Original Code and/or Modifications of Original
//  Code as made available by Serge Sterck in his PMTPOSD.ZIP package.
//
//  It also derives from original work and info by JMA (www.jma.se),
//  Marko Udvanc as well as suggestions made by other people
//
//  The PMTOOSD2 package is considered OPEN SOURCE, you may use it as you
//  see fit, and make modifications, as long as the original copyright
//  statements are left intact and operational.
//
//  The Original Code and all software distributed in the pakcage are
//  distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND,
//  EITHER EXPRESS OR IMPLIED, AND FSYS-SOFTWARE AND ALL CONTRIBUTORS
//  HEREBY DISCLAIM ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION,
//  ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
//  QUIET ENJOYMENT OR NON-INFRINGEMENT.
//
//  =====================================================================
//
//
// JvW 2005-11-14 Reducing sampling delay to 20 msec (more accurate :-)
// JvW 2005-11-03 Added time-stamping to test BIOS spiking
// JvW 2005-11-01 Initial version, display all BIOS bytes
//
#include "stdio.h"
#include "stdlib.h"
#include "conio.h"
#include "time.h"
#include "iopl32.h"
#define INCL_DOS
#include "os2.h"
#include "pmtposd2.h"

typedef struct pmtp_cmos_bytes
{
   unsigned int  acpowr;
   unsigned int  hibern;
   unsigned int  suspnd;
   unsigned int  pwmode;
   unsigned int  inetky;
   unsigned int  pwmisc;
   unsigned int  tlight;
   unsigned int  displs;
   unsigned int  bright;
   unsigned int  volume;
} PMTP_CMOS_BYTES;                              // end of struct "pmpt_cmos_bytes"

#ifdef __WATCOMC__
 #define _outp outp
 #define _inp  inp
#endif
unsigned read_cmos(unsigned port)
{
   _outp(0x70,port);
   return ( _inp(0x71) & 0xFF);                 // mask to single byte
}

void get_cmos
(
   PMTP_CMOS_BYTES    *cmos                     // OUT   cmos bytes
)
{
   enterIOPL32();
   cmos->acpowr = read_cmos( CMOS_BYTE_ACPOWR);
   cmos->hibern = read_cmos( CMOS_BYTE_HIBERN);
   cmos->suspnd = read_cmos( CMOS_BYTE_SUSPND);
   cmos->pwmode = read_cmos( CMOS_BYTE_PWMODE);
   cmos->inetky = read_cmos( CMOS_BYTE_INETKY);
   cmos->pwmisc = read_cmos( CMOS_BYTE_PWMISC);
   cmos->tlight = read_cmos( CMOS_BYTE_TLIGHT);
   cmos->displs = read_cmos( CMOS_BYTE_DISPLY);
   cmos->bright = read_cmos( CMOS_BYTE_BRIGHT);
   cmos->volume = read_cmos( CMOS_BYTE_VOLUME);
   leaveIOPL32();
}
/*---------------------------------------------------------------------------*/


int main(int argc,char **argv)
{
   PMTP_CMOS_BYTES     old;
   PMTP_CMOS_BYTES     tpc;
   ULONG               sleeptime = 5;

   if (argc > 1)
   {
      sleeptime = atol( argv[1]);
   }

   get_cmos(&tpc);
   for(;;)                                      // do forever ...
   {
      printf( "%8.8lu ac:%2.2lx hib:%2.2lx sus:%2.2lx mode:%2.2lx inet:%2.2lx "
                     "misc:%2.2lx tl:%2.2lx disp:%2.2lx br:%2.2lx vol:%2.2lx\n",
             clock(), tpc.acpowr, tpc.hibern, tpc.suspnd, tpc.pwmode, tpc.inetky,
                      tpc.pwmisc, tpc.tlight, tpc.displs, tpc.bright, tpc.volume);

      DosSleep(sleeptime);

      old = tpc;                                // remember previous
      while (old.acpowr == tpc.acpowr &&
             old.hibern == tpc.hibern &&
             old.suspnd == tpc.suspnd &&
             old.pwmode == tpc.pwmode &&
             old.inetky == tpc.inetky &&
             old.pwmisc == tpc.pwmisc &&
             old.tlight == tpc.tlight &&
             old.displs == tpc.displs &&
             old.bright == tpc.bright &&
             old.volume == tpc.volume  )
      {
         DosSleep(sleeptime);
         get_cmos(&tpc);
      }
   }
   return 0;
}
