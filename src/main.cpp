#include <stack>

#include "gladiator.h"
#include "decouverte.h"

Gladiator* gladiator;

float kw =2;//2.f;
float kv = 1.2f;
float wlimit = 3.f;
float vlimit = 0.6;
float erreurPos = 0.05;
float consl = 0.f;
float consr = 0.f;
bool has_goal = false;
bool turned = false;
const MazeSquare* last = nullptr;
Position goal {0, 0, 0};
const int TAILLE_MAX = 40;
const MazeSquare* pile[TAILLE_MAX]; // notre pile
int sommet = -1;
int i = 0;
unsigned long last_time = millis();
bool reached_target = false;
using Tableau = std::array<std::array<int, 12>, 12>;
Tableau isVisited {0};


double reductionAngle(double x)
{
    x = fmod(x + PI, 2 * PI);
    if (x < 0)
        x += 2 * PI;
    return x - PI;
}


bool turn(Position cons, Position pos) {
    double dx = cons.x - pos.x;
    double dy = cons.y - pos.y;
    double rho = atan2(dy, dx);
    gladiator->log("turn dx=%f dy=%f rho=%f d=%f cx=%f cy=%f x=%f y=%f", dx, dy, rho, reductionAngle(rho - pos.a), cons.x, cons.y, pos.x, pos.y);
    if (abs(reductionAngle(rho - pos.a)) > 0.1) {
        float consw = kw * reductionAngle(rho - pos.a);
        consw = consw > wlimit ? (consw > 0 ? 1 : -1) * wlimit : consw;

        float consvl = -gladiator->robot->getRobotRadius() * consw; // GFA 3.6.2
        float consvr = gladiator->robot->getRobotRadius() * consw; // GFA 3.6.2
        gladiator->control->setWheelSpeed(WheelAxis::RIGHT, consvr, false); // GFA 3.2.1
        gladiator->control->setWheelSpeed(WheelAxis::LEFT, consvl, false);  // GFA 3.2.1
        return false;
    } else {
        return true;
    }
}
bool go_to(Position cons, Position pos)
{
    double consvl, consvr;
    double dx = cons.x - pos.x;
    double dy = cons.y - pos.y;
    double d = sqrt(dx * dx + dy * dy);
    gladiator->log("goto dx=%f dy=%f d=%f cx=%f cy=%f x=%f y=%f", dx, dy, d, cons.x, cons.y, pos.x, pos.y);
    bool arrived = false;
    if (d > erreurPos)
    {
        double rho = atan2(dy, dx);
        double consw = kw * reductionAngle(rho - pos.a);

        double consv = kv * d * cos(reductionAngle(rho - pos.a));
        consw = abs(consw) > wlimit ? (consw > 0 ? 1 : -1) * wlimit : consw;
        consv = abs(consv) > vlimit ? (consv > 0 ? 1 : -1) * vlimit : consv;

        consvl = consv - gladiator->robot->getRobotRadius() * consw; // GFA 3.6.2
        consvr = consv + gladiator->robot->getRobotRadius() * consw; // GFA 3.6.2
        arrived = false;
    }
    else
    {
        consvr = 0;
        consvl = 0;
        arrived = true;
    }

    gladiator->control->setWheelSpeed(WheelAxis::RIGHT, consvr, false); // GFA 3.2.1
    gladiator->control->setWheelSpeed(WheelAxis::LEFT, consvl, false);  // GFA 3.2.1
    return arrived;
}
int empiler(const MazeSquare* s) {
    if (sommet < TAILLE_MAX) {
        sommet++;
        pile[sommet] = s;
        
        return 1;
    }
    return 0;
}

const MazeSquare* depiler() {
    const MazeSquare* s = nullptr;
    if (sommet >= 0) {
        s = pile[sommet];
        sommet--;
    }
    return s;
}



void reset() {
    //fonction de reset:
    //initialisation de toutes vos variables avant le début d'un match
    gladiator->log("Call of reset function"); // GFA 4.5.1
    i = 0;
    has_goal = false;
    turned = false;
    last = nullptr;
    consl = 0.f;
    consr = 0.f;
    last_time = millis();
    reached_target = false;
    sommet = -1;
    gladiator->log("Reset done"); // GFA 4.5.1
    // initialize the table of visited cells
    for (size_t i = 0; i < 12; i++) {
        for (size_t j = 0; j < 12; j++) {
            isVisited[i][j] = 0;
        }
    }
}

void setup() {
    //instanciation de l'objet gladiator
    gladiator = new Gladiator();
    //enregistrement de la fonction de reset qui s'éxecute à chaque fois avant qu'une partie commence
    gladiator->game->onReset(&reset); // GFA 4.4.1
}

std::pair<int, int> flee_wall(int cur_i, int cur_j){
    if (cur_i <= 6 && cur_j <= 6){
        return {cur_i+1, cur_j+1};
    }else if (cur_i <= 6 && cur_j > 6){
        return {cur_i+1,cur_j-1};
    }else if (cur_i > 6 && cur_j <= 6){
        return {cur_i-1,cur_j+1};
    }else{// >6 > 6
        return {cur_i-1, cur_j-1};
    }
}



// i want to add a timer so when i reach 20s i move to the target closest to the center
int timer = 0;
void loop() {
    if(gladiator->game->isStarted()) { //tester si un match à déjà commencer
        //code de votre stratégie

        // mettre à jour les cases par les quelles on est passé
        
        const MazeSquare* cur = gladiator->maze->getNearestSquare();
        
        // il faut se casser des murs et marquer là ou on passe sachat que ça peut être colorié


        if (cur != nullptr){ 
            int nb = get_nb_neighbors(cur);
            // visited 2 veut dire que soit c'est un cul de sac ou un sommet chemin (comme une arête d'un graphe) pas très interessant
            if ((nb == 1) || (nb == 2)){
                isVisited[cur->i][cur->j] = 2;
            }else
            {
                isVisited[cur->i][cur->j] = 1;
            }
        }

        Position myPosition = gladiator->robot->getData().position;
        const MazeSquare* target = nullptr;


        // TODO : si encloisonnée go to center

        /////////// --------- Tant qu'il peut partir, qu'il est pa dans un cul de sac il revient vers le père

        // todo au dessus d'un mur
        // if we are above a wall go to the center
        // if (myPosition.x > 0.5 && myPosition.x < 1.5 && myPosition.y > 0.5 && myPosition.y < 1.5){
        //     gladiator->log("I am above a wall, go to the center");
        //     std::pair<int, int> flee = flee_wall(cur->i, cur->j);
        //     target = gladiator->maze->getSquare(flee.first, flee.second);
        //     has_goal = true;
        // }

        // taiter le problème du rétricissement 
        gladiator->log("TIME diff is  %ld", millis()-last_time);
        // if ( millis()-last_time >= 5000 ){
        //     // check if we are in the borders
            


        //     last_time = millis();
        //     gladiator->log("Time is up, go to the center %ld ", last_time);
        //     std::pair<int, int> flee = flee_wall(cur->i, cur->j);
        //     float square_size = gladiator->maze->getSquareSize();
        //     target = gladiator->maze->getSquare(flee.first, flee.second);
        // }
        
        if(!has_goal) {
            gladiator->log("HAVE NO GOAL");
            //const MazeSquare * target = nullptr;
            target = nullptr;
            const MazeSquare *square = nullptr;
            const MazeSquare *neighbors[4]{nullptr}; // East, North, West, South
            if (sommet == -1) { // pas de but et pas de case dans la pile
                gladiator->log("Stack is empty lets discover !");
                square = gladiator->maze->getNearestSquare();
                // empiler(square = gladiator->maze->getNearestSquare());
                gladiator->log("coordiantes of square: %d %d ", square->i, square->j);
                getNeighbors_bis(gladiator, square,  neighbors, isVisited, last);
                // log all the coordinates of the neighbors 
                gladiator->log("number  of neighbors %d", nb_far_neighors(neighbors));
                gladiator->log("Printing");
                for (size_t p = 0; p < 4; p++) {
                    if (neighbors[p] != nullptr){
                        gladiator->log("neighbor %ld \t i :  %d  j:    %d ", p ,neighbors[p]->i, neighbors[p]->j);
                    }
                }
                gladiator->log("Printing after");

                // push to the neighbors to the dfs stack
                // faire le dfs ou on prends une branche quand on decide de prednre une direction               
                
                for (int k = 0; k < 4; k++) {
                    if (neighbors[k] != nullptr) {
                        if (isLeaf(neighbors[k])) {
                            // push your father
                            gladiator->log("there is a leaf!!!!! coords  %d %d ", neighbors[k]->i, neighbors[k]->j);
                            empiler(square);
                            gladiator->log("coordiantes of emmpile : %d %d ", square->i, square->j);
                        }
                        //empiler(square);
                        empiler(neighbors[k]);
                        gladiator->log("coordiantes of emmpile : %d %d ", neighbors[k]->i, neighbors[k]->j);
                        
                    }
                }  
            }
            target = depiler();
                // the target could have disappeared but still go as far as before 
                // test if the target is still in the game 
            if (target == nullptr) {
                gladiator->log("target is not a wall : %d %d ", target->i, target->j);
                target = last;
            }
            else{ 
                if(!isLeaf(target)){
                    sommet = -1;// last modification
                } 
                last = square;
                gladiator->log("coordiantes of depile (target) : %d %d ", target->i, target->j);
                float square_size = gladiator->maze->getSquareSize();
                last = square;
                goal.x =  (target->i + 0.5) * square_size;
                goal.y = (target->j + 0.5) * square_size;
                gladiator->saveUserWaypoint(goal.x, goal.y);
                gladiator->log("Go to new target x=%f; y=%f", goal.x, goal.y);
                has_goal = true;
                
            }
        } // has a goal
        else {
            reached_target = false;
            if (!turned) {
                bool finish_turning = turn(goal, myPosition);
                if (finish_turning) {
                    turned = true;
                }
                delay(10); // TODO : à supprimer
            }else {
                reached_target = go_to(goal, myPosition);
                delay(10); // TODO : supprime
            }

            if (reached_target)
            {
                
                has_goal = false;
                turned = false;
                gladiator->log("Target reached ! ");
                delay(100);
            } 
        }

    }
    delay(100);// was 10
    timer+=10;
    
}




