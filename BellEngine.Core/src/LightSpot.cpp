#include "LightSpot.h"
#include <StringHelpers.h>
#include <MathHelper.h>
#include <Transform.h>
#include <BellEngineID.h>

LightSpot::LightSpot()
{
	constAttenuation = 0.0f;
	linearAttenuation = 0.0f;
	quadraticAttenuation = 0.0f;
	cutOffDistance = 0.0f;
	direction = Vector3D();
	innerAngle = 0.0f;
	outerAngle = 0.0f;

	SetTransform(new Transform());
}

LightSpot::~LightSpot()
{
	DisposeTransform();

	if (collider)
	{
		delete collider;
	}
}

const bool LightSpot::IsActive()
{
	return this->isActive;
}

void LightSpot::Activate(const bool& value)
{
	this->isActive = value;
}

void LightSpot::Dispose()
{
	DisposePointer(transform);
	DisposePointer(collider);
}

void LightSpot::SetInnerAngle(const float& value)
{
	this->innerAngle = value;
}

const float LightSpot::GetInnerAngle()
{
	return this->innerAngle;
}

void LightSpot::SetOuterAngle(const float& value)
{
	this->outerAngle = value;
}

const float LightSpot::GetOuterAngle()
{
	return this->outerAngle;
}

void LightSpot::SetDirection(const Vector3D& direction)
{
	this->direction = direction;
}

const Vector3D LightSpot::GetDirection()
{
	return this->direction;
}

void LightSpot::SetConstantAttenuation(const float& value)
{
	this->constAttenuation = value;
}

const float LightSpot::GetConstantAttenuation()
{
	return this->constAttenuation;
}

void LightSpot::SetLinearAttenuation(const float& value)
{
	this->linearAttenuation = value;
}

const float LightSpot::GetLinearAttenuation()
{
	return this->linearAttenuation;
}

void LightSpot::SetQuadraticAttenuation(const float& value)
{
	this->quadraticAttenuation = value;
}

const float LightSpot::GetQuadraticAttenuation()
{
	return this->quadraticAttenuation;
}

void LightSpot::SetCutoffDistance(const float& value)
{
	this->cutOffDistance = value;
}

const float LightSpot::GetCutoffDistance()
{
	return this->cutOffDistance;
}

const Vector4D LightSpot::GetBaseDiffuseColor()
{
	return this->baseDiffuseColor;
}

const Vector4D LightSpot::GetBaseSpecularColor()
{
	return this->baseSpecularColor;
}

void LightSpot::SetDiffuse(const Vector4D& color)
{
	this->currentDiffuseColor = color;

	if (!isBaseDiffuseSet)
	{
		this->baseDiffuseColor = color;
		isBaseDiffuseSet = true;
	}
}

const Vector4D LightSpot::GetDiffuse()
{
	return currentDiffuseColor;
}

void LightSpot::SetSpecular(const Vector4D& color)
{
	this->currentSpecularColor = color;

	if (!isBaseSpecularSet)
	{
		this->baseSpecularColor = color;
		isBaseSpecularSet = true;
	}
}

const Vector4D LightSpot::GetSpecular()
{
	return currentSpecularColor;
}

const bool LightSpot::IsVisible()
{
	return isVisible;
}

void LightSpot::SetVisibility(const bool& option)
{
	isVisible = option;
}

void LightSpot::SetCollider(ICollider* ptr)
{
	this->collider = ptr;
}

ICollider* LightSpot::GetCollider()
{
	return this->collider;
}

const std::string LightSpot::GetName()
{
	return this->name;
}

void LightSpot::SetName(const std::string& name)
{
	this->name = name;
}

unsigned short LightSpot::GetEnumID()
{
	return 1;
}

const std::string LightSpot::GetType()
{
	return "LightSpot";
}

void LightSpot::Update(const double& deltaTime)
{
	if (collider)
	{
		collider->Transform(GetWorldMatrix());
	}
}

const unsigned long long LightSpot::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_LIGHTSPOT;
}

Matrix4D LightSpot::GetWorldMatrix()
{
	Matrix4D m = Matrix4D(1.0f);

	//Translation transform
	Matrix4D matTrans = GetTranslationMatrix();
	m = m * matTrans;

	Matrix4D rotateMat = GetRotationMatrix();
	m = m * rotateMat;

	//Scale transform
	Matrix4D scale = GetScaleMatrix();
	m = m * scale;

	return m;
}

Matrix4D LightSpot::GetTranslationMatrix()
{
	return glm::translate(Matrix4D(1.0f), transform->GetPosition());
}

Matrix4D LightSpot::GetScaleMatrix()
{
	return  glm::scale(Matrix4D(1.0f), Vector3D(1.0f, 1.0f, 1.0f));
}

Matrix4D LightSpot::GetRotationMatrix()
{
	//TODO:: convert light direction to rotation
	Vector3D r = Vector3D(0.0f, 0.0f, 0.0f);
	Quaternion q = MathHelper::EulerAnglesToQuaternion(r);

	return Matrix4D(q);
}

ICloneable* LightSpot::Clone()
{
	LightSpot* clone = new LightSpot();
	clone->name = this->name + "_" + StringHelpers::GenerateRandomString(4);
	clone->baseDiffuseColor = this->baseDiffuseColor;
	clone->baseSpecularColor = this->baseSpecularColor;
	clone->currentDiffuseColor = this->currentDiffuseColor;
	clone->currentSpecularColor = this->currentSpecularColor;
	clone->isVisible = this->isVisible;
	clone->cutOffDistance = this->cutOffDistance;
	clone->constAttenuation = this->constAttenuation;
	clone->linearAttenuation = this->linearAttenuation;
	clone->quadraticAttenuation = this->quadraticAttenuation;
	clone->direction = this->direction;
	clone->innerAngle = this->innerAngle;
	clone->outerAngle = this->outerAngle;

	clone->transform = (Transform*)transform->Clone();

	clone->collider = (ICollider*)collider->Clone();

	return clone;
}