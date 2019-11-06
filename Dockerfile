FROM ubuntu:14.04

# Install all dependencies
RUN apt-get update 
RUN apt-get install -y --force-yes build-essential expect gawk flex bison texinfo gettext libncurses-dev automake autoconf libtool pkg-config wget python python-dev python-setuptools  python-distutils-extra busybox bc git unzip bash


# Create user since ptxdist refuses to run as root
RUN apt-get install sudo
RUN useradd -m non-root
RUN echo "non-root ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Copy files from local repo folder
RUN su - non-root -c "mkdir /home/non-root/MaskinProgrammering03"
COPY --chown=non-root . /home/non-root/MaskinProgrammering03/


RUN ls /home/non-root/MaskinProgrammering03/
RUN unzip /home/non-root/MaskinProgrammering03/eACommander.zip
RUN mv /home/non-root/MaskinProgrammering03/eACommander /opt/
RUN ls /opt
RUN ls /opt/eACommander

# Get the right OSELAS toolchain from deb repo so we dont have to built it ourselves
RUN echo "deb http://debian.pengutronix.de/debian/ sid main contrib non-free" >> /etc/apt/sources.list
RUN apt-get update 
RUN apt-get install -y --force-yes oselas.toolchain-2012.12.0-arm-cortexm3-uclinuxeabi-gcc-4.7.2-uclibc-0.9.33.2-binutils-2.22-kernel-3.6-sanitized

# Install right ptxdist version
RUN wget --no-check-certificate http://ptxdist.de/software/ptxdist/download/ptxdist-2013.07.1.tar.bz2 && tar xjf ptxdist-2013.07.1.tar.bz2 && ls
RUN cd ptxdist-2013.07.1
RUN ls
RUN cd ptxdist-2013.07.1 && ./configure 
RUN cd ptxdist-2013.07.1 && make
RUN cd ptxdist-2013.07.1 && sudo make install

# Create script to be run, if OpenMaskin3 folder does not exist, clone it
# WARNING:
# A file is built by busybox with an undefined reference to usleep, but we cant
# reach that files before we build... So we try to build, ignore errors, fix the 
# file, then build again. The line that is fixed is just a usleep in a while true
# loop, probably just to not consume so much resources sooooo should be OK to remove...?
RUN echo $'#!/usr/bin/env bash \n\
  cd /home/non-root \n\
  [[ -d "MaskinProgrammering03" ]] || git clone https://github.com/chrisvibe/MaskinProgrammering03.git \n\
  ls \n\
  whoami \n\
  cd MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko \n\
  ptxdist select configs/ptxconfig \n\
  ptxdist platform configs/platform-energymicro-efm32gg-dk3750/platformconfig \n\
  ptxdist toolchain /opt/OSELAS.Toolchain-2012.12.0/arm-cortexm3-uclinuxeabi/gcc-4.7.2-uclibc-0.9.33.2-binutils-2.22-kernel-3.6-sanitized/bin \n\
  ptxdist images \n\
  sed -i '/usleep/d' /home/non-root/MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko/platform-energymicro-efm32gg-dk3750/build-target/busybox-1.21.0/modutils/modprobe-small.c \n\
  ptxdist images' >> /home/non-root/docker_pull_and_setup.sh


# Output the script for debugging purposes
RUN cat /home/non-root/docker_pull_and_setup.sh
# Give all persmissions to script
RUN chmod 777 /home/non-root/docker_pull_and_setup.sh


# On docker run command, run the script we created earlier
CMD su - non-root -c "bash /home/non-root/docker_pull_and_setup.sh"

