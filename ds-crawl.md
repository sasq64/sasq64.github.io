---
layout: page
title: "DS Crawl"
description: ""
---
{% include JB/setup %}
LINEY'S DUNGEON CRAWL - Nintendo DS version v1.0
================================================
by Sasq

![DSCrawl]({{ site.url }}/assets/crawl/shot001.png)
![DSCrawl]({{ site.url }}/assets/crawl/shot002.png)

![DSCrawl]({{ site.url }}/assets/crawl/shot003.png)
![DSCrawl]({{ site.url }}/assets/crawl/shot004.png)

QUICK INFO
----------

This is a port of Liney's Dungeon Crawl for the Nintendo DS.
It is not just a straight port, I have put some effort into formatting
the play area into windows fitting the DS layout, and tried to make
the game enjoyable using the buttons available on the NDS.

The game will load it's data from - and save itself to - a libfat compatible
memory card if available. This is the recommended way of playing.

If you are instead playing from a GBA cart, it will use the SRAM there to
save, and load its data from within the executable (this means you can't
easily change the settings in INIT.TXT for instance).

*NOTE* However - The SRAM is pretty small, and even though I compress the
saves, it will most likely not be enough to finish the entire game. So
backup your game if you start reaching the lower levels (say 15-20 or so).

*HINT* You can press Select at the init screen to check the contents of
SRAM. If the total ever goes above 65536 bytes, you are screwed.


INSTALLING
----------
Copy all files (The NDS, the BMP:s and INIT.TXT) to a directory called
CRAWL, or to the root directory of your memory card. Launch the NDS.

The included NDS-file is compiled with the Rein version of the fat
library. If it doesn't work for you you need to download another
version and use that instead.
"rein" seems to be recommended for SD-cards, and also works on MP2.
"chishm" is the original chishm routines (works fine on MP2).
"green", "blue" and "gray" are the 3 versions of libfatdragon - google
it for list of what they support.

*OR*

Burn the ds.gba-file on your GBA-cart and launch.



GAMEPLAY
--------
Most actions are performed by holding a direction on the directon pad and
pressing one of A,B,X,Y. This gives you access to 9*4 shortcuts, which have
reasonable defaults but can be configured from INIT.TXT.

The four blue squares under the Text matrix shows what will happen if you
press one of the corresponding four buttons. It will change appearance as
you fiddle with the direction pad, so it's easy to see which shortcuts are
available and use them.

When you perform specific actions, like targeting,
the shortcuts will change and lock to a predefined set of keys, and
the directional pad will work as usual.

In menues, the shortcuts also locks and you can move up and down in the
menu with the directional pad as expected, and select an choice with
the 'A' button.



MOVEMENT
--------
Hold a direction and press A to move in that direction. This works better
than simply using the dirpad, since diagonals are easier to do, and it
is easier to click A repeatadly than a direction. Also, A repeats so you
can hold it down to move continously.
(Hand-to-hand combat is accomplished by simply moving towards an enemy.)
Hold L or R and press direction pad to run.

If "Smart Diagonals" are turned on (default) then moving straight into
a wall when the space on either side of the wall is open, will move there
instead (turning a straight movement into a diagonal one).
This makes going around bends and "pillar dancing" much easier.


KEYS
----
Left shoulder button acts as SHIFT (the same as pressing the
up-arrow at the lower left of the keyboard matrix).
Right shoulder button acts as CTRL - it is necessary in some
situations.

**General**

* SELECT = Inventory ('i', or '?' if at a prompt)
* Shift-SELECT = Escape
* START = Fullscreen map ('X')
* Shift-START Quit ('Q')
* Dirpad+A = Move in that direction
* DirPad+B,X,Y = Shortcuts

**In Menues**

* A = Select
* B = Cancel
* X = Skip between sections
* Y = SPACE ("More" - when menu doesnt fit one screen)
