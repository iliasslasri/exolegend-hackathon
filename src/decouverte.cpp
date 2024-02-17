#include "decouverte.h"


enum Direction {
    NORTH,
    EAST,
    WEST,
    SOUTH
};


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
Position getAccessibleNeighbor(Gladiator *gladiator, Position posRaw) {
    const MazeSquare *square = gladiator->maze->getNearestSquare();
    int ret[2];
    //getIJfromXY(lastSquare.x, lastSquare.y, ret);
    // check the if a direction is not null take it other  return -1, -1 
    if (square->eastSquare != NULL) {
        
        // adding the new position to the table using getIJfromXY
        getIJfromXY(posRaw.x + (float)(CELL_SIZE+0.5*CELL_SIZE), posRaw.y, ret);
        // print all the table
        for (int i = 0; i < 14; ++i) {
        for (int j = 0; j < 14; ++j) {
            gladiator->log("%s", (grille[i][j] ? "true" : "false"));
        }
        std::cout << "\n";
    }
        if(( grille[ret[0]][ret[1]] == false)){
            grille[ret[0]][ret[1]] = true;
            return Position {posRaw.x + (float)(CELL_SIZE+0.5*CELL_SIZE), posRaw.y, 0};
        }
    } else if (square->northSquare != NULL) {
        
        getIJfromXY(posRaw.x, posRaw.y+ (float)(CELL_SIZE+0.5*CELL_SIZE), ret);
        // print the ret values
        gladiator->log("ret[0] %d ret[1] %d", ret[0], ret[1]);
        if(( grille[ret[0]][ret[1]] == false)){
            grille[ret[0]][ret[1]] = true;
            return Position {posRaw.x, posRaw.y + (float)(CELL_SIZE+0.5*CELL_SIZE), (float)M_PI/2};
        }
    } else if (square->westSquare != NULL) {
        
        getIJfromXY(posRaw.x - (float)(CELL_SIZE+0.5*CELL_SIZE), posRaw.y, ret);
        if(( grille[ret[0]][ret[1]] == false)){
            grille[ret[0]][ret[1]] = true;
            return Position {posRaw.x - (float)(CELL_SIZE+0.5*CELL_SIZE), posRaw.y, (float)M_PI};
        }
    } else if (square->southSquare != NULL) {
        
        getIJfromXY(posRaw.x, posRaw.y - (float)(CELL_SIZE+0.5*CELL_SIZE), ret);
        if(( grille[ret[0]][ret[1]] == false)){
            grille[ret[0]][ret[1]] = true;
            return Position {posRaw.x, posRaw.y - (float)(CELL_SIZE+0.5*CELL_SIZE), (float)(M_PI/2 + M_PI)};
        }
    } // Then loops without the second condition
    else if (square->eastSquare != NULL) {
        
        // adding the new position to the table using getIJfromXY
        getIJfromXY(posRaw.x+ (float)(CELL_SIZE+0.5*CELL_SIZE), posRaw.y, ret);
        grille[ret[0]][ret[1]] = true;
        return Position {posRaw.x + (float)(CELL_SIZE+0.5*CELL_SIZE), posRaw.y, 0};
        
    } else if (square->northSquare != NULL) {
        
        getIJfromXY(posRaw.x, posRaw.y+ (float)(CELL_SIZE+0.5*CELL_SIZE), ret);
        grille[ret[0]][ret[1]] = true;
        return Position {posRaw.x, posRaw.y + (float)(CELL_SIZE+0.5*CELL_SIZE), (float)M_PI/2};
    
    } else if (square->westSquare != NULL) {
        
        getIJfromXY(posRaw.x - (float)(CELL_SIZE + 0.5*CELL_SIZE), posRaw.y, ret);
        grille[ret[0]][ret[1]] = true;
        return Position {posRaw.x - (float)(CELL_SIZE + 0.5*CELL_SIZE), posRaw.y, (float)M_PI};
    
    } else if (square->southSquare != NULL) {
        
        getIJfromXY(posRaw.x, posRaw.y - (float)(CELL_SIZE + 0.5*CELL_SIZE), ret);
        grille[ret[0]][ret[1]] = true;
        return Position {posRaw.x, posRaw.y - (float)(CELL_SIZE + 0.5*CELL_SIZE), (float)(M_PI/2 + M_PI)};

    }
    return Position {-1, -1, -1};   
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


void set_direction(Gladiator Gladiator, Direction Direction){
    //

}


void turn_right_90(Gladiator gladiator){

}


void turn_left_90(Gladiator Gladiator){

}

void getNeighbors(){}