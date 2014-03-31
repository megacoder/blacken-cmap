# BLACKEN-CMAP

A long time ago, and very far away, I wrote this as a gag to pwn a colleague of mine.  He had a very small office with no exterior windows.  I networked into his machine and fired off this program.  And then I waited.  After a few minutes, I walked by his office to see him with his lights out, leaning forward from his chair, trying desperately to read the dimming screen.

This program wakes up periodically and reads the current X11 screen RGB color map.  For each nonzero entry, it subtracts one.  The updated color map is then written back to the screen.

As a result, the screen slowly, slowly, ever so slowly, fades to black.

Mwahahaha!!
