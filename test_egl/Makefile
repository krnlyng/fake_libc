OUT         := test_egl
SRC         := test_egl.c

OBJ         := $(patsubst %.c, %.o, $(filter %.c, $(SRC)))
OBJ         += $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))
OBJ         += $(patsubst %.s, %.o, $(filter %.s, $(SRC)))
DEP         := $(OBJ:.o=.d)

CFLAGS      := -Wall -std=c99 -mhard-float -fPIC -I../libEGL/platforms/common -I/usr/lib/droid-devel/droid-headers
CXXFLAGS    := -Wall -Werror -std=c++0x -mhard-float -fPIC -I../libEGL/platforms/common -I/usr/lib/droid-devel/droid-headers
LDFLAGS     :=
LDLIBS      := -lEGL

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

.PHONY: release clean

release: CFLAGS += -O3
release: CXXFLAGS += -O3
release: $(OUT)

clean:
	$(MSG) -e "\tCLEAN\t"
	$(CMD)$(RM) $(OBJ) $(DEP) $(OUT)

$(OUT): $(OBJ)
	$(MSG) -e "\tLINK\t$@"
	$(CMD)$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.s
	$(MSG) -e "\tAS\t$@"
	$(CMD)$(AS) -c $< -o $@

%.o: %.c %.d
	$(MSG) -e "\tCC\t$@"
	$(CMD)$(CC) $(CFLAGS) -c $< -o $@

%.d: %.c
	$(MSG) -e "\tDEP\t$@"
	$(CMD)$(CC) $(CFLAGS) -MF $@ -MM $<

%.o: %.cpp %.d
	$(MSG) -e "\tCXX\t$@"
	$(CMD)$(CXX) $(CXXFLAGS) -c $< -o $@

%.d: %.cpp
	$(MSG) -e "\tDEP\t$@"
	$(CMD)$(CXX) $(CXXFLAGS) -MF $@ -MM $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif

