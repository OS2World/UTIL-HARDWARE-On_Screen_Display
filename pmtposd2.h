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

//- constants related to PM/resources
#define ID_TIMER                           1


#define ID_VOL_LEDON                      50
#define ID_BAR_LEDOFF                     51
#define ID_SPEAKER                        52
#define ID_SPK_OFF                        53
#define ID_SCRDUAL                        55
#define ID_SCRLCD                         56
#define ID_SCRVGA                         57
#define ID_SCR_LEDON                      58
#define ID_COPYRIGHT                      59

#define ID_ICON                           90

#define ID_TPOSD                         100    // Normal popup  (in tasklist)
#define ID_TPSTH                         101    // Stealth popup (no tasklist)


//- constants defining positions of the BIOS bytes needed
#define CMOS_BYTE_ACPOWR    0x40
#define CMOS_BYTE_HIBERN    0x42
#define CMOS_BYTE_SUSPND    0x45
#define CMOS_BYTE_PWMODE    0x47
#define CMOS_BYTE_INETKY    0x64
#define CMOS_BYTE_PWMISC    0x65
#define CMOS_BYTE_TLIGHT    0x66
#define CMOS_BYTE_DISPLY    0x67
#define CMOS_BYTE_BRIGHT    0x6c
#define CMOS_BYTE_VOLUME    0x6E

