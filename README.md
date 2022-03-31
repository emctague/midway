# midway

A small C++ library for MIDI input on macOS and Win32.

Uses system APIs on both platforms (Windows Multimedia, Core MIDI).

Consider this **an experiment**, not to be used in production - though it may be a useful reference if you want to use
the APIs underlying it. 

## Project Goals
 - Be simple to use
 - Behave predictably on every platform
 - Bare minimum support for note on/off events

## Current Limitations

 - We cannot get the device manufacturer name on Win32.
 - We cannot identify the source device for a played note.
