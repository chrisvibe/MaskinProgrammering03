#!/bin/bash
# Minimal rebuild for the game
# While developing the game you do not need to re-flash all the images, only the root
# filesystem. This will speed the flashing process, and can be done by:
ptxdist clean game && \
ptxdist compile game && \
ptxdist targetinstall game && \
ptxdist image root.romfs && \
ptxdist test flash-rootfs
