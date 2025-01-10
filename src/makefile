CC = gcc
CCFLAGS = -std=gnu99 -fgnu89-inline -O2 -s -ffast-math -fomit-frame-pointer
#CCFLAGS = -g -std=gnu99 -fgnu89-inline -Wall
LDFLAGS = `allegro-config --libs` -laldmb -ldumb -lm
FILES = anim.c blending.c game.c lighting.c main.c map.c masking.c menu.c particle.c pyramid.c scroller.c
EDITOR = map.c editor.c

all: paciana editor

paciana: $(FILES)
	$(CC) $(CCFLAGS) -o paciana $(FILES) $(LDFLAGS)

editor: $(EDITOR)
	$(CC) $(CCFLAGS) -o editor $(EDITOR) $(LDFLAGS)

clean:
	rm paciana
	rm editor
# DO NOT DELETE

anim.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/allegro.h
anim.o: /usr/include/allegro/base.h /usr/include/errno.h
anim.o: /usr/include/features.h /usr/include/stdc-predef.h
anim.o: /usr/include/limits.h /usr/include/time.h /usr/include/string.h
anim.o: /usr/include/strings.h /usr/include/allegro/internal/alconfig.h
anim.o: /usr/include/allegro/platform/alplatf.h
anim.o: /usr/include/allegro/platform/alucfg.h /usr/include/fcntl.h
anim.o: /usr/include/unistd.h /usr/include/allegro/platform/alunixac.h
anim.o: /usr/include/allegro/platform/astdint.h /usr/include/inttypes.h
anim.o: /usr/include/stdint.h /usr/include/allegro/system.h
anim.o: /usr/include/allegro/base.h /usr/include/allegro/unicode.h
anim.o: /usr/include/allegro/config.h /usr/include/allegro/inline/system.inl
anim.o: /usr/include/allegro/debug.h /usr/include/allegro/unicode.h
anim.o: /usr/include/allegro/mouse.h /usr/include/allegro/timer.h
anim.o: /usr/include/allegro/keyboard.h /usr/include/allegro/joystick.h
anim.o: /usr/include/allegro/palette.h /usr/include/allegro/gfx.h
anim.o: /usr/include/allegro/3d.h /usr/include/allegro/fixed.h
anim.o: /usr/include/allegro/inline/gfx.inl
anim.o: /usr/include/allegro/inline/asm.inl /usr/include/allegro/color.h
anim.o: /usr/include/allegro/palette.h /usr/include/allegro/inline/color.inl
anim.o: /usr/include/allegro/draw.h /usr/include/allegro/gfx.h
anim.o: /usr/include/allegro/inline/draw.inl /usr/include/allegro/3d.h
anim.o: /usr/include/allegro/inline/gfx.inl /usr/include/allegro/rle.h
anim.o: /usr/include/allegro/inline/rle.inl /usr/include/allegro/compiled.h
anim.o: /usr/include/allegro/text.h /usr/include/allegro/font.h
anim.o: /usr/include/allegro/fli.h /usr/include/allegro/file.h
anim.o: /usr/include/allegro/config.h /usr/include/allegro/gui.h
anim.o: /usr/include/allegro/sound.h /usr/include/allegro/digi.h
anim.o: /usr/include/allegro/stream.h /usr/include/allegro/midi.h
anim.o: /usr/include/allegro/file.h /usr/include/allegro/lzss.h
anim.o: /usr/include/allegro/datafile.h /usr/include/allegro/fixed.h
anim.o: /usr/include/allegro/fmaths.h /usr/include/allegro/inline/fmaths.inl
anim.o: /usr/include/allegro/matrix.h /usr/include/allegro/fmaths.h
anim.o: /usr/include/allegro/inline/matrix.inl /usr/include/allegro/quat.h
anim.o: /usr/include/allegro/3dmaths.h
anim.o: /usr/include/allegro/inline/3dmaths.inl
anim.o: /usr/include/allegro/alcompat.h
anim.o: /usr/include/allegro/platform/alunix.h anim.h game.h map.h
anim.o: /usr/include/aldumb.h /usr/include/allegro.h /usr/include/dumb.h
anim.o: lost.h blending.h lighting.h menu.h scroller.h particle.h
blending.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/allegro.h
blending.o: blending.h
editor.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/allegro.h
editor.o: /usr/include/allegro/internal/aintern.h editor.h map.h lost.h
game.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/allegro.h
game.o: /usr/include/aldumb.h /usr/include/dumb.h /usr/include/math.h game.h
game.o: map.h lighting.h menu.h lost.h anim.h particle.h blending.h
lighting.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/allegro.h
lighting.o: lighting.h blending.h game.h map.h /usr/include/aldumb.h
lighting.o: /usr/include/dumb.h lost.h masking.h
main.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/allegro.h
main.o: game.h map.h /usr/include/aldumb.h /usr/include/dumb.h lost.h menu.h
main.o: anim.h
map.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/allegro.h
map.o: game.h map.h /usr/include/aldumb.h /usr/include/dumb.h lost.h
masking.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/allegro.h
masking.o: masking.h
menu.o: /usr/include/stdio.h /usr/include/allegro.h menu.h game.h map.h
menu.o: /usr/include/aldumb.h /usr/include/dumb.h /usr/include/stdlib.h
menu.o: anim.h pyramid.h scroller.h lost.h
particle.o: /usr/include/allegro.h /usr/include/string.h
particle.o: /usr/include/strings.h /usr/include/features.h
particle.o: /usr/include/stdc-predef.h /usr/include/math.h particle.h map.h
particle.o: game.h /usr/include/aldumb.h /usr/include/dumb.h
particle.o: /usr/include/stdlib.h /usr/include/stdio.h lost.h blending.h
pyramid.o: /usr/include/stdio.h /usr/include/allegro.h pyramid.h game.h map.h
pyramid.o: /usr/include/aldumb.h /usr/include/dumb.h /usr/include/stdlib.h
pyramid.o: lost.h
scroller.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/ctype.h
scroller.o: /usr/include/features.h /usr/include/stdc-predef.h
scroller.o: /usr/include/allegro.h scroller.h
