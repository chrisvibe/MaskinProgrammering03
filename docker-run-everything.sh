docker build -t ex3-image .
docker run --mount source=myvol2,target=/home/non-root/MaskinProgrammering03 --privileged ex3-image