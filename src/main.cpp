#include "gladiator.h"
#include "decouverte.h"
#include <cmath>
#undef abs

// x,y représentent des coordonnées en m
// Vector{1.5,1.5} représente le point central
// Pour convertir une cordonnée de cellule (i,j) (0<=i<=13, 0<=j<=13) :
// x = i * CELL_SIZE + 0.5*CELL_SIZE
// y = j * CELL_SIZE + 0.5*CELL_SIZE
// avec CELL_SIZE = 3.0/14 (~0.214)

Gladiator* gladiator;

void reset() {
}



void setup() {
    //instanciation de l'objet gladiator
    gladiator = new Gladiator();
    //enregistrement de la fonction de reset qui s'éxecute à chaque fois avant qu'une partie commence
    gladiator->game->onReset(&reset);
}

void loop() {
    if (gladiator->game->isStarted())
    {
        static unsigned i = 0;
        bool showLogs = (i%50 == 0);
        
        // later it should be undiscovered
        
        Vector2 cell_vector = getAccessibleNeighbor(gladiator);
        if (cell_vector.x() == -1 && cell_vector.y() == -1) {
            gladiator->log("no accessible neighbor");
            return;
        }
        if (aim(gladiator, cell_vector, showLogs))
        {
            gladiator->log("target atteinte !");
        }
        i++;
    }
    delay(10); // boucle à 100Hz
}

