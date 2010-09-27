/* blacken-cmap --- sets all unallocated color cells to black.
   By Jamie Zawinski <jwz@mcom.com>

   this is useful for debugging color allocation problems; run "xcmap"
   to get a view of your colormap, and then run "blacken-cmap -loop"
   in the background.  As applications free their color cells, those
   cells will turn black instead of simply staying the color they were
   before.  So if you still see colors in the map that you don't think
   should be there, you know someone has not properly freed their colors.

   Really it would be a lot more convenient if this was just an option
   to xcmap.

   This works by allocating writable color cells until it can't allocate
   any more; setting them to black; then freeing them all again.  So
   there's a small window of opportunity where it could interfere with
   color allocation of other programs (since it momentarily fills the
   colormap once every three seconds.)  Maybe it should grab the server
   to close this window.

   cc -o blacken-cmap blacken-cmap.c -lX11
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#define USAGE "usage: %s [ -loop | -id cmap_id | -white ]\n"

void
main (argc, argv)
     int argc;
     char **argv;
{
  Display *dpy;
  Colormap cmap = 0;
  int loop_p = 0;
  int pixels_size = 256;
  unsigned short shade = 0;
  int i;
  unsigned long *pixels = (unsigned long *)
    malloc (sizeof (unsigned long) * pixels_size);

  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "-loop"))
	{
	  loop_p = 1;
	}
      else if (!strcmp (argv[i], "-white"))
	{
	  shade = ~0;
	}
      else if (!strcmp (argv[i], "-black"))
	{
	  shade = 0;
	}
      else if (!strcmp (argv[i], "-id") && i != argc-1)
	{
	  char c;
	  i++;
	  if ((1 != sscanf (argv[i], " %d %c", &cmap, &c)) &&
	      (1 != sscanf (argv[i], " 0x%x %c", &cmap, &c)))
	    {
	      fprintf (stderr, "%s: cmap id %s unparsable\n",
		       argv[0], argv[i]);
	      exit (1);
	    }
	}
      else
	{
	  fprintf (stderr, USAGE, argv[0]);
	  exit (1);
	}
    }

  dpy = XOpenDisplay (0);
  if (! cmap)
    cmap = DefaultColormap (dpy, DefaultScreen (dpy));

  do
    {
      XColor color;
      int i = 0;
      int clear_count = 0;
      while (XAllocColorCells (dpy, cmap, False, 0, 0, &color.pixel, 1))
	{
	  if (i >= (pixels_size - 1))
	    {
	      pixels_size *= 2;
	      pixels = (unsigned long *)
		realloc (pixels, sizeof (unsigned long) * pixels_size);
	    }
	  pixels [i] = color.pixel;
	  i++;
	  XQueryColor (dpy, cmap, &color);
	  if (color.red != shade ||
	      color.green != shade ||
	      color.blue != shade)
	    clear_count++;
	  color.red = color.green = color.blue = shade;
	  color.flags = DoRed|DoGreen|DoBlue;
	  XStoreColor (dpy, cmap, &color);
	}
      XSync (dpy, False);
      XFreeColors (dpy, cmap, pixels, i, 0);
      XSync (dpy, False);
      if (clear_count)
	{
	  printf ("cleared %d cell%s\n", clear_count,
		  (clear_count == 1 ? "" : "s"));
	  fflush (stdout);
	}
      if (loop_p)
	sleep (3);
    }
  while (loop_p);
  exit (0);
}
