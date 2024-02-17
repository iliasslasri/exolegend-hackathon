#include "decouverte.h"



const float NORTH = M_PI/2;
const float EAST = 0;
const float SOUTH = -M_PI/2;
const float WEST = M_PI;


void randomwalk(Gladiator *gladiator) {
    // we will implement a random walk
    

    gladiator->control->setWheelSpeed(WheelAxis::RIGHT, 0.6); //controle de la roue droite
    gladiator->control->setWheelSpeed(WheelAxis::LEFT, 0.6); //control de la roue gauche
    //Lorsque le jeu commencera le robot ira en ligne droite

    // implement a ra
    
    auto posRaw = gladiator->robot->getData().position;

    float angle = posRaw.a;


    if (angle > 0.5) {
        gladiator->control->setWheelSpeed(WheelAxis::RIGHT, 0.6); //controle de la roue droite
        gladiator->control->setWheelSpeed(WheelAxis::LEFT, 0.6); //control de la roue gauche
    } else if (angle < -0.5) {
        gladiator->control->setWheelSpeed(WheelAxis::RIGHT, -0.6); //controle de la roue droite
        gladiator->control->setWheelSpeed(WheelAxis::LEFT, -0.6); //control de la roue gauche
    } else {
        gladiator->control->setWheelSpeed(WheelAxis::RIGHT, 0.6); //controle de la roue droite
        gladiator->control->setWheelSpeed(WheelAxis::LEFT, -0.6); //control de la roue gauche
    }
}
inline float moduloPi(float a) // return angle in [-pi; pi]
{
    return (a < 0.0) ? (std::fmod(a - M_PI, 2*M_PI) + M_PI) : (std::fmod(a + M_PI, 2*M_PI) - M_PI);
}
bool cmp_s(const MazeSquare* s1,const MazeSquare* s2) {
    if (s1!=nullptr && s2!=nullptr) {
        if (s1->i == s2->i && s2->j == s1->j) {
            return true;
        }
    }
    return false;
}
bool aim(Gladiator* gladiator, const Vector2& target, bool showLogs)
{
    constexpr float ANGLE_REACHED_THRESHOLD = 0.1;
    constexpr float POS_REACHED_THRESHOLD = 0.05;

    auto posRaw = gladiator->robot->getData().position;
    Vector2 pos{posRaw.x, posRaw.y};

    Vector2 posError = target - pos;

    float targetAngle = posError.angle();
    float angleError = moduloPi(targetAngle - posRaw.a);

    bool targetReached = false;
    float leftCommand = 0.f;
    float rightCommand = 0.f;

    if (posError.norm2() < POS_REACHED_THRESHOLD) //
    {
        targetReached = true;
    } 
    else if (std::abs(angleError) > ANGLE_REACHED_THRESHOLD)
    {
        float factor = 0.05;
        if (angleError < 0)
            factor = - factor;
        rightCommand = factor;
        leftCommand = -factor;
    }
    else {
        float factor = 0.2;
        rightCommand = factor;//+angleError*0.1  => terme optionel, "pseudo correction angulaire";
        leftCommand = factor;//-angleError*0.1   => terme optionel, "pseudo correction angulaire";
    }

    gladiator->control->setWheelSpeed(WheelAxis::LEFT, leftCommand);
    gladiator->control->setWheelSpeed(WheelAxis::RIGHT, rightCommand);

    if (showLogs || targetReached)
    {
        gladiator->log("ta %f, ca %f, ea %f, tx %f cx %f ex %f ty %f cy %f ey %f", targetAngle, posRaw.a, angleError, target.x(), pos.x(), posError.x(), target.y(), pos.y(), posError.y());
    }

    return targetReached;
}


// i want to get the coordiantes of the neighbor that has no wall 
int getAccessibleNeighbor(Gladiator *gladiator,Position posRaw, Vector2 vect) {

    

    const MazeSquare *square = gladiator->maze->getNearestSquare();
    // check the if a direction is not null take it other  return -1, -1 
    if (square->northSquare != NULL) {
        vect.set_xy(posRaw.x, posRaw.y+CELL_SIZE);
        return 0;
    } else if (square->eastSquare != NULL) {
        vect.set_xy(posRaw.x+CELL_SIZE, posRaw.y);
        return 0;
    } else if (square->westSquare != NULL) {
        vect.set_xy(posRaw.x-CELL_SIZE, posRaw.y);
        return 0;
    } else if (square->southSquare != NULL) {
        vect.set_xy(posRaw.x, posRaw.y-CELL_SIZE) ;   
        return 0;
    }else return -1;
       
}

// get i j and from x y i and j to meters

void getIJfromXY(float x, float y, int* ret) {
    ret[0] = (int) (x / CELL_SIZE);
    ret[1] = (int) (y / CELL_SIZE);

}

// get x y and from i and j 
Vector2 getXYfromIJ(int i, int j){
    return Vector2(i*CELL_SIZE + 0.5*CELL_SIZE, j*CELL_SIZE+0.5*CELL_SIZE);
}

// hasWall() returns true if there is a wall in the direction of the angle
bool hasWall(Gladiator* gladiator, const MazeSquare* nearestSquare, const float angle){
    if(angle > 2*M_PI- (M_PI/4) || angle < M_PI/4){
        // test if north is a wall 
        if(nearestSquare->eastSquare == NULL){
            gladiator->log("east is a wall");
            return true;
        }
    } else if(angle < 2*M_PI- (M_PI/4) && angle > M_PI+ (M_PI/4)){
        // test if east is a wall
        if(nearestSquare->southSquare == NULL){
            return true;
        }
    } else if(angle > M_PI/4 && angle < 3*M_PI/4){
        // test if east is a wall
        if(nearestSquare->northSquare == NULL){
            return true;
        }
    } else {
        // test if west is a wall
        if(nearestSquare->westSquare == NULL){
            return true;
        }
    }
    return false;
}


// void set_direction(Gladiator Gladiator, float angle){
//     //
//     while (angle - Direction > 0.1){
//         gladiator->control->setWheelSpeed(WheelAxis::RIGHT, 0.6); //controle de la roue droite
//         gladiator->control->setWheelSpeed(WheelAxis::LEFT, -0.6); //control de la roue gauche
//     }
// }


// void turn_right_90(Gladiator gladiator){
//     gladiator->control->setWheelSpeed(WheelAxis::RIGHT, 0.6); //controle de la roue droite
//     gladiator->control->setWheelSpeed(WheelAxis::LEFT, -0.6); //control de la roue gauche
//     delay(500);
//     gladiator->control->setWheelSpeed(WheelAxis::RIGHT, 0); //controle de la roue droite
//     gladiator->control->setWheelSpeed(WheelAxis::LEFT, 0); //control de la roue gauche
// }


void turn_north(Gladiator *gladiator, Position posRaw){
    if (NORTH - posRaw.a > 0.1){
        gladiator->control->setWheelSpeed(WheelAxis::RIGHT, 0.6); //controle de la roue droite
        gladiator->control->setWheelSpeed(WheelAxis::LEFT, -0.6); //control de la roue gauche
    }else if ( NORTH - posRaw.a < -0.1){
        gladiator->control->setWheelSpeed(WheelAxis::RIGHT, -0.6); //controle de la roue droite
        gladiator->control->setWheelSpeed(WheelAxis::LEFT, 0.6); //control de la roue gauche
    }
}

bool isCoin(MazeSquare* square){
    // returns true when the maze square is a corner
    if (square->northSquare == nullptr && square->westSquare == nullptr){
        return true;
    }else if (square->northSquare == nullptr && square->eastSquare == nullptr){
        return true;
    }else if (square->southSquare == nullptr && square->westSquare == nullptr){
        return true;
    }else if (square->southSquare == nullptr && square->eastSquare == nullptr){
        return true;
    }else 
        return false;
}


bool isEdge(const MazeSquare* square){
    // returns true when the maze square is an edge
    if (square->northSquare == nullptr && square->southSquare == nullptr ){
        return true;
    }else if (square->eastSquare == nullptr && square->westSquare == nullptr){
        return true;
    }else 
        return false;
    
}

void getNeighbors(Gladiator *gladiator,const MazeSquare * square, const MazeSquare* neighbors[4]){
    // on va essayer d'avoir le neighbor le plus loin dans la direction de la direction actuelle


    const MazeSquare* target = nullptr;
    const MazeSquare* temp = nullptr;
    
    if(square->eastSquare != nullptr){
        temp = square->eastSquare;
        while((temp !=nullptr)){
            target = temp;
            temp = temp->eastSquare;
        }
        neighbors[0] = target;
    }else if(square->northSquare != nullptr){
        temp = square->northSquare;
        while(temp !=nullptr){
            target = temp;
            temp = temp->northSquare;
            gladiator->log("Can go north ");
        }
        neighbors[1] = target;
    }else if(square->westSquare != nullptr){
        temp = square->westSquare;
        while(temp !=nullptr){
            target = temp;
            temp = temp->westSquare;
            gladiator->log("Can go west ");
        }
        neighbors[2] = target;
    }else if(square->southSquare != nullptr ){//&& (square->eastSquare !=nullptr || square->westSquare!=nullptr)){
        temp = square->southSquare;
        while((temp !=nullptr)){
            target = temp;
            temp = temp->southSquare;
            gladiator->log("Can go south ");
        }
        neighbors[3] = target;
    }
    return;
}


void getNeighbors_bis(Gladiator *gladiator,const MazeSquare * square, const MazeSquare* neighbors[4], Tableau &isVisited,const MazeSquare *last){
    // on va essayer d'avoir le neighbor le plus loin dans la direction de la direction actuelle

    const MazeSquare* target[4]{nullptr, nullptr, nullptr, nullptr};
    const MazeSquare* temp = nullptr;
    const MazeSquare* checkpoint[4]{nullptr, nullptr, nullptr, nullptr};
    gladiator->log("Getting neighbors inside function");
    if (square == nullptr){
        gladiator->log("square is null");
        return;
    }
    if(square->eastSquare != nullptr){
        temp = square->eastSquare;
        while(temp !=nullptr){
             if (!isEdge(temp)&& (isVisited[temp->i][temp->j] == 0)){
                checkpoint[0] = temp;
                gladiator->log("interesting edge");
            }else if (!isEdge(temp)) // n'importe lequel
                checkpoint[0] = temp;
            gladiator->log("exploring east %d %d", temp->i, temp->j);
            target[0] = temp;
            temp = temp->eastSquare;
        }
    }

    if(square->northSquare != nullptr){
        temp = square->northSquare;
        while(temp !=nullptr){
            if (!isEdge(temp) && (isVisited[temp->i][temp->j] == 0)){
                checkpoint[1] = temp;
            }else if (!isEdge(temp)) // n'importe lequel
                checkpoint[1] = temp;
            
            gladiator->log("exploring north %d %d", temp->i, temp->j);
            target[1] = temp;
            temp = temp->northSquare;
            
        }
    }
    if(square->westSquare != nullptr){
        temp = square->westSquare;
        while(temp !=nullptr) { // checkpoint interessant pas encore découvert
            if (!isEdge(temp) && (isVisited[temp->i][temp->j] == 0)){
                checkpoint[2] = temp;
            }else if (!isEdge(temp)) // n'importe lequel
                checkpoint[2] = temp;
            
            gladiator->log("exploring west %d %d", temp->i, temp->j);
            target[2] = temp;
            temp = temp->westSquare;
        }
    }
    if(square->southSquare != nullptr ){//&& (square->eastSquare !=nullptr || square->westSquare!=nullptr)){
        temp = square->southSquare;
        while(temp !=nullptr){
            if (!isEdge(temp) && (isVisited[temp->i][temp->j] == 0)){// intersection
                checkpoint[3] = temp;
            }else if (!isEdge(temp)) // n'importe lequel
                checkpoint[2] = temp;
            gladiator->log("exploring south %d %d", temp->i, temp->j);
            target[3] = temp;
            temp = temp->southSquare;
        }
     
    }

    gladiator->log("found a target i will tests visits");
    for (size_t t = 0; t < 4; t++) {
        if ((target[t] != nullptr) && !cmp_s(target[t], last)){

            if (isVisited[target[t]->i][target[t]->j] == 0){// pas visité
                neighbors[t] = target[t];
            } else if (isVisited[target[t]->i][target[t]->j] == 2) // c'est une feuille visité ou une arrete 
                {
                    neighbors[t] = checkpoint[t];
                }
            else // visited == 1
            {
                    neighbors[t] = checkpoint[t];
            }
        }else if ((target[t] != nullptr))
        {
            if (isVisited[target[t]->i][target[t]->j] == 0){// pas visité
                neighbors[t] = target[t];
            } else if (isVisited[target[t]->i][target[t]->j] == 2) // c'est une feuille visité ou une arrete 
                {
                    neighbors[t] = checkpoint[t];
                }
            else // visited == 1
            {
                    neighbors[t] = checkpoint[t];
            }
        }
        
        else {
            neighbors[t] = nullptr;
        }
    }
    
    return;
}

int nb_far_neighors(const MazeSquare* neighbors[4]){
    int count = 0;
    for (size_t i = 0; i < 4; i++) {
        if (neighbors[i] != nullptr){
            count++;
        }
    }
    return count;
}

bool isLeaf(const MazeSquare *square){
    // returns true when the maze square is a leaf
    int count = 0;
    if (square->northSquare == nullptr){
        count++;
    }
    if (square->southSquare == nullptr){
        count++;
    }
    if (square->eastSquare == nullptr){
        count++;
    }
    if (square->westSquare == nullptr){
        count++;
    }
    if (count == 3){
        return true;
    }
    return false;
}

int get_nb_neighbors(const MazeSquare* s){
    int count = 0;
    if (s->eastSquare != nullptr)
        count ++;
    if (s->westSquare != nullptr)
        count ++;
    if (s->northSquare != nullptr)
        count ++;    
    if (s->southSquare != nullptr)
        count ++;

    return count ;
}
