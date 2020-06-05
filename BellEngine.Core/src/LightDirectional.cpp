#include "LightDirectional.h"
#include <StringHelpers.h>
#include <MathHelper.h>
#include "Transform.h"
#include <BellEngineID.h>

LightDirectional::LightDirectional()
{
	direction = Vector3D(0.0f, -1.0f, 0.0f);
	isActive = true;
	baseSpecularColor = currentDiffuseColor = baseDiffuseColor = currentSpecularColor = Vector4D();
	isBaseDiffuseSet = isBaseSpecularSet = false;
	isVisible = true;
	collider = nullptr;
	transform = nullptr;

	SetTransform(new Transform());
}

LightDirectional::~LightDirectional()
{
	Dispose();
}

const bool LightDirectional::IsActive()
{
	return this->isActive;
}

void LightDirectional::Activate(const bool& value)
{
	this->isActive = value;
}

void LightDirectional::Dispose()
{
	DisposePointer(transform);
	DisposePointer(collider);
}

const Vector4D LightDirectional::GetBaseDiffuseColor()
{
	return this->baseDiffuseColor;
}

const Vector4D LightDirectional::GetBaseSpecularColor()
{
	return this->baseSpecularColor;
}

void LightDirectional::SetDiffuse(const Vector4D& color)
{
	this->currentDiffuseColor = color;

	if (!isBaseDiffuseSet)
	{
		this->baseDiffuseColor = color;
		isBaseDiffuseSet = true;
	}
}

const Vector4D LightDirectional::GetDiffuse()
{
	return currentDiffuseColor;
}

void LightDirectional::SetSpecular(const Vector4D& color)
{
	this->currentSpecularColor = color;

	if (!isBaseSpecularSet)
	{
		this->baseSpecularColor = color;
		isBaseSpecularSet = true;
	}
}

const Vector4D LightDirectional::GetSpecular()
{
	return currentSpecularColor;
}

const bool LightDirectional::IsVisible()
{
	return isVisible;
}

void LightDirectional::SetVisibility(const bool& option)
{
	isVisible = option;
}

void LightDirectional::SetCollider(ICollider* ptr)
{
	this->collider = ptr;
}

ICollider* LightDirectional::GetCollider()
{
	return this->collider;
}

void LightDirectional::SetDirection(const Vector3D& direction)
{
	this->direction = direction;
}

const Vector3D LightDirectional::GetDirection()
{
	return this->direction;
}

unsigned short LightDirectional::GetEnumID()
{
	return 2;
}

const std::string LightDirectional::GetType()
{
	return "LightDirectional";
}

void LightDirectional::Update(const double& deltaTime)
{
	if (collider)
	{
		collider->Transform(GetWorldMatrix());
	}
}

const unsigned long long LightDirectional::GetTypeID()
{
	return ENGINE_OBJ_ID_GPH_LIGHTDIRECTIONAL;
}

Matrix4D LightDirectional::GetWorldMatrix()
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

Matrix4D LightDirectional::GetTranslationMatrix()
{
	return glm::translate(Matrix4D(1.0f), transform->GetPosition());
}

Matrix4D LightDirectional::GetScaleMatrix()
{
	return  glm::scale(Matrix4D(1.0f), Vector3D(1.0f, 1.0f, 1.0f));
}

Matrix4D LightDirectional::GetRotationMatrix()
{
	Vector3D r = Vector3D(0.0f, 0.0f, 0.0f);
	Quaternion q = MathHelper::EulerAnglesToQuaternion(r);

	return Matrix4D(q);
}

ICloneable* LightDirectional::Clone()
{
	LightDirectional* clone = new LightDirectional();
	clone->name = this->name + "_" + StringHelpers::GenerateRandomString(4);
	clone->baseDiffuseColor = this->baseDiffuseColor;
	clone->baseSpecularColor = this->baseSpecularColor;
	clone->currentDiffuseColor = this->currentDiffuseColor;
	clone->currentSpecularColor = this->currentSpecularColor;
	clone->direction = this->direction;
	clone->isVisible = this->isVisible;	
	clone->transform = (ITransform*)transform->Clone();

	clone->collider = (ICollider*)collider->Clone();

	return clone;
}

const std::string LightDirectional::GetName()
{
	return name;
}

void LightDirectional::SetName(const std::string& name)
{
	this->name = name;
}
