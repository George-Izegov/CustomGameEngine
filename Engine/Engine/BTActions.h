#pragma once

#include "AISystem/BrainTree.h"
#include "AIAgent.h"
#include "math_statics.h"
#include <iostream>

namespace bt = BrainTree;

class MoveTo : public bt::Leaf
{
    Status update() override
    {
        Vector3 goal = blackboard->getVector(BBPositionName);

        OpenSteer::Vec3 SteeringForce;

        SteeringForce = owner->steerForSeek(Vector3ToVec3(goal));

        if (goal.Distance(goal, Vec3ToVector3(owner->position())) < 10.0f)
        {
            return Status::Success;
        }

        owner->applySteeringForce(SteeringForce, 1 / 60.0f);

        return Status::Running;
    }

public:
    std::string BBPositionName;
};

class MoveOut : public bt::Leaf
{
    Status update() override
    {
        Vector3 Out = owner->m_Camera->GetPosition();

        OpenSteer::Vec3 SteeringForce;

        SteeringForce = owner->steerForFlee(Vector3ToVec3(Out));

        owner->applySteeringForce(SteeringForce, 1 / 60.0f);

        if (Out.Distance(Out, Vec3ToVector3(owner->position())) > 100.0f)
        {
            return Status::Success;
        }

        return Status::Running;
    }

};

class RandomPosition : public bt::Leaf
{
    Status update() override
    {
        Vector3 point = GetRandomPointInSquare(Vector3(0, 0, 0), radius);
        blackboard->setVector(BBPositionName, point);

        return Status::Success;
    }

public:
    std::string BBPositionName;
    float radius;
};

class Wait : public bt::Leaf
{
    Status update() override
    {
        blackboard->getVector(BBPositionName);
        std::cout << "Wait" << std::endl;

        return Status::Success;
    }

public:
    std::string BBPositionName;
};

class CheckForHummer : public bt::Decorator
{
public:
    AIAgent* owner;

public:
    Status update() override
    {
        auto status = child->tick();
        if (status == Status::Success) {
                return Status::Success;
            }
    }
};

class Interrupt : public bt::Decorator
{
public:
    Status update() override
    {
        if (BB->getBool(BBBoolName))
        {
            auto status = child->tick();

            if (status == Status::Success) {
                return Status::Success;
            }
        }
        else
        {
            return Status::Failure;
        }
    }

    std::string BBBoolName;
    SPBlackboard BB;
};