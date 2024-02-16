#include "gladiator.h"
#include "decouverte.h"
Gladiator* gladiator;
void reset();
void setup() {
    //instanciation de l'objet gladiator
    gladiator = new Gladiator();
    //enregistrement de la fonction de reset qui s'éxecute à chaque fois avant qu'une partie commence
    gladiator->game->onReset(&reset);
}

void reset() {
    //fonction de reset:
    //initialisation de toutes vos variables avant le début d'un match
}

void loop() {
    if (gladiator->game->isStarted()){
        static unsigned i = 0;
        bool showLogs = (i%50 == 0);
        
        if (aim(gladiator, {1.5, 1.5}, showLogs)) // TODO : Remove show logs from aim
        {
            gladiator->log("target atteinte !");
        }
        i++;
    }
    else {
        gladiator->log("attente du début du match");
        }
    delay(10); // boucle à 100Hz
}
