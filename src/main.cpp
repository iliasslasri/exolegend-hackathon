#include "gladiator.h"
#include "decouverte.h"
Gladiator* gladiator;

float kw = 0.5;
float kv = 1.f;
float wlimit = 3.f;
float vlimit = 0.6;
float erreurPos = 0.07;

double reductionAngle(double x)
{
    x = fmod(x + PI, 2 * PI);
    if (x < 0)
        x += 2 * PI;
    return x - PI;
}
void go_to(Position cons, Position pos)
{
    double consvl, consvr;
    double dx = cons.x - pos.x;
    double dy = cons.y - pos.y;
    double d = sqrt(dx * dx + dy * dy);

    if (d > erreurPos)
    {
        double rho = atan2(dy, dx);
        double consw = kw * reductionAngle(rho - pos.a);

        double consv = kv * d * cos(reductionAngle(rho - pos.a));
        consw = abs(consw) > wlimit ? (consw > 0 ? 1 : -1) * wlimit : consw;
        consv = abs(consv) > vlimit ? (consv > 0 ? 1 : -1) * vlimit : consv;

        consvl = consv - gladiator->robot->getRobotRadius() * consw; // GFA 3.6.2
        consvr = consv + gladiator->robot->getRobotRadius() * consw; // GFA 3.6.2
    }
    else
    {
        consvr = 0;
        consvl = 0;
    }

    gladiator->control->setWheelSpeed(WheelAxis::RIGHT, consvr, false); // GFA 3.2.1
    gladiator->control->setWheelSpeed(WheelAxis::LEFT, consvl, false);  // GFA 3.2.1
}

void reset() {
    //fonction de reset:
    //initialisation de toutes vos variables avant le début d'un match
    gladiator->log("Call of reset function"); // GFA 4.5.1
}

void setup() {
    //instanciation de l'objet gladiator
    gladiator = new Gladiator();
    //enregistrement de la fonction de reset qui s'éxecute à chaque fois avant qu'une partie commence
    gladiator->game->onReset(&reset); // GFA 4.4.1
}
float xx = 0;
float yy = 0;
void loop() {
    if(gladiator->game->isStarted()) { //tester si un match à déjà commencer
        //code de votre stratégie
        Position myPosition = gladiator->robot->getData().position;
        Vector2 vect{};
        getAccessibleNeighbor(gladiator, myPosition, vect);
        
        Position goal {vect.x(), vect.y(), 0};
        
        go_to(goal, myPosition);

        if (!(xx == goal.x && yy == goal.y)){
            gladiator->log("goal is %f  %f  %f", goal.x,goal.y, goal.a );
            xx = goal.x;
            yy = goal.y;
        }
    
    }
    delay(100);
}






// void loop() {
//     if (gladiator->game->isStarted())
//     {
//         static unsigned i = 0;
//         bool showLogs = (i%50 == 0);
        
//         // later it should be undiscovered
        
//         auto cell_vector = getAccessibleNeighbor(gladiator);
//         if (cell_vector.x() == -1 && cell_vector.y() == -1) {
//             gladiator->log("no accessible neighbor");
//             return;
//         }
//         if (aim(gladiator, cell_vector, showLogs))
//         {
//             gladiator->log("target atteinte !");
//             auto cell_vector = getAccessibleNeighbor(gladiator);
//         }
//         i++;
//     }
//     delay(10); // boucle à 100Hz
// }

