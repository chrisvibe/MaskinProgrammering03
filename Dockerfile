FROM rubensseva/maskiner3finishedkernel

COPY --chown=non-root ./OSELAS.BSP-EnergyMicro-Gecko/local_src/ /home/non-root/MaskinProgrammering03LocalCopy/

RUN su - non-root -c "rsync -av /home/non-root/MaskinProgrammering03LocalCopy/ /home/non-root/MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko/local_src/"

RUN echo $'#!/usr/bin/env bash \n\
cd /home/non-root/MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko \n\
sh TDT4258_helper_scripts/ptxdist_rebuild_flash_driver.sh \n\ 
sh TDT4258_helper_scripts/ptxdist_rebuild_flash_game.sh' >> /home/non-root/build_modified.sh

RUN chown non-root /home/non-root/build_modified.sh
RUN chmod +x /home/non-root/build_modified.sh

CMD su - non-root -c "cd /home/non-root/MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko && sh /home/non-root/build_modified.sh"
