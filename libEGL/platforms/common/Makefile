OUT         := libhybris-eglplatformcommon.so
GEN_HDR		:= wayland-android-server-protocol.h wayland-android-client-protocol.h
GEN_SRC		:= wayland-android-protocol.c
SRC         := $(GEN_SRC) native_handle.c wayland-egl.c \
                eglplatformcommon.cpp nativewindowbase.cpp server_wlegl.cpp \
			    server_wlegl_buffer.cpp	server_wlegl_handle.cpp \
			    windowbuffer.cpp \
				../fbdev/fbdev_window.cpp \
			   	../hwcomposer/hwcomposer_window.cpp \
			   	../wayland/wayland_window.cpp

OBJ         := $(patsubst %.c, %.o, $(filter %.c, $(SRC)))
OBJ         += $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))
DEP         := $(OBJ:.o=.d)

WAYLAND_SCANNER := wayland-scanner

CFLAGS      := -Wall -std=c99 -mhard-float -fPIC -I/usr/lib/droid-devel/droid-headers -I../../ -I.
CXXFLAGS    := -Wall -mhard-float -fPIC -I/usr/lib/droid-devel/droid-headers -I../../ -I.
LDFLAGS     := -shared
LDLIBS      := -ldl -lwayland-server -lwayland-client

DEBUG       ?= 0
VERBOSE     ?= 0

ifeq ($(DEBUG),1)
	CFLAGS += -O0 -g3 -ggdb -pg -DDEBUG=1
	CXXFLAGS += -O0 -g3 -ggdb -pg -DDEBUG=1
	LDFLAGS += -pg
endif

ifeq ($(VERBOSE),1)
	MSG := @true
	CMD :=
else
	MSG := @echo
	CMD := @
endif

.PHONY: release clean install

release: CFLAGS += -O3
release: CXXFLAGS += -O3
release: $(OUT)

clean:
	$(MSG) -e "\tCLEAN\t"
	$(CMD)$(RM) $(OBJ) $(DEP) $(GEN_HDR) $(GEN_SRC) $(OUT)

$(OUT): $(OBJ)
	$(MSG) -e "\tLINK\t$@"
	$(CMD)$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%-protocol.c: %.xml
	$(MSG) -e "\tWAYLAND_SCANNER\t$@"
	$(CMD)$(WAYLAND_SCANNER) code < $< > $@

%-client-protocol.h : %.xml
	$(MSG) -e "\tWAYLAND_SCANNER(c)\t$@"
	$(CMD)$(WAYLAND_SCANNER) client-header < $< > $@

%-server-protocol.h : %.xml
	$(MSG) -e "\tWAYLAND_SCANNER(s)\t$@"
	$(CMD)$(WAYLAND_SCANNER) server-header < $< > $@

%.o: %.c $(GEN_HDR)
	$(MSG) -e "\tCC\t$@"
	$(CMD)$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp $(GEN_HDR)
	$(MSG) -e "\tCXX\t$@"
	$(CMD)$(CXX) $(CXXFLAGS) -c $< -o $@

install: $(OUT)
	install -d $(DESTDIR)/usr/bin/
	install -m 0755 $(OUT) $(DESTDIR)/usr/bin/
	install -m 0755 sparse/usr/bin/sfdroid_powerup $(DESTDIR)/usr/bin/
	install -m 0755 sparse/usr/bin/sfdroid_powerup_cm12.1 $(DESTDIR)/usr/bin/
	install -m 0755 sparse/usr/bin/am $(DESTDIR)/usr/bin/
	install -m 0755 sparse/usr/bin/sfdroid.sh $(DESTDIR)/usr/bin/
	install -d $(DESTDIR)/etc/dbus-1/system.d/
	install -m 0644 sparse/etc/dbus-1/system.d/sfdroid.conf $(DESTDIR)/etc/dbus-1/system.d/
	install -d $(DESTDIR)/etc/udev/rules.d/
	install -m 0644 sparse/etc/udev/rules.d/99-sfdroid-uinput.rules $(DESTDIR)/etc/udev/rules.d/
	install -d $(DESTDIR)/usr/share/applications/
	install -m 0644 sparse/usr/share/applications/*.desktop $(DESTDIR)/usr/share/applications/
	install -d $(DESTDIR)/usr/share/icons/hicolor/96x96/apps/
	install -m 0644 sparse/usr/share/icons/hicolor/96x96/apps/sfdroid.png $(DESTDIR)/usr/share/icons/hicolor/96x96/apps/

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif

