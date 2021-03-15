#include "Bullet.h"
Bullet::Bullet(GameObject& Obj, bool isRight) :bulletObj(entt::handle(Obj.registry(), Obj.entity()))
{
	IsRight = isRight;

	if (IsRight) {
		startPos = glm::vec3(bulletObj.get<Transform>().GetLocalPosition().x + 1.0f, bulletObj.get<Transform>().GetLocalPosition().y, bulletObj.get<Transform>().GetLocalPosition().z);
	}
	else {
		startPos = glm::vec3(bulletObj.get<Transform>().GetLocalPosition().x - 1.0f, bulletObj.get<Transform>().GetLocalPosition().y, bulletObj.get<Transform>().GetLocalPosition().z);
	}

	if (IsRight) {
		endPos = glm::vec3(bulletObj.get<Transform>().GetLocalPosition().x + 5, bulletObj.get<Transform>().GetLocalPosition().y, bulletObj.get<Transform>().GetLocalPosition().z);
	}

	else {
		endPos = glm::vec3(bulletObj.get<Transform>().GetLocalPosition().x - 5, bulletObj.get<Transform>().GetLocalPosition().y, bulletObj.get<Transform>().GetLocalPosition().z);
	}
}

void Bullet::projectile()
{
	t += .05;
	if (t > 1) {
		death = true;
	}
	glm::vec3 newPos = (endPos - startPos) * t + startPos;
	bulletObj.get<Transform>().SetLocalPosition(newPos.x, newPos.y, bulletObj.get<Transform>().GetLocalPosition().z);
}

void Bullet::update()
{
	projectile();
}

