bs3x
====
This is a repository for the WaveIdea Bitstream 3X software.

Practical info
==============
There are 2 CPUs in the bs3x
One is called EZ : it is dedicated to the USB and bootloader
One is called Z8 : dedicated to bs3x features

EZ is connected to the USB, Sync-24, and MIDI connectors
Z8 is communicating with EZ to send/receive MIDI.

There are thus 2 firmware, one for EZ, one for Z8, plus a common part
all attached.

Tools used to compile are : 
- ZDS II for Z8 Encore! (zilog, http://www.zilog.com/index.php?option=com_product&task=iframe) (free)
- Keil uvision 3 (https://www.keil.com/demo/eval/c51.htm) (paid)

License
=======
Software provided as is for use with Bitstream 3X only.
Software written by Jerome Dumas.
For further licensing information please contact:
Wave Idea
3 rue Frédéric Sauvage
ZI Ecopolis Sud
13500 Martigues
France
