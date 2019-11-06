# MaskinProgrammering03


## Docker
Vi har en dockerfil som basically gjør alt av bygging og flashing. Eneste kravet er at plasseringen til dockerfilen ikke endres i repoet. Hvis man setter en remote på repoet, vil dockerfilen også pulle nye endringer fra master, så lenge repot er public.

### Oppsett med helper scripts
Det er to helper scripts for å kjøre docker, disse legger til persistent storage så ptxdist blir optimalisert. Scriptene kjører docker i privileged mode for JLink og USB tilkobling, som er en svakhet med tanke på sikkerhet, men det funker :) 

* `docker-run-everything`: Dette scriptet gjør alt autmatisk med persistent storage. Den bygger også docker file før den kjører, så første gang kan ta en stund, men etter det går det fort
* `docker-run-interacte`: Dette scriptet setter deg inn i ett bash shell, men med samme persisten storage som det forrige script. Det gjør at du først kan kjøre docker-run-everything, også evt inspisere filene med dette scriptet.

### Oppsett uten helper scripts
Hvis du f.eks. vil kalle image `test-image`, så kan du kjøre
* `$ docker build -t test-image .` for å builde image og
* `$ docker run --privileged test-image` for å kjøre conainer med image (men uten bash shell og persisten storage)
