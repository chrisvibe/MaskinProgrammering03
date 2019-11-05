su - non-root -c "git clone https://github.com/rubensseva/OpenMaskin3.git"

# Configure ptxdist with platform, toolchain etc
su - non-root -c "cd ~/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko && ptxdist select configs/ptxconfig"
su - non-root -c "cd ~/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko && ptxdist platform configs/platform-energymicro-efm32gg-dk3750/platformconfig"
su - non-root -c "cd ~/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko && ptxdist toolchain /opt/OSELAS.Toolchain-2012.12.0/arm-cortexm3-uclinuxeabi/gcc-4.7.2-uclibc-0.9.33.2-binutils-2.22-kernel-3.6-sanitized/bin"

# THIS IS EXTREMELY SKETCKY
# A file is built by busybox with an undefined reference to usleep, but we cant
# reach that files before we build... So we try to build, ignore errors, fix the 
# file, then build again. The line that is fixed is just a usleep in a while true
# loop, probably just to not consume so much resources sooooo should be OK to remove...?
su - non-root -c "cd ~/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko && ptxdist images; exit 0"
su - non-root -c "sed -i '/usleep/d' /home/non-root/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko/platform-energymicro-efm32gg-dk3750/build-target/busybox-1.21.0/modutils/modprobe-small.c"
su - non-root -c "cd ~/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko && ptxdist images"

