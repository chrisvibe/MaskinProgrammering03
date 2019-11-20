FROM rubensseva/maskiner3finishedkernel

COPY --chown=non-root ./OSELAS.BSP-EnergyMicro-Gecko/local_src/ /home/non-root/MaskinProgrammering03LocalCopy/

RUN su - non-root -c "rsync -av /home/non-root/MaskinProgrammering03LocalCopy/ /home/non-root/MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko/local_src/"

CMD su - non-root -c "cd /home/non-root/MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko && sh TDT4258_helper_scripts/ptxdist_rebuild_flash_driver.sh"
