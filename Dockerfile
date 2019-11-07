FROM rubensseva/maskiner03:develop

COPY --chown=non-root . /home/non-root/MaskinProgrammering03/

RUN echo $' \n\
  cd MaskinProgrammering03/OSELAS.BSP-EnergyMicro-Gecko \n\
  bash TDT4258_helper_scripts/ptxdist_setup.sh \n\
  bash TDT4258_helper_scripts/ptxdist_build_all.sh \n\
  bash TDT4258_helper_scripts/ptxdist_flash_all.sh' >> /home/non-root/docker_pull_and_setup.sh

# Output the script for debugging purposes
RUN cat /home/non-root/docker_pull_and_setup.sh
# Give all persmissions to script
RUN chmod 777 /home/non-root/docker_pull_and_setup.sh


# On docker run command, run the script we created earlier
CMD su - non-root -c "bash /home/non-root/docker_pull_and_setup.sh"

