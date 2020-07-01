
#pragma once

#include "thirdparty/GLMath.hpp"

class btCollisionShape;
class btCompoundShape;
class btDefaultMotionState;
class btRigidBody;

class btDefaultVehicleRaycaster;
class btRaycastVehicle;

class btDiscreteDynamicsWorld;

class PhysicWorld;

class PhysicVehicle
{
private:
    friend PhysicWorld;

private:
    enum e_Wheels
    {
        eFrontLeft = 0,
        eFrontRight,
        eBackLeft,
        eBackRight,
        eCount,
    };

    struct bullet
    {
        btCollisionShape*       chassisShape = nullptr;
        btCompoundShape*        compound = nullptr;
        btRigidBody*            carChassis = nullptr;

        btDefaultVehicleRaycaster*  vehicleRayCaster = nullptr;
        btRaycastVehicle*           vehicle = nullptr;
    }
    _bullet;

private:
    PhysicVehicle(btDiscreteDynamicsWorld& dynamicsWorld);
    ~PhysicVehicle();

public:
    void applyEngineForce(float engineForce);
    void setSteeringValue(float vehicleSteering);
    void reset();

    // void disableContactResponse();
    // void enableContactResponse();

public:
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec4& rotation);

public:
    const glm::mat4&    getOpenGLMatrix(glm::mat4& mat4x4) const;

    int                 getNumWheels() const;
    const glm::mat4&    getWheelOpenGLMatrix(int index, glm::mat4& mat4x4) const;

};
