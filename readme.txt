
On Screen Display utility for most ThinkPad laptops


Main program PMTPOSD2.EXE
=========================

A Utility to display the current settings for
several ThinkPad BIOS related settings like:

 - Speaker volume (with audible feedback)
 - Speaker muting
 - LCD brightness
 - Monitor selection VGA/LCD/DUAL

Pops up a small dialog at the bottom/center of
the screen with the relevant info any time one
of the monitored values changes, usually due
to the user operating a special ThinkPad key.


Note to ACPI driver users
=========================
When using the new ACPI drivers (as present in eCS 2.x)
instead of the classic APM support, this utility will
NOT function at all since the ACPI drivers intercept
the events that make the changes to the ThinkPad BIOS.
This may or may not be fixed in later ACPI releases ...


Installation
============

Just copy the PMTPOSD2.EXE together with
the IOPL32.DLL to a suitable directory.

The utility should be started at system startup,
either with a start command from STARTUP.CMD:

        start  pmtposd2

Or by placing a program-object in the startup folder.

If you want to replace/update a previous version, you
can stop the current version by activating the pop up,
with the "ThinkPad" or "Access IBM" key for example,
and then use <F3> to terminate the program.
When closing it using <F3>, be aware that you must be
quick when the default time of 3 seconds is in effect.
Best to check after quiting if it is really gone by
trying the "Access IBM" or one of the other buttons ...


Activation and usage
====================

Activation using the "ThinkPad" / "Access IBM" key or
any of the keys that change monitored values like:

 - Volume-up/down and mute
 - <Fn>+<F7> to toggle screen selection
 - <Fn>+<PgUp> or <PgDn> to set brightness (on most)
   <Fn>+<Home> or <End>  on some others like T31
   where the <Fn>+<PgUp> controls the Thinklight

   Note: It seems to be poorly documented that <Fn>+<F8>
         toggles font/display size for full-screen text
         display like BIOS startup screen, real DOS and
         OS2 full screen VIO screens.


Some function keys may be used when the OSD dialog is up:

  - F1 presents a small 'about' and 'usage' help
  - F3 quits the program completely
  - F4 gives a single audible feedback sound
  - F5 switches automatic audible feedback off
  - F6 switches automatic audible feedback on


Note:
The function keys only work when the help/about dialog is
closed, so using them while reading the help does not work.


When no changes are detected for a given period, the
dialog will go away, but the program stays active.



Optional parameters, startup usage:


   ThinkPad On Screen Display utility for OS/2 and eComStation

   Usage:  pmtposd2  [show  [sample  [audio [stealth]]]]

     show    = Nr of seconds dialog stays up, default 3 sec
     sample  = msec between BIOS samples, default 360 msec
     audio   = 0 is audio feedback OFF, 1 = ON; default ON
               Supply audible feedback on speaker volume
               When specifying a non-zero value, sound is on,
               zero or text will turn audible feedback off
     stealth = 1 is stealth mode  OFF, 0 = ON; default OFF
               No dialog display on startup of the program
               and NO presence in task-list or Alt-Tab list!
               A non-zero value activates stealth mode, zero
               or text will display the dialog at startup


The 'show' time is the time the dialog will stay visible without
any change taking place. Suit to your own taste ...

The 'sample' time sets the minimum time in msec a certain change
in monitored BIOS values must exist before the popup is activated.
This avoids spurious popups when no real changes are made, which
seems to happen fairly often on my T42 without this ...
It also delays the reaction-time for the popup with the same value,
so don't make it too long. For nearly instant reaction set the
value to 0, but be aware you may get 'false popups'.

The 'audio' parameter, when set to 0, stops the program
from giving audible feedback when changing the speaker volume.
It is on by default, if you listen to music on the ThinkPad often
you may want to switch it off, and use the music itself as feedback.
Not that the 'audible feedback' uses the default PM alarm sound,
which should usually be a system .WAV file being played ...

You can either switch the audible feedback off by specifying the
'audio' parameter at startup, or using the <F4> function-key while
the dialog is up.

At any time the dialog is up, you can check the actual volume by
using the <F5> function key, which generates a notification beep.


The 'stealth' parameter, which can be any value, causes the program
to start without displaying the dialog with all current settings.
It also keeps the program (popup and help) OUT of the tasklist and
and other related ones like lSwitcher and eCenter, as well as the
Alt-Tab quick switch mechanism. This may make it hard to give it
focus again when you lost that! (close windows until visible :-)


Example: 'pmtposd2  5 360 quiet stealth'
or:      'pmtposd2  5 360 0 0'

Will leave the popup stay for 5 seconds after the last change, use
360 msec sampling, NO audible feedback and start without a dialog.


Example: 'pmtposd2  2 0'

Will leave the popup stay for just 2 seconds after the last change,
use the shortest sampling (30 msec), audible feedback and start
with displaying the dialog with the initial values shown.


Test program PMTPTEST.EXE
=========================

This is a simple text-mode program that will display a few of
the relevant BIOS bytes used by PMTPOSD2. Useful to test on
different ThinkPads, and figure out new functionality :-)

It displays a timestamp, msec since start of the program
plus 6 values for the BIOS bytes being monitored.

Simply start in a windowed OS2 command environment,
and end with Ctrl-C when you get bored ...


Sources
=======
All sources to build the program are available in the SRC
subdirectory after unzipping the distribution archive.
It also contains a screenshot of version 1.05 showing
what the OSD with the help popup should look like ...


Contributors
============

Original program is by Serge Sterck (PMTPOSD.ZIP) in 2003
with a simpler dialog and causing a shutdown hang ...

Some of that work is based on information by JMA
(www.jma.se)

All updates to that so far by Jan van Wijk, Fsys Software

Other useful information and feedback from:

- Rainer Stroebel, user interface and functionality feedback
- David Tholen, user interface feedback and activation
- Marko Udvanc, with info on additional ThinkPad BIOS bytes
- Christian Langanke for requesting the stealth startup
- Andre Doff, with some textul updates and minor change requests

I am sure I am missing a few here, if you feel
you should be in this list, let me know :-)

