# MaskinProgrammering03

## Docker
We have a dockerfile that is able to do everything from building to flashing for us. It pulls from another dockerfile we created earlier which contains a finished kernel.
Flashing is dependent on the --privileged flag to work. This will fail silently on mac, since docker on mac runs on a lighter virtualization level, thereby disabling USB interfacing. It is possible to enable USB access on mac by installing old versions of docker. Windows is not tested, but it should work in theory.

The first time building the image will take some time, since it has to pull a large amount of files. After this is done the data is cached, and building is a breeze.

## How to run
To build and flash the currently modified files that exist in the local-src folder, first install docker, then follow these steps

1. Enter a terminal and change directory to git repo root directory
2. Ensure that the development board is connected through USB, and confirm it is powered on. It must be ready for connection at the time of running the next command, otherwise only the build will be run, but the flashing will fail
3. Think of a cool name for your docker image, for example "cybertruck". Note that image names has some restrictions, such as that the letters must be lowercase.
4. Run the command `$ docker build -t cybertruck . && docker run --privileged cybertruck`

## How to run interactive
Follow steps 1-3 from previous paragraph, then follow these steps:

1. Run the command `$ docker build -t cybertruck .`, this only builds the image.
2. Run the command `$ docker run --privileged -it cybertruck /bin/bash`, this puts you in interactive shell inside a docker container built from the docker image. Its an ubuntu 14.04 image, so standard commands from that distro is availble, including apt-get. Just beware that whatever you do in here will not be persisted when exiting the container. For persistent storage, you have to set up volumes.
3. Run the command `$ su non-root`, this changes user to a user that is created for running PTXdist commands, it is also placed in the sudoers file if that is neccecary. But dont run PTXdist as sudo, because PTXdist refuses to do that.
4. Run the command `$ cd`, this changes directory to the non-root user directory, where all the relevant files are. 
5. At this point, the folder MaskinProgrammering03 contains a finished kernel, with updated files under the local-src folder. We have created helper scripts inside a subfolder of the OSELAS toolchain folder (they must be run from the OSELAS folder for them to work), but just running PTXdist commands as non-root work as well. The main use case for the interactive approach is to rebuild the entire thing, which can be done with `ptxdist distclean`, and then `ptxdist images`. Just remember that all data is lost upon exiting the container, no matter what is done inside the interactive shell.

## Connecting to the board
We have included the miniterm.py program in our repo for connecting to the board. It requires a python2 depencency `pyserial` for it to work, which should be pretty straightforward to install from the pip package manager for python2. When it is set up, and the board is connected and running in AEM mode, you can run the command  `python2 miniterm.py -b 115200 -p /dev/ttyUSB0 | tee -a output.txt` to connect to the board and capture all output to the output.txt file.
