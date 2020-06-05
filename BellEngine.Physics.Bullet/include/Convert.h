#pragma once
#include <MathLibrary.h>
#include "../physics/bullet/btBulletCollisionCommon.h"
#include "../physics/bullet/btBulletDynamicsCommon.h"

class Convert
{
public:
	inline static btVector3 Vec3ToBtVec3(const Vector3D& v)
	{
		return btVector3(v.x, v.y, v.z);
	}
	inline static Vector3D BtVec3ToVec3(const btVector3& v)
	{
		return Vector3D(v.x(), v.y(), v.z());
	}

	inline static void BtTransformToMat4(const btTransform& transformIn, Matrix4D& transformOut)
	{
		transformIn.getOpenGLMatrix(&transformOut[0][0]);
	}

    inline static Quaternion BtQuatToQuat(const btQuaternion& q)
    { 
        return Quaternion(q.getW(), q.getX(), q.getY(), q.getZ());
    }

    inline static btQuaternion QuatToBtQuat(const Quaternion& q)
    {
        return btQuaternion(q.x, q.y, q.z, q.w); 
    }

    inline static btMatrix3x3 glmToBullet(const Matrix3D& m)
    { 
        return btMatrix3x3(m[0][0], m[1][0], m[2][0], m[0][1], m[1][1], m[2][1], m[0][2], m[1][2], m[2][2]);
    }

    // btTransform does not contain a full 4x4 matrix, so this transform is lossy.
    // Affine transformations are OK but perspective transformations are not.
   /* inline static btTransform glmToBullet(const glm::mat4& m)
    {
        glm::mat3 m3(m);
        return btTransform(glmToBullet(m3), glmToBullet(glm::vec3(m[3][0], m[3][1], m[3][2])));
    }*/

    inline static Matrix4D bulletToGlm(const btTransform& t)
    {
        Matrix4D m(0.0f);
        const btMatrix3x3& basis = t.getBasis();
        // rotation
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                m[c][r] = basis[r][c];
            }
        }
        // traslation
        btVector3 origin = t.getOrigin();
        m[3][0] = origin.getX();
        m[3][1] = origin.getY();
        m[3][2] = origin.getZ();
        // unit scale
        m[0][3] = 0.0f;
        m[1][3] = 0.0f;
        m[2][3] = 0.0f;
        m[3][3] = 1.0f;
        return m;
    }
};