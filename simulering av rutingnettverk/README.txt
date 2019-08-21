HVA SOM IKKE FUNGERER!
Overføring av rutingTabell til nodene. Jeg får til og sende rutingtabellen over TCP, men den blir ikke riktig til nodene.
Så TCP sender en ruting Tabell til hver node, men siden den ikke blir riktig bruker nodene en hardkodet ruting Tabell istedenfor.
Jeg har heller ikke fått implementert metoden som sjekker at alle kanter er blitt rapportert to ganger.

HVA SOM FUNGERER!
Sending av pakker med UDP gjennom nodene.
Sending av vekter og naboer fra nodene til serveren gjennom TCP.
Utregning av djikstra på nodene hos serveren.

PROTOKOLL!
Protokollen til ruting serveren er som følger. Serveren venter på at nodene skal tilkoble seg og hver gang en node kobler seg til serveren sender serveren en liten beskjed om at
koblingen er opprettet.
Etter at noden mottar beskjed om at koblingen er opprettet sender den en pakke til serveren i form av et unsigned char array.
Dette arrayet inneholder først to bytes som beskriver lengden på arrayet. Så to bytes som inneholder adressen til noden som pakken er sendt fra.
Så to bits med antall naboer noden har. Etter dette kommer et variabelt antall bytes basert på antall naboer.Her kommer først variabelt antall bytes som har addressen til naboene.
Så kommer variabelt antall bytes med vektene til kantene mellom node og nabo.
Etter at nabo har sendt pakke til serveren venter den på en pakke med rutingtabell tilbake.

Serveren holder orden på hvor mange pakker den har motatt. Når serveren har mottat pakker fra alle nodene, vil den så gå gjennom alle forbindelsene og
sende passende rutingtabell tilbake til nodene. Denne pakken er et unsigned char arrray, hvor to første bytes beskriver lengde på pakke, neste to bytes er antall noder, så
følger varierende antall bytes med først adresse til nodene, så følger varierende antall bytes med nabonoder pakkene rutes gjennom for å nå destinasjon.

Etter å ha sendt pakke til alle nodenen avslutter serveren.
Når nodene har mottat rutingtabell begynner node 1 og sende UDP pakker.
