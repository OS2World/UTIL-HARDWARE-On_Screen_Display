//
//  PMTPOSD2 On-Screen-Display utility for (most) IBM ThinkPads
//           showing volume-setting, mute, LCD-brightness and
//           monitor selection external-VGA/LCD or Dual (both)
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
//  The PMTOSD2 package is considered OPEN SOURCE, you may use it as you
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
//  ===========================================================================
//
#define OSD_V "1.09 2009-09-18" // Made 2nd 'stealth' TPSTH dialog popup
//efine OSD_V "1.09 2009-09-15" // Textual updates, better usage wording
//efine OSD_V "1.08 2005-12-23" // Added stealth parameter, no dialog at start
//efine OSD_V "1.07 2005-11-16" // Reduced title-length to below 40 (API restr)
//efine OSD_V "1.06 2005-11-14" // Quick response, default sampling 90 msec now
//efine OSD_V "1.06 2005-11-14" // Sharpened bitmap; TPOSD ICON in HELP dialog
//efine OSD_V "1.05 2005-11-12" // Give audible feedback on volume changes
//efine OSD_V "1.05 2005-11-11" // Replaced DLG text by bitmap, better scaling
//efine OSD_V "1.04 2005-11-09" // Added usage popup on -? and versioning defs
//efine OSD_V "1.04 2005-11-09" // F3 while dialog up quits program completely
//efine OSD_V "1.04 2005-11-09" // Parameter 2: nr of msec between BIOS sampling
//efine OSD_V "1.03 2005-11-06" // Parameter 1: nr of seconds dialog stays up
//efine OSD_V "1.03 2005-11-05" // Fix partial dialog display by size increase
//efine OSD_V "1.03 2005-11-05" // Added OSD activation using 'Access IBM' key
//efine OSD_V "1.02 2005-11-04" // Combined Brightness/Volumes in same display
//efine OSD_V "1.02 2005-11-04" // Created PMTPOSD2 package, as open source
//efine OSD_V "1.02 2005-11-03" // Fixed spurious popups due to BIOS 'spikes'
//efine OSD_V "1.01 2005-11-01" // Moved dialog to lower center of the screen
//efine OSD_V "1.01 2005-10-31" // No dialog timeout as long as changes are made
//efine OSD_V "1.01 2005-10-31" // Added display of LCD/VGA/DUAL monitor setting
//efine OSD_V "1.00 2005-10-30" // Fix shutdown-hang by calling CancelShutdown()
//
#define OSD_M "PMTPOSD2"
#define OSD_N "ThinkPad On Screen Display utility"
#define OSD_C "(c) 2009  Jan van Wijk"
//
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define INCL_DOS
#define INCL_PM
#include <os2.h>

#include "iopl32.h"
#include "pmtposd2.h"

#define CMOS_NONE      0xffffffff

static HBITMAP         icon_vol;
static HBITMAP         icon_scr;
static HBITMAP         bar_off;
static HBITMAP         vbar_on;
static HBITMAP         bbar_on;
static HBITMAP         copyright;
static ULONG           id_timer;
static HAB             hab;
static BOOL            audible  = TRUE;         // give audible feedback
static ULONG           timeout;
static ULONG           showtime = 15;           // default showtime 3 seconds
static ULONG           sampling = 150;          // default sampling 150 msec
static ULONG           old      = CMOS_NONE;    // previous cmos state
static char            usage[1000];             // usage text buffer

#define resetTIMEOUT {if (timeout < showtime) timeout = showtime;}

#define iMUTING(x) ((x & 0x00000040)      )     // boolean value 0 or other
#define iVOLUME(x) ((x & 0x0000000f)      )     // 4 bits value  0 .. 0xE
#define iBRIGHT(x) ((x & 0x00000700) >>  8)     // 3 bits value  0 .. 7
#define iDISPLY(x) ((x & 0x00030000) >> 16)     // 2 bits value  1 .. 3

// constants defining absolute/relative positions within dialog
#define PMTP_ICON_LEFT    2
#define PMTP_ICON_RIGHT 376
#define PMTP_ICON_YPOS   34
#define PMTP_BAR_XPOS    62
#define PMTP_BAR_VYPOS   30
#define PMTP_BAR_BYPOS   64
#define PMTP_BAR_XDELTA  22

#ifdef __WATCOMC__
 #define _outp outp
 #define _inp  inp
#endif

/*****************************************************************************/
// Read contents of one CMOS byte, using port input/output
/*****************************************************************************/
ULONG pmtpCmosByte                              // RET   CMOS byte
(
   ULONG               id                       // IN    byte offset
)
{
   _outp(        0x70, id);                     // select byte to read
   return ( _inp(0x71) & 0xFF);                 // restrict to single byte
}
/*---------------------------------------------------------------------------*/

/*****************************************************************************/
// Read contents of relevant CMOS bytes/bits into a single ULONG value
/*****************************************************************************/
ULONG pmtpGetCmos                               // RET   32 bits TP BIOS info
(
   void
)
{
   ULONG               rc = 0;                  // note: any bit change in the
                                                // ULONG will activate the OSD
   enterIOPL32();
   rc  = ((pmtpCmosByte( CMOS_BYTE_PWMISC) &0x08) << 24); // Access IBM key
   rc += ((pmtpCmosByte( CMOS_BYTE_DISPLY) &0x03) << 16); // Display selection
   rc += ((pmtpCmosByte( CMOS_BYTE_BRIGHT) &0x27) <<  8); // Brightness  +toggle
   rc += ((pmtpCmosByte( CMOS_BYTE_VOLUME) &0xcf)      ); // Volume/mute +toggle
   leaveIOPL32();
   return( rc);
}
/*---------------------------------------------------------------------------*/


/*****************************************************************************/
// Present 'about' and usage information to the user
/*****************************************************************************/
void pmtpAboutHelp
(
)
{
   MB2INFO             mbox;

   mbox.cb        = sizeof( MB2INFO);
   mbox.cButtons  = 1;                          // just one button
   mbox.hIcon     = WinLoadPointer( HWND_DESKTOP, 0, ID_ICON);
   mbox.flStyle   = MB_CUSTOMICON | MB_MOVEABLE;
   mbox.mb2d[0].idButton = 10;
   mbox.mb2d[0].flStyle  = BS_PUSHBUTTON | BS_DEFAULT;
   strcpy( mbox.mb2d[0].achText, " Close the ThinkPad On Screen Display"
                                 " 'About' and usage help dialog ");

   WinMessageBox2( HWND_DESKTOP, HWND_DESKTOP, usage, OSD_N, 0, &mbox);

}                                               // end 'pmtpAboutHelp'
/*---------------------------------------------------------------------------*/


/*****************************************************************************/
// pmtp specific window procedure, handling the main OSD dialog events
/*****************************************************************************/
MRESULT EXPENTRY pmtpWinProc
(
   HWND                hwnd,                    // handle of window
   ULONG               msg,                     // id of message
   MPARAM              mp1,                     // first message parameter
   MPARAM              mp2                      // second message parameter
)
{
   HPS                 hps;
   POINTL              ptl;
   ULONG               i;
   ULONG               tpc;

   switch(msg)
   {
      case WM_INITDLG:
         timeout = showtime;                    // set dialog showtime
         hps = WinGetPS(hwnd);
         tpc = pmtpGetCmos();

         switch (iMUTING(tpc))                  // set volume related bitmaps
         {
            case 0:  icon_vol = GpiLoadBitmap( hps, NULLHANDLE, ID_SPEAKER, 0L,0L); break;
            default: icon_vol = GpiLoadBitmap( hps, NULLHANDLE, ID_SPK_OFF, 0L,0L); break;
         }

         switch (iDISPLY(tpc))                  // set display related bitmaps
         {
            case 1:  icon_scr = GpiLoadBitmap( hps, NULLHANDLE, ID_SCRLCD,  0L,0L); break;
            case 2:  icon_scr = GpiLoadBitmap( hps, NULLHANDLE, ID_SCRVGA,  0L,0L); break;
            default: icon_scr = GpiLoadBitmap( hps, NULLHANDLE, ID_SCRDUAL, 0L,0L); break;
         }
         bar_off   = GpiLoadBitmap( hps, NULLHANDLE, ID_BAR_LEDOFF, 0L,0L);
         vbar_on   = GpiLoadBitmap( hps, NULLHANDLE, ID_VOL_LEDON,  0L,0L);
         bbar_on   = GpiLoadBitmap( hps, NULLHANDLE, ID_SCR_LEDON,  0L,0L);
         copyright = GpiLoadBitmap( hps, NULLHANDLE, ID_COPYRIGHT,  0L,0L);

         WinReleasePS(hps);

         //- Take about 5 samples per second while dialog is up
         id_timer = WinStartTimer( hab, hwnd, ID_TIMER, 180L);
         break;

      case WM_TIMER:
         hps = WinGetPS(hwnd);
         WinInvalidateRegion(hwnd,NULLHANDLE,0);
         tpc = pmtpGetCmos();

         if (icon_vol != 0) GpiDeleteBitmap( icon_vol);
         switch (iMUTING(tpc))                  // set volume related icons
         {
            case 0:  icon_vol = GpiLoadBitmap( hps, NULLHANDLE, ID_SPEAKER, 0L,0L); break;
            default: icon_vol = GpiLoadBitmap( hps, NULLHANDLE, ID_SPK_OFF, 0L,0L); break;
         }
         if (icon_scr != 0) GpiDeleteBitmap( icon_scr);
         switch (iDISPLY(tpc))                  // set display related icons
         {
            case 1:  icon_scr = GpiLoadBitmap( hps, NULLHANDLE, ID_SCRLCD,  0L,0L); break;
            case 2:  icon_scr = GpiLoadBitmap( hps, NULLHANDLE, ID_SCRVGA,  0L,0L); break;
            default: icon_scr = GpiLoadBitmap( hps, NULLHANDLE, ID_SCRDUAL, 0L,0L); break;
         }
         WinReleasePS(hps);

         if (old == tpc)                        // no changes, keep counting
         {
            timeout--;
            if (timeout == 0)                   // and end dialog at timeout
            {
               WinStopTimer(  hab, (HWND) 0, id_timer);
               WinDismissDlg( hwnd,TRUE);
            }
         }
         else                                   // reset timeout after changes
         {
            if (audible && (old != 0) && (iVOLUME(tpc) != iVOLUME(old)))
            {
               WinAlarm( HWND_DESKTOP, WA_WARNING); // audible volume feedback
            }
            resetTIMEOUT;                       // set full showtime again
            old = tpc;                          // set previous cmos state
         }
         break;

      case WM_PAINT:
         tpc = pmtpGetCmos();
         hps = WinBeginPaint(hwnd, (ULONG) 0, 0);

         ptl.x = 1;
         ptl.y = 1;
         WinDrawBitmap( hps, copyright, NULL,&ptl,0L,0L,DBM_NORMAL);

         ptl.x = PMTP_ICON_LEFT;
         ptl.y = PMTP_ICON_YPOS;
         WinDrawBitmap( hps, icon_vol,  NULL,&ptl,0L,0L,DBM_NORMAL);

         ptl.x = PMTP_BAR_XPOS;
         ptl.y = PMTP_BAR_VYPOS;
         for(i = 0; i < iVOLUME(tpc); i++)
         {
            WinDrawBitmap( hps, vbar_on, NULL,&ptl,0L,0L,DBM_NORMAL);
            ptl.x += PMTP_BAR_XDELTA;
         }
         for(i = iVOLUME(tpc); i < 14; i++)
         {
            WinDrawBitmap( hps, bar_off, NULL,&ptl,0L,0L,DBM_NORMAL);
            ptl.x += PMTP_BAR_XDELTA;
         }

         ptl.x = PMTP_BAR_XPOS;
         ptl.y = PMTP_BAR_BYPOS;
         for(i = 0; i < iBRIGHT(tpc); i++)
         {
            WinDrawBitmap( hps, bbar_on, NULL,&ptl,0L,0L,DBM_NORMAL);
            ptl.x += PMTP_BAR_XDELTA;
            WinDrawBitmap( hps, bbar_on, NULL,&ptl,0L,0L,DBM_NORMAL);
            ptl.x += PMTP_BAR_XDELTA;
         }
         for(i = iBRIGHT(tpc); i < 7; i++)
         {
            WinDrawBitmap( hps, bar_off, NULL,&ptl,0L,0L,DBM_NORMAL);
            ptl.x += PMTP_BAR_XDELTA;
            WinDrawBitmap( hps, bar_off, NULL,&ptl,0L,0L,DBM_NORMAL);
            ptl.x += PMTP_BAR_XDELTA;
         }

         ptl.x = PMTP_ICON_RIGHT;
         ptl.y = PMTP_ICON_YPOS;
         WinDrawBitmap( hps, icon_scr, NULL,&ptl,0L,0L,DBM_NORMAL);

         WinEndPaint(hwnd);
         break;

      case WM_CHAR:
         if (SHORT1FROMMP(mp1) & KC_VIRTUALKEY)
         {
            switch (SHORT2FROMMP(mp2))
            {
               case VK_F1:
                  timeout = (ULONG) -1;         // no timeout for OSD dialog
                  pmtpAboutHelp();
                  timeout = showtime;           // set dialog showtime
                  break;

               case VK_F3:
                  WinDismissDlg( hwnd, DID_CANCEL); // signal cancel/quit to main
                  break;

               case VK_F4:
                  WinAlarm( HWND_DESKTOP, WA_NOTE); // explicit audible feedback
                  resetTIMEOUT;                 // set full showtime again
                  break;

               case VK_F5:
                  audible = FALSE;
                  resetTIMEOUT;                 // set full showtime again
                  break;

               case VK_F6:
                  audible = TRUE;
                  resetTIMEOUT;                 // set full showtime again
                  break;

               default:
                  return WinDefDlgProc(hwnd, msg, mp1, mp2);
                  break;
            }
         }
         else
         {
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
         }
         break;

      default:
         return WinDefDlgProc(hwnd, msg, mp1, mp2);
         break;
   }
   return WinDefDlgProc(hwnd,msg,mp1,mp2);
}
/*---------------------------------------------------------------------------*/


int main(int argc,char **argv)
{
   HMQ                 hmq;
   ULONG               first;
   ULONG               last;
   BOOL                help    = FALSE;
   BOOL                stealth = FALSE;
   char               *exename = strrchr( argv[0], '\\');

   if (exename++ == NULL)                       // no path separator in name
   {
      exename = argv[0];                        // so use full name
   }
   hab = WinInitialize(0);
   hmq = WinCreateMsgQueue(hab,0);

   sprintf( usage, "%s %s %s\n\n"
           "Some parts copyright 2003 by Serge Sterck (PMTPOSD)\n\n"
           "Usage:\n\n%s [show [sample [audio [stealth]]]]\n\n"
           " show   \t= Nr of seconds OSD stays up, default 3 sec\n"
           " sample \t= msec between BIOS samples, default 150 msec\n"
           " audio  \t : 0 is audio feedback OFF, 1 = ON; default ON\n"
           " stealth\t : 1 is stealth mode  OFF, 0 = ON; default OFF\n"
           " Example:\t%s  2  50  1  0\n\n"
           " F1\tpresents this 'about' and 'usage' help\n"
           " F3\tquits the program completely\n"
           " F4\tgives a single audible feedback sound\n"
           " F5\tswitches automatic audible feedback off\n"
           " F6\tswitches automatic audible feedback on\n\n"
           "(Fkeys are not operational from this help dialog)\n\n",
              OSD_M, OSD_V, OSD_C, exename, exename);

   if (argc > 1)                                // some parameter ...
   {
      if (!isdigit( argv[1][0]))                // invalid param or help/usage
      {
         help = TRUE;                           // just usage help
      }
      else                                      // param 1: interval other
      {                                         // than default of 15 (3 sec)
         showtime = (atol(argv[1]) * 5) +2;     // with a minimum of 0.4 sec

         if (argc > 2)                          // param 2: sample time other
         {                                      // than default of 150 msec
            sampling = max( atol(argv[2]), 30); // minimum will be 30 msec to
                                                // avoid pegging the CPU :-)
            if (argc > 3)
            {
               audible = atol(argv[3]);         // 0 or text set quiet, other
                                                // values enable sound
               if (argc > 4)
               {
                  stealth = !atol(argv[4]);     // 0 or text, show dialog at startup
               }                                // at startup, other values stealth
            }
         }
      }
   }

   if (help)                                    // usage text prepared ...
   {
      pmtpAboutHelp();
   }
   else                                         // start OSD display & monitor
   {
      WinCancelShutdown( hmq, TRUE);            // avoid hanging shutdown; JvW
      last = pmtpGetCmos();                     // read initial last sample
      do
      {
         if (stealth && (old == CMOS_NONE))     // start in stealth mode
         {
            old = last;                         // make cmos status match
         }
         else
         {
            // to be refined, try to keep program out of tasklist somehow
            // if the stealth parameter is set to ON
            if (WinDlgBox( HWND_DESKTOP, HWND_DESKTOP, // show dialog, updates 'old'
                     (PFNWP) pmtpWinProc, (HMODULE) 0, // dedicated window procedure
                      (stealth) ? ID_TPSTH : ID_TPOSD, // Stealth or Normal dialog
                                  NULL) == DID_CANCEL) // quit the program when the
            {                                          // dialog is canceled using F3
               break;
            }
         }
         last = old;
         do                                     // wait until real value change
         {
            first = last;                       // propagate last sample
            DosSleep( sampling);                // bridge typical BIOS spike
            last  = pmtpGetCmos();              // and read next sample
         } while ((last != first) || (last == old));
      } while (old != CMOS_NONE);               // forever, until Dlg Cancel
   }
   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
   return 0;
}
/*---------------------------------------------------------------------------*/

