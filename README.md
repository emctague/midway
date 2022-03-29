# midway

A small C++ library for MIDI input on macOS and Win32.

Uses system APIs on both platforms (Windows Multimedia, Core MIDI).

Consider this **an experiment**, not to be used in production - though it may be a useful reference if you want to use
the APIs underlying it. 

## Project Goals
 - Be simple to use
 - Behave predictably on every platform
 - Bare minimum support for note on/off events

## Current Challenges

Differences in behaviour or unclear/missing functionality in underlying platform APIs comes with some challenges:

 - Detecting MIDI device connect+disconnect without polling seems to be inconsistent or not available
 - On macOS, we can uniquely identify a MIDI device between connections/usages, but not on Win32, where devices are
   apparently identified only by the order they were connected to the system.
 - On Win32, we need to be able to provide a free-able handle for a given device connection, but on macOS each device is
   connected to a central object and does not have to be freed itself.