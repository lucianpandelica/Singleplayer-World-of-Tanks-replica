#include "lab_m1/Tema2/Helpers.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;

float Helpers::GetAngle(glm::vec3 crt_dir, glm::vec3 init_dir, glm::vec3 ref_dir)
{
    float angleCos = glm::dot(init_dir, crt_dir);
    float angle = acos(angleCos);
    float dir = glm::dot(ref_dir, crt_dir);
    if (dir < 0) {
        angle = -angle;
    }

    return angle;
}

glm::vec3 Helpers::RotateVec(glm::vec3 vec, float speed_rot, float deltaTime, glm::vec3 rotDir)
{
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::rotate(modelMatrix, RADIANS(speed_rot * deltaTime), rotDir);
    glm::vec4 aux = glm::vec4(vec.x, vec.y, vec.z, 1);
    glm::vec4 res = aux * modelMatrix;

    return glm::normalize(glm::vec3(res.x, res.y, res.z));
}