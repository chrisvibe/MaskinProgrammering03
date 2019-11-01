#!/bin/bash
# Use the following commands to do a minimal rebuild and flash for the driver:
ptxdist clean driver-gamepad
ptxdist compile driver-gamepad
ptxdist targetinstall driver-gamepad
ptxdist targetinstall kernel
ptxdist image root.romfs
ptxdist test flash-rootfs
