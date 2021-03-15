#pragma once
#include "Utilities/BackendHandler.h"

class Player
{
public:
	Player(GameObject Obj);

	void PlayerMove(GLFWwindow* window, double dt);
	void PlayerPhy(GLFWwindow* window, double dt);
	void mapping();
	void blocker();
	bool returnFace();

	glm::vec3 getPosition();
private:
	GameObject* melonObj = nullptr;

	VertexArrayObject::sptr melonvao;

	float groundHight = 0.0f;
	bool isGround = true;
	bool faceingR = true;
	float acceleration = -50.0f;
	float position = 0.0f;

	bool canShoot = true;


};

