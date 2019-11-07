FROM rubensseva/maskiner03:develop

COPY --chown=non-root . /home/non-root/MaskinProgrammering03/
  
# Create script to be run, if OpenMaskin3 folder does not exist, clone it
# WARNING:
# A file is built by busybox with an undefined reference to usleep, but we cant
# reach that files before we build... So we try to build, ignore errors, fix the 
# file, then build again. The line that is fixed is just a usleep in a while true
# loop, probably just to not consume so much resources sooooo should be OK to remove...?
RUN echo $' \n\
  cd /home/non-root \n\
  ls \n\
  whoami \n\
  cd MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko \n\
  ptxdist select configs/ptxconfig \n\
  ptxdist platform configs/platform-energymicro-efm32gg-dk3750/platformconfig \n\
  ptxdist toolchain /opt/OSELAS.Toolchain-2012.12.0/arm-cortexm3-uclinuxeabi/gcc-4.7.2-uclibc-0.9.33.2-binutils-2.22-kernel-3.6-sanitized/bin \n\
  ptxdist images \n\
  sed -i '/usleep/d' /home/non-root/MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko/platform-energymicro-efm32gg-dk3750/build-target/busybox-1.21.0/modutils/modprobe-small.c \n\
  ptxdist images \n\
  ptxdist test flash-all' >> /home/non-root/docker_pull_and_setup.sh


# Output the script for debugging purposes
RUN cat /home/non-root/docker_pull_and_setup.sh
# Give all persmissions to script
RUN chmod 777 /home/non-root/docker_pull_and_setup.sh


# On docker run command, run the script we created earlier
CMD su - non-root -c "bash /home/non-root/docker_pull_and_setup.sh"

