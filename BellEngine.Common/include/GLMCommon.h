#pragma once
#pragma warning(push, 0)        
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // Vector3D
#include <glm/vec4.hpp> // Vector4D
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtx/quaternion.hpp>	
#include <glm/gtx/closest_point.hpp>
#pragma warning(pop)

typedef glm::vec1 Vector1D;
typedef glm::vec2 Vector2D;
typedef glm::vec3 Vector3D;
typedef glm::vec4 Vector4D;
typedef glm::mat2 Matrix2D;
typedef glm::mat3 Matrix3D;
typedef glm::mat4 Matrix4D;
typedef glm::quat Quaternion;