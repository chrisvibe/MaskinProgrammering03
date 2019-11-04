FROM ubuntu:14.04

# Get the right OSELAS toolchain from deb repo so we dont have to built it ourselves
RUN echo "deb http://debian.pengutronix.de/debian/ sid main contrib non-free" >> /etc/apt/sources.list
RUN apt-get update
RUN apt-cache search oselas
RUN apt-get install -y --force-yes oselas.toolchain-2012.12.0-arm-cortexm3-uclinuxeabi-gcc-4.7.2-uclibc-0.9.33.2-binutils-2.22-kernel-3.6-sanitized

# Create user since ptxdist refuses to run as root
RUN apt-get install sudo
RUN useradd -m non-root
RUN echo "non-root ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Install all dependencies
RUN apt-get install -y --force-yes build-essential expect gawk flex bison texinfo gettext libncurses-dev automake autoconf libtool pkg-config wget python python-dev python-setuptools  python-distutils-extra busybox bc git

# Install right ptxdist version
RUN wget --no-check-certificate http://ptxdist.de/software/ptxdist/download/ptxdist-2013.07.1.tar.bz2 && tar xjf ptxdist-2013.07.1.tar.bz2 && ls
RUN cd ptxdist-2013.07.1
RUN ls
RUN cd ptxdist-2013.07.1 && ./configure 
RUN cd ptxdist-2013.07.1 && make
RUN cd ptxdist-2013.07.1 && sudo make install

# Clone our repo
RUN su - non-root -c "git clone https://github.com/rubensseva/OpenMaskin3.git"

# Configure ptxdist with platform, toolchain etc
RUN su - non-root -c "cd ~/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko && ptxdist select configs/ptxconfig"
RUN su - non-root -c "cd ~/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko && ptxdist platform configs/platform-energymicro-efm32gg-dk3750/platformconfig"
RUN su - non-root -c "cd ~/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko && ptxdist toolchain /opt/OSELAS.Toolchain-2012.12.0/arm-cortexm3-uclinuxeabi/gcc-4.7.2-uclibc-0.9.33.2-binutils-2.22-kernel-3.6-sanitized/bin"

# THIS IS EXTREMELY SKETCKY
# A file is built by busybox with an undefined reference to usleep, but we cant
# reach that files before we build... So we try to build, ignore errors, fix the 
# file, then build again. The line that is fixed is just a usleep in a while true
# loop, probably just to not consume so much resources sooooo should be OK to remove...?
RUN su - non-root -c "cd ~/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko && ptxdist images; exit 0"
RUN su - non-root -c "sed -i '/usleep/d' /home/non-root/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko/platform-energymicro-efm32gg-dk3750/build-target/busybox-1.21.0/modutils/modprobe-small.c"
RUN su - non-root -c "cd ~/OpenMaskin3/OSELAS.BSP-EnergyMicro-Gecko && ptxdist images"

# Dont terminate so we can enter container and inspect 
# it with: docker run -t <name-of-image> /bin/bash
CMD tail -f /dev/null
