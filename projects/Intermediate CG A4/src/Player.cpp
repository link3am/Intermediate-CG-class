#include "Player.h"

Player::Player(GameObject Obj)
{
	melonObj = &Obj;
}


void Player::PlayerMove(GLFWwindow* window, double dt) {
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		melonObj->get<Transform>().MoveLocalFixed(0.07f, 0.0f, 0.0f);
		melonObj->get<Transform>().SetLocalRotation(0.0f, -10.0f, 0.0f);
		melonObj->get<Transform>().SetLocalScale(1.0f, 1.0f, -1.0f);
		faceingR = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

		melonObj->get<Transform>().MoveLocalFixed(-0.07f, 0.0f, 0.0f);
		melonObj->get<Transform>().SetLocalRotation(0.0f, 10.0f, 0.0f);
		melonObj->get<Transform>().SetLocalScale(-1.0f, 1.0f, -1.0f);
		faceingR = false;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

		if (isGround == true) {
			position += 15 * dt;
			isGround = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (isGround == true && melonObj->get<Transform>().GetLocalPosition().y >= 0.2)
		{
			melonObj->get<Transform>().MoveLocalFixed(0.0f, -0.2f, 0.0f);
			isGround = false;
			groundHight = 0.0f;
		}
	}
}

void Player::mapping()
{
	if (melonObj->get<Transform>().GetLocalPosition().x > 5.0f && melonObj->get<Transform>().GetLocalPosition().x < 11.0f && melonObj->get<Transform>().GetLocalPosition().y >= 3.9f)
	{
		groundHight = 4.0f;
	}
	else if (melonObj->get<Transform>().GetLocalPosition().x > 1.0f && melonObj->get<Transform>().GetLocalPosition().x < 3.0f && melonObj->get<Transform>().GetLocalPosition().y >= 1.9f)
	{
		groundHight = 2.0f;
	}
	else if (melonObj->get<Transform>().GetLocalPosition().x > 12.0f && melonObj->get<Transform>().GetLocalPosition().x < 18.0f && melonObj->get<Transform>().GetLocalPosition().y >= 2.9f)
	{
		groundHight = 3.0f;
	}
	else if (melonObj->get<Transform>().GetLocalPosition().x > 19.0f && melonObj->get<Transform>().GetLocalPosition().x < 19.8f && melonObj->get<Transform>().GetLocalPosition().y >= 1.4f)
	{
		groundHight = 1.5f;
	}
	else if (melonObj->get<Transform>().GetLocalPosition().x > 21.5f && melonObj->get<Transform>().GetLocalPosition().x < 23.5f && melonObj->get<Transform>().GetLocalPosition().y >= 0.9f)
	{
		groundHight = 1.0f;
	}
	else if (melonObj->get<Transform>().GetLocalPosition().x > 25.6f && melonObj->get<Transform>().GetLocalPosition().x < 26.4f && melonObj->get<Transform>().GetLocalPosition().y >= 1.4f)
	{//can
		groundHight = 1.5f;
	}
	else if (melonObj->get<Transform>().GetLocalPosition().x > 30.7f && melonObj->get<Transform>().GetLocalPosition().x < 32.4f && melonObj->get<Transform>().GetLocalPosition().y >= 0.6f)
	{
		groundHight = 0.7f;
	}
	else if (melonObj->get<Transform>().GetLocalPosition().x > 42.0f || melonObj->get<Transform>().GetLocalPosition().x < -3)
	{
		groundHight = -10.0f;
	}
	else
	{
		groundHight = 0;
	}

}

void Player::blocker()
{
	//microwave
	{
		if (melonObj->get<Transform>().GetLocalPosition().x > 12.0f && melonObj->get<Transform>().GetLocalPosition().x < 14.0f && melonObj->get<Transform>().GetLocalPosition().y < 2.9f)
		{
			melonObj->get<Transform>().MoveLocalFixed(-0.1f, 0.0f, 0.0f);
		}
		if (melonObj->get<Transform>().GetLocalPosition().x > 16.0f && melonObj->get<Transform>().GetLocalPosition().x < 18.5f && melonObj->get<Transform>().GetLocalPosition().y < 2.9f)
		{
			melonObj->get<Transform>().MoveLocalFixed(0.1f, 0.0f, 0.0f);
		}
	}
	//pan
	if (melonObj->get<Transform>().GetLocalPosition().x > 30.7f && melonObj->get<Transform>().GetLocalPosition().x < 31.0f && melonObj->get<Transform>().GetLocalPosition().y < 0.6f)
	{
		melonObj->get<Transform>().MoveLocalFixed(-0.1f, 0.0f, 0.0f);
	}
	if (melonObj->get<Transform>().GetLocalPosition().x > 32.2f && melonObj->get<Transform>().GetLocalPosition().x < 32.4f && melonObj->get<Transform>().GetLocalPosition().y < 0.6f)
	{
		melonObj->get<Transform>().MoveLocalFixed(0.1f, 0.0f, 0.0f);
	}
}




void Player::PlayerPhy(GLFWwindow* window, double dt)
{

	PlayerMove(window, dt);
	mapping();
	blocker();
	if (melonObj->get<Transform>().GetLocalPosition().y < groundHight)
	{

		melonObj->get<Transform>().SetLocalPosition(melonObj->get<Transform>().GetLocalPosition().x, groundHight, melonObj->get<Transform>().GetLocalPosition().z);
		isGround = true;
		//test
		position = 0.0f;
	}

	if (melonObj->get<Transform>().GetLocalPosition().y > groundHight + 0.1)//gravity
	{
		//test 
		isGround = false;
		position += acceleration * dt * dt;

	}

	melonObj->get<Transform>().MoveLocalFixed(0.0f, position, 0.0f);

	if (melonObj->get<Transform>().GetLocalPosition().y <= -5)
	{
		melonObj->get<Transform>().SetLocalPosition(0.0f, 0.0f, 0.0f);
		isGround = true;
	}

}

bool Player::returnFace()
{ 
	if (melonObj->get<Transform>().GetLocalScale().x > 0) {
		return true;
	}
	else {
		return false;
	}
}

glm::vec3 Player::getPosition()
{
	return melonObj->get<Transform>().GetLocalPosition();
}
