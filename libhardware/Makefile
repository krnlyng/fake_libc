CFLAGS  = -Wall -Werror -std=c99 -fPIC -mhard-float -I/usr/lib/droid-devel/droid-headers
LDFLAGS = -ldl -shared -landroid_namespace
LDLIBS  =
OUT     = libhardware.so
OBJ     = hardware.o

.PHONY: release debug clean

release: CFLAGS := $(CFLAGS) -O2
release: $(OUT)

debug:   CFLAGS := $(CFLAGS) -O0 -g3 -ggdb -pg
debug:   $(OUT)

clean:
	$(RM) $(OBJ) $(OBJ:.o=.d) $(OUT)

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c %.d
	$(CC) $(CFLAGS) -c $< -o $@

%.d: %.c
	$(CC) $(CFLAGS) -MF $@.tmp -MM $<
	@sed -e 's|.*:|$*.o:|' < $@.tmp > $@
	@rm -rf $*.d.tmp

ifneq ($(MAKECMDGOALS),clean)
	-include $(OBJ:.o=.d)
endif
