ifeq ($(PTX_KERNEL_DIR),)

all:
	@echo "please export PTX_KERNEL_DIR"

clean:
	rm -rf *.o *.ko *.kmod *.mod.c .*.cmd .*.o.d Module.symvers .tmp_versions modules.order

else

obj-m	:= driver-gamepad.o
CFLAGS_MODULE := -mlong-calls

modules modules_install clean:
	$(MAKE) -C $(PTX_KERNEL_DIR) M=$(CURDIR) V=$(V) $@

endif

# -------------------------------
# added this:
.PHONY : pretty
pretty :
	-indent *.c *.h

.PHONY : clean2
clean2 :
	-rm -rf *.o *.elf *.bin *.hex *.out
