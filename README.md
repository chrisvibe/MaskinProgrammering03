# MaskinProgrammering03

## Docker
We have a dockerfile that is able to do everything from building to flashing for us. It pulls from another dockerfile containing all dependencies that we have uploaded to docker hub.

### Run with helper scripts
There are two helper scripts that runs docker. They only execute a few simple commands, but they might be used as reference if not directly executed. The scripts use persitent storage and runs in privileged mode for USB connection.

* `docker-run-everything`: This script sets up ptxdist and builds everything, and stores it in persistent storage. Take note of the name of the persistent storage, it is important that it is correct if you later want to run interactively with the same storage. This script should ideally be used once for setup, then the rest may be handled interactively (for only building drivers etc)
* `docker-run-interacte`: This script runs the docker image and then sets up a bash shell. When you are in the shell, you are initially logged in as root user. Ptxdist refuses to run as root, so there is another user we have named non-root for this purpose. You may change user to this user by running `su non-root` and then `cd` to get to the home directory of non-root. Most of the neccessary files are also in the home directory in non-root. There should also be some helper scripts for ptxdist under the BSP folder for setting up and running ptxdist. 

### Run without helper scripts
If you want to build with an image called `test-image`:
* `$ docker build -t test-image .` to build the image and 
* `$ docker run --privileged test-image` to run the image with USB connectivety (but without interactivity and persistent storage, see docker documentation or the docker helper scripts to add this functionality)
