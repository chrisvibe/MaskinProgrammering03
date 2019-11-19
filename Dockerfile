FROM rubensseva/maskiner3finishedkernel

COPY --chown=non-root . /home/non-root/MaskinProgrammering03LocalCopy/

RUN su - non-root -c "rsync -av /home/non-root/MaskinProgrammering03LocalCopy/OSELAS.BSP-EnergyMicro-Gecko/local_src/ /home/non-root/MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko/local_src/"

