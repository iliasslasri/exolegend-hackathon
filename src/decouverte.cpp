
#include "decouverte.h"

float moduloPi(float a) // return angle in [-pi; pi]
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

    if (posError.norm2() < POS_REACHED_THRESHOLD)
    {
        targetReached = true;
    } 
    else if (std::abs(angleError) > ANGLE_REACHED_THRESHOLD)
    {
        float factor = 0.1;
        if (angleError < 0)
            factor = - factor;
        rightCommand = factor;
        leftCommand = -factor;
    }
    else {
        float factor = 0.5;
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