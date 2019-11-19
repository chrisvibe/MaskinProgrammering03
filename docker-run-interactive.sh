docker build -t ex3-image .
docker run --mount source=myvol1,target=/home/non-root/MaskinProgrammering03 --privileged -it ex4-image /bin/bash
