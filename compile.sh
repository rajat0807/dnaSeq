chmod 755 *.html
chmod 755 *.php
chmod 755 *.h
chmod 755 *.c
gcc -m32 -g sequencefile.c statistics.c suffixtries.c bitmap.c graphics.c 3dgraphics.c entropicprofiles.c -lm -o ep
chmod 755 ep
