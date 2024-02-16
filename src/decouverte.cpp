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
Vector2 getAccessibleNeighbor(Gladiator *gladiator) {

    // get the position of the robot
    auto posRaw = gladiator->robot->getData().position;
    // get the i j of the robot
    int ret[2];
    getIJfromXY(posRaw.x, posRaw.y,ret); // Fix: Use curly braces for initialization
    int i = ret[0];
    int j = ret[1]; 

    const MazeSquare *square = gladiator->maze->getNearestSquare();
    // check the if a direction is not null take it other  return -1, -1 
    if (square->northSquare != NULL) {
        return Vector2(i, j+1);
    } else if (square->eastSquare != NULL) {
        return getXYfromIJ(i+1, j);
    } else if (square->westSquare != NULL) {
        return getXYfromIJ(i-1, j);
    } else if (square->southSquare != NULL) {
        return getXYfromIJ(i, j-1);
    }else return Vector2(-1, -1);
    
    
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

