#include "Camera.h"
#include "glm/gtx/transform.hpp"

// Valores padr�o da c�mera
const float DEF_FOV = 45.0f; // graus

//------------------------------------------------------------
// Construtor
//------------------------------------------------------------
Camera::Camera()
: mPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
  mTargetPos(glm::vec3(0.0f, 0.0f, 0.0f)),
  mUp(glm::vec3(0.0f, 1.0f, 0.0f)),
  mRight(0.0f, 0.0f, 0.0f),
  WORLD_UP(0.0f, 1.0f, 0.0f),
  mYaw(glm::pi<float>()),
  mPitch(0.0f),
  mFOV(DEF_FOV)
{
}

//------------------------------------------------------------
// Retorna a matriz de visualiza��o
//------------------------------------------------------------
glm::mat4 Camera::getViewMatrix()const
{
	return glm::lookAt(mPosition, mTargetPos, mUp);
}

//------------------------------------------------------------
// Retorna o vetor de apar�ncia local da c�mera
//------------------------------------------------------------
const glm::vec3& Camera::getLook() const
{
	return mLook;
}

//------------------------------------------------------------
// Retorna o vetor local direito da c�mera
//------------------------------------------------------------
const glm::vec3& Camera::getRight() const
{
	return mRight;
}

//------------------------------------------------------------
// Retorna o vetor local da c�mera para cima
//------------------------------------------------------------
const glm::vec3& Camera::getUp() const
{
	return mUp;
}

//-----------------------------------------------------------------------------
// FPSCamera - Construtor
//-----------------------------------------------------------------------------
FPSCamera::FPSCamera(glm::vec3 position, float yaw, float pitch)
{
	mPosition = position;
	mYaw = yaw;
	mPitch = pitch;
}

//-----------------------------------------------------------------------------
// FPSCamera - Define a posi��o da c�mera no espa�o mundial
//-----------------------------------------------------------------------------
void FPSCamera::setPosition(const glm::vec3& position)
{
	mPosition = position;
}

//-----------------------------------------------------------------------------
// FPSCamera - Define a posi��o incremental da c�mera no espa�o mundial
//-----------------------------------------------------------------------------
void FPSCamera::move(const glm::vec3& offsetPos)
{
	mPosition += offsetPos;
	updateCameraVectors();
}

//-----------------------------------------------------------------------------
// FPSCamera - Define a orienta��o incremental da c�mera
//-----------------------------------------------------------------------------
void FPSCamera::rotate(float yaw, float pitch)
{
	mYaw += glm::radians(yaw);
	mPitch += glm::radians(pitch);

	// Constrain the pitch
	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);
	updateCameraVectors();
}

//-----------------------------------------------------------------------------
// FPSCamera - Calcula o vetor frontal dos �ngulos de Euler da c�mera (atualizados)
//-----------------------------------------------------------------------------
void FPSCamera::updateCameraVectors()
{
	// Esferas para coordenadas cartesianas
	// Calcular o vetor de dire��o da vista com base nos �ngulos de inclina��o e inclina��o (rolo n�o considerado)
	// raio � 1 para comprimento normalizad
	glm::vec3 look;
	look.x = cosf(mPitch) * sinf(mYaw);
	look.y = sinf(mPitch);
	look.z = cosf(mPitch) * cosf(mYaw);

	mLook = glm::normalize(look);

	// Recalcule o vetor Right e Up. Por simplicidade, o vetor correto
   // ser assumido horizontalmente o vetor de Up do mundo.
	mRight = glm::normalize(glm::cross(mLook, WORLD_UP));
	mUp = glm::normalize(glm::cross(mRight, mLook));

	mTargetPos = mPosition + mLook;
}


//------------------------------------------------------------
// OrbitCamera - construtor
//------------------------------------------------------------
OrbitCamera::OrbitCamera()
	: mRadius(10.0f)
{}

//------------------------------------------------------------
// OrbitCamera - Define o alvo para olhar
//------------------------------------------------------------
void OrbitCamera::setLookAt(const glm::vec3& target)
{
	mTargetPos = target;
}

//------------------------------------------------------------
// OrbitCamera - Define o raio da c�mera para direcionar a dist�ncia
//------------------------------------------------------------
void OrbitCamera::setRadius(float radius)
{
	// Clamp the radius
	mRadius = glm::clamp(radius, 2.0f, 80.0f);
}

//------------------------------------------------------------
// OrbitCamera - Gira a c�mera ao redor do alvo
// na posi��o dada guinada e inclina��o em graus.
//------------------------------------------------------------
void OrbitCamera::rotate(float yaw, float pitch)
{
	mYaw = glm::radians(yaw);
	mPitch = glm::radians(pitch);

	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);

	// Atualize os vetores Front, Right e Up usando os �ngulos atualizados de Euler
	updateCameraVectors();
}

//------------------------------------------------------------
// OrbitCamera - Calcula o vetor frontal da c�mera
// (atualiza) Euler Angles
//------------------------------------------------------------
void OrbitCamera::updateCameraVectors()
{
	// Esferas para coordenadas cartesianas
	mPosition.x = mTargetPos.x + mRadius * cosf(mPitch) * sinf(mYaw);
	mPosition.y = mTargetPos.y + mRadius * sinf(mPitch);
	mPosition.z = mTargetPos.z + mRadius * cosf(mPitch) * cosf(mYaw);
}
