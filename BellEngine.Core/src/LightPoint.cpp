#include "LightPoint.h"
#include <MathHelper.h>
#include <StringHelpers.h>
#include <BellEngineID.h>
#include <Transform.h>

LightPoint::LightPoint()
{
	constAttenuation = 0.0f;
	linearAttenuation = 0.0f;
	quadraticAttenuation = 0.0f;
	cutOffDistance = 0.0f;
	SetTransform(new Transform());
}

LightPoint::~LightPoint()
{
	Dispose();
}

void LightPoint::SetConstantAttenuation(const float& value)
{
	this->constAttenuation = value;
}

const float LightPoint::GetConstantAttenuation()
{
	return this->constAttenuation;
}

void LightPoint::SetLinearAttenuation(const float& value)
{
	this->linearAttenuation = value;
}

const float LightPoint::GetLinearAttenuation()
{
	return this->linearAttenuation;
}

void LightPoint::SetQuadraticAttenuation(const float& value)
{
	this->quadraticAttenuation = value;
}

const float LightPoint::GetQuadraticAttenuation()
{
	return this->quadraticAttenuation;
}

void LightPoint::SetCutoffDistance(const float& value)
{
	this->cutOffDistance = value;
}

const float LightPoint::GetCutoffDistance()
{
	return this->cutOffDistance;
}

const bool LightPoint::IsActive()
{
	return this->isActive;
}

void LightPoint::Activate(const bool& value)
{
	this->isActive = value;
}

void LightPoint::Dispose()
{
	DisposePointer(transform);
	DisposePointer(collider);
}

const Vector4D LightPoint::GetBaseDiffuseColor()
{
	return this->baseDiffuseColor;
}

const Vector4D LightPoint::GetBaseSpecularColor()
{
	return this->baseSpecularColor;
}

void LightPoint::SetDiffuse(const Vector4D& color)
{
	this->currentDiffuseColor = color;

	if (!isBaseDiffuseSet)
	{
		this->baseDiffuseColor = color;
		isBaseDiffuseSet = true;
	}
}

const Vector4D LightPoint::GetDiffuse()
{
	return currentDiffuseColor;
}

void LightPoint::SetSpecular(const Vector4D& color)
{
	this->currentSpecularColor = color;

	if (!isBaseSpecularSet)
	{
		this->baseSpecularColor = color;
		isBaseSpecularSet = true;
	}
}

const Vector4D LightPoint::GetSpecular()
{
	return currentSpecularColor;
}

const bool LightPoint::IsVisible()
{
	return isVisible;
}

void LightPoint::SetVisibility(const bool& option)
{
	isVisible = option;
}

void LightPoint::SetCollider(ICollider* ptr)
{
	this->collider = ptr;
}

ICollider* LightPoint::GetCollider()
{
	return this->collider;
}

const std::string LightPoint::GetName()
{
	return this->name;
}

void LightPoint::SetName(const std::string& name)
{
	this->name = name;
}

unsigned short LightPoint::GetEnumID()
{
	return 0;
}

const std::string LightPoint::GetType()
{
	return "LightPoint";
}

void LightPoint::Update(const double& deltaTime)
{
	if (collider)
	{
		collider->Transform(GetWorldMatrix());
	}
}

const unsigned long long LightPoint::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_LIGHTPOINT;
}

Matrix4D LightPoint::GetWorldMatrix()
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

Matrix4D LightPoint::GetTranslationMatrix()
{
	return glm::translate(Matrix4D(1.0f), transform->GetPosition());
}

Matrix4D LightPoint::GetScaleMatrix()
{
	return  glm::scale(Matrix4D(1.0f), Vector3D(1.0f, 1.0f, 1.0f));
}

Matrix4D LightPoint::GetRotationMatrix()
{
	Vector3D r = Vector3D(0.0f, 0.0f, 0.0f);
	Quaternion q = MathHelper::EulerAnglesToQuaternion(r);

	return Matrix4D(q);
}

ICloneable* LightPoint::Clone()
{
	LightPoint* clone = new LightPoint();
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
	clone->transform = (ITransform*)transform->Clone();

	clone->collider = (ICollider*)collider->Clone();

	return clone;
}

