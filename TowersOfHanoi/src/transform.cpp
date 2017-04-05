#include <transform.h>

Transform::Transform()
:
position(),
pitch(),
yaw(),
roll(),
scale(1, 1, 1)
{
}

glm::quat Transform::rotation()
{
	glm::quat x = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
	glm::quat y = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
	glm::quat z = glm::angleAxis(roll, glm::vec3(0, 0, -1));

	glm::quat outputRot = glm::normalize(z*y*x);

	return outputRot;
}

glm::vec3 Transform::euler()
{
	return glm::eulerAngles(rotation());
}

glm::vec3 Transform::forward()
{
	glm::quat outputRot = rotation();

	return outputRot*glm::vec3(0, 0, -1);
}

glm::vec3 Transform::up()
{
	glm::quat outputRot = rotation();

	return outputRot*glm::vec3(0, 1, 0);
}
