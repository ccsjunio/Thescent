#pragma once
#include <MathLibrary.h>
#include <vector>
#include <limits>       // std::numeric_limits

class MathHelper
{
public:
	template <class T>T 
	inline static RandomInRange(T min, T max)
	{
		double value = min + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (static_cast<double>(max - min))));

		return static_cast<T>(value);
	};

	inline static Vector3D Rotate3D_X(const Vector3D& vector, float angle) 
	{
		return Rotate3D(vector, Vector3D(1.0f, 0.0f, 0.0f), angle);
	};

	inline static Vector3D Rotate3D_Y(const Vector3D& vector, float angle)
	{
		return Rotate3D(vector, Vector3D(0.0f, 1.0f, 0.0f), angle);
	};

	inline static Vector3D Rotate3D_Z(const Vector3D& vector, float angle)
	{
		return Rotate3D(vector, Vector3D(0.0f, 0.0f, 1.0f), angle);
	};

	inline static Vector3D Rotate3D(const Vector3D& vector, const Vector3D& angles)
	{
		Vector3D v = vector;

		v = Rotate3D_X(v, angles.x);
		v = Rotate3D_Y(v, angles.y);
		v = Rotate3D_Z(v, angles.z);

		return v;
	};

	inline static Vector3D Rotate3D(const Vector3D& vector, const Vector3D& axis, float angle)
	{
		float cos_theta = cos(glm::radians(angle));
		float sin_theta = sin(glm::radians(angle));

		Vector3D rotated = (vector * cos_theta) + (glm::cross(axis, vector) * sin_theta) + (axis * glm::dot(axis, vector)) * (1 - cos_theta);

		return rotated;
	};

	inline static Quaternion EulerAnglesToQuaternion(const Vector3D& eulerAngles)
	{
		Vector3D radians = Vector3D();
		radians.x = glm::radians(eulerAngles.x);
		radians.y = glm::radians(eulerAngles.y);
		radians.z = glm::radians(eulerAngles.z);

		return Quaternion(radians);
	};

	inline static Vector3D QuaternionToEulerAngles(const Quaternion& quaternion)
	{
		Vector3D radians = glm::eulerAngles(quaternion);

		Vector3D degrees = Vector3D();
		degrees.x = glm::degrees(radians.x);
		degrees.y = glm::degrees(radians.y);
		degrees.z = glm::degrees(radians.z);

		return degrees;
	};

	inline static Vector3D GetScaleFromWorldMatrix(const Matrix4D& worldMatrix)
	{
		// length2 returns length squared i.e. v·v
		// no square root involved
		return Vector3D(glm::length2(glm::vec3(worldMatrix[0])), glm::length2(glm::vec3(worldMatrix[1])), glm::length2(glm::vec3(worldMatrix[2])));
	};
	//Finds the 2D area of a triangle.
	inline static float GetTriangleArea2D(const float& x1, const float& y1, const float& x2, const float& y2, const float& x3, const float& y3)
	{
		return (x1 - x2) * (y2 - y3) - (x2 - x3) * (y1 - y2);
	}
	//Compute barycentric coordinates (u, v, w) for point p with respect to triangle (a, b, c).
	inline static void FindBarycentricCoordinates(const Vector3D& p, const Vector3D& trianglePointA, const Vector3D& trianglePointB, const Vector3D& trianglePointC, float& u, float& v, float& w)
	{
		Vector3D a = trianglePointA;
		Vector3D b = trianglePointB;
		Vector3D c = trianglePointC;

		// Unnormalized triangle normal
		Vector3D m = glm::cross(b - a, c - a);
		// Nominators and one-over-denominator for u and v ratios
		float nu, nv, ood;
		// Absolute components for determining projection plane
		float x = abs(m.x), y = abs(m.y), z = abs(m.z);
		// Compute areas in plane of largest projection
		if (x >= y && x >= z)
		{
			// x is largest, project to the yz plane
			nu = GetTriangleArea2D(p.y, p.z, b.y, b.z, c.y, c.z); // Area of PBC in yz plane
			nv = GetTriangleArea2D(p.y, p.z, c.y, c.z, a.y, a.z); // Area of PCA in yz plane
			ood = 1.0f / m.x; // 1/(2*area of ABC in yz plane)52 Chapter 3 A Math and Geometry Primer
		}
		else if (y >= x && y >= z)
		{
			// y is largest, project to the xz plane
			nu = GetTriangleArea2D(p.x, p.z, b.x, b.z, c.x, c.z);
			nv = GetTriangleArea2D(p.x, p.z, c.x, c.z, a.x, a.z);
			ood = 1.0f / -m.y;
		}
		else
		{
			// z is largest, project to the xy plane
			nu = GetTriangleArea2D(p.x, p.y, b.x, b.y, c.x, c.y);
			nv = GetTriangleArea2D(p.x, p.y, c.x, c.y, a.x, a.y);
			ood = 1.0f / m.z;
		}
		u = nu * ood;
		v = nv * ood;
		w = 1.0f - u - v;
	}
	//Test if point p is contained in triangle.
	inline static bool IsPointInTriangle(const Vector3D& p, const Vector3D& trianglePointA, const Vector3D& trianglePointB, const Vector3D& trianglePointC)
	{
		float u, v, w;
		FindBarycentricCoordinates(p, trianglePointA, trianglePointB, trianglePointC, u, v, w);

		return v >= 0.0f && w >= 0.0f && (v + w) <= 1.0f;
	}

	inline static bool PointIntersectVolume(const Vector3D& point, const Vector3D& volumeMax, const Vector3D& volumeMin)
	{
		return
			point.x > volumeMin.x &&
			point.x < volumeMax.x &&
			point.y > volumeMin.y &&
			point.y < volumeMax.y &&
			point.z > volumeMin.z &&
			point.z < volumeMax.z;
	}

	inline static Vector3D ClosestPtPointTriangle(const Vector3D& p, const Vector3D& trianglePointA, const Vector3D& trianglePointB, const Vector3D& trianglePointC)
	{
		Vector3D ab = trianglePointB - trianglePointA;
		Vector3D ac = trianglePointC - trianglePointA;
		Vector3D bc = trianglePointC - trianglePointB;

		// Compute parametric position s for projection P' of P on AB,
		// P' = A + s*AB, s = snom/(snom+sdenom)
		float snom = glm::dot(p - trianglePointA, ab), sdenom = glm::dot(p - trianglePointB, trianglePointA - trianglePointB);

		// Compute parametric position t for projection P' of P on AC,
		// P' = A + t*AC, s = tnom/(tnom+tdenom)
		float tnom = glm::dot(p - trianglePointA, ac), tdenom = glm::dot(p - trianglePointC, trianglePointA - trianglePointC);

		if (snom <= 0.0f && tnom <= 0.0f) return trianglePointA; // Vertex region early out

		// Compute parametric position u for projection P' of P on BC,
		// P' = B + u*BC, u = unom/(unom+udenom)
		float unom = glm::dot(p - trianglePointB, bc), udenom = glm::dot(p - trianglePointC, trianglePointB - trianglePointC);

		if (sdenom <= 0.0f && unom <= 0.0f) return trianglePointB; // Vertex region early out
		if (tdenom <= 0.0f && udenom <= 0.0f) return trianglePointC; // Vertex region early out


		// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
		Vector3D n = glm::cross(trianglePointB - trianglePointA, trianglePointC - trianglePointA);
		float vc = glm::dot(n, glm::cross(trianglePointA - p, trianglePointB - p));
		// If P outside AB and within feature region of AB,
		// return projection of P onto AB
		if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
			return trianglePointA + snom / (snom + sdenom) * ab;

		// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
		float va = glm::dot(n, glm::cross(trianglePointB - p, trianglePointC - p));
		// If P outside BC and within feature region of BC,
		// return projection of P onto BC
		if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
			return trianglePointB + unom / (unom + udenom) * bc;

		// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
		float vb = glm::dot(n, glm::cross(trianglePointC - p, trianglePointA - p));
		// If P outside CA and within feature region of CA,
		// return projection of P onto CA
		if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
			return trianglePointA + tnom / (tnom + tdenom) * ac;

		// P must project inside face region. Compute Q using barycentric coordinates
		float u = va / (va + vb + vc);
		float v = vb / (va + vb + vc);
		float w = 1.0f - u - v; // = vc / (va + vb + vc)
		return u * trianglePointA + v * trianglePointB + w * trianglePointC;
	}

	inline static bool FindMaxMinPoints(const std::vector<Vector3D>& points, Vector3D& max, Vector3D& min)
	{
		max = Vector3D(std::numeric_limits<float>::lowest());
		min = Vector3D(std::numeric_limits<float>::max());

		if (points.size() != 0)
		{
			for (size_t i = 0; i < points.size(); i++)
			{
				//Min
				if (points[i].x < min.x)
				{
					min.x = points[i].x;
				}
				if (points[i].y < min.y)
				{
					min.y = points[i].y;
				}
				if (points[i].z < min.z)
				{
					min.z = points[i].z;
				}
				//Max
				if (points[i].x > max.x)
				{
					max.x = points[i].x;
				}
				if (points[i].y > max.y)
				{
					max.y = points[i].y;
				}
				if (points[i].z > max.z)
				{
					max.z = points[i].z;
				}
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	
	// Given segment ab and point c, computes closest point d on ab.
	// Also returns t for the position of d, d(t) = a + t*(b - a)
	inline static void ClosestPtPointSegment(const Vector3D& c, const Vector3D& a, const Vector3D& b, float& t, Vector3D& d)
	{
		Vector3D ab = b - a;
		// Project c onto ab, computing parameterized position d(t) = a + t*(b – a)
		t = glm::dot(c - a, ab) / glm::dot(ab, ab);
		// If outside segment, clamp t (and therefore d) to the closest endpoint
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;
		// Compute projected position from the clamped t
		d = a + t * ab;
	}
	inline static void ClosestPtPointSegment2(const Vector3D& c, const Vector3D& a, const Vector3D& ab, float& t, Vector3D& d)
	{
		// Project c onto ab, computing parameterized position d(t) = a + t*(b – a)
		t = glm::dot(c - a, ab) / glm::dot(ab, ab);
		// If outside segment, clamp t (and therefore d) to the closest endpoint
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;
		// Compute projected position from the clamped t
		d = a + t * ab;
	}
	//Since we have 3 possible outcomes, a short will be used to return either 0, 1 or 2
	//This can be replaced with just a bool, depending on how the special case (point on plane) wants to be handled
	inline static short PlaneHalfSpaceTest(const Vector3D& vecTestPoint, const Vector3D& vecNormal, const Vector3D& vecPointOnPlane)
	{
		//Calculate a vector from the point on the plane to our test point
		Vector3D vecTemp(vecTestPoint - vecPointOnPlane);

		//Calculate the distance: dot product of the new vector with the plane's normal
		float fDist(glm::dot(vecTemp, vecNormal));

		if (fDist > FLT_EPSILON)
		{
			//Point is in front of the plane
			return 0;
		}
		else if (fDist < -FLT_EPSILON)
		{
			//Point is behind the plane
			return 1;
		}
		//If neither of these were true, then the point is on the plane
		return 2;
	}

	inline static bool IsPointWithinPlane(const Vector3D point, const Vector3D& planeNormal, const float& height, const std::vector<Vector3D>& planeVertices)
	{	
		//Add vertices to normal to make a "box" on top of the plane
		std::vector<Vector3D> boxVertices = planeVertices;
		for (size_t i = 0; i < 4; i++)
		{
			Vector3D v = planeVertices[i];
			Vector3D vBox = v + planeNormal * height;
			boxVertices.push_back(vBox);
		}

		//Find min and max
		Vector3D min(0.0f), max(0.0f);
		if (FindMaxMinPoints(boxVertices, max, min))
		{
			//Use max and min to find out if the point is within the 'virtual box'
			return PointIntersectVolume(point, max, min);
		}
			   
		return false;
	}

	inline static int IntersectRayPlane(const Vector3D& rayStart, const Vector3D& rayEnd, const Vector3D& planeNormal, const float& planeConst, float& minRayLength, Vector3D& pointOfIntersection)
	{
		// Compute the t value for the directed line ab intersecting the plane
		Vector3D ab = rayEnd - rayStart;
		minRayLength = (planeConst - glm::dot(planeNormal, rayStart)) / glm::dot(planeNormal, ab);
		
		// If t in [0..1] compute and return intersection point
		if (minRayLength >= 0.0f && minRayLength <= 1.0f) 
		{
			pointOfIntersection = rayStart + minRayLength * ab;
			return 1;
		}
	
		// Else no intersection
		return 0;
	}
	// Intersect segment ab against plane of triangle def. If intersecting,
	// return t value and position q of intersection
	inline static int IntersectRayPlane(const Vector3D& rayStart, const Vector3D& rayEnd, const Vector3D& planePA, const Vector3D& planePB, const Vector3D& planePC, float& minRayLength, Vector3D& pointOfIntersection)
	{
		Vector3D planeNormal = glm::cross(planePB - planePA, planePC - planePA);
		float planeConst = glm::dot(planeNormal, planePA);

		return IntersectRayPlane(rayStart, rayEnd, planeNormal, planeConst, minRayLength, pointOfIntersection);
	}

	// Test if ray r = p + td intersects sphere s
	inline static int TestRaySphere(const Vector3D& rayStart, const Vector3D& direction, const Vector3D& sphereCenter, const float& sphereRadius)
	{
		Vector3D m = rayStart - sphereCenter;
		float c = glm::dot(m, m) - sphereRadius * sphereRadius;
		// If there is definitely at least one real root, there must be an intersection
		if (c <= 0.0f) return 1;
		float b = glm::dot(m, direction);
		// Early exit if ray origin outside sphere and ray pointing away from sphere
		if (b > 0.0f) return 0;
		float disc = b * b - c;
		// A negative discriminant corresponds to ray missing sphere
		if (disc < 0.0f) return 0;
		// Now ray must hit sphere
		return 1;
	}

	// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
	// returns t value of intersection and intersection point q
	inline static int IntersectRaySphere(const Vector3D& p, const Vector3D& d, const Vector3D& sphereCenter, const float& sphereRadius, float& minRayLength, Vector3D& pointOfIntersection)
	{
		Vector3D m = p - sphereCenter;
		float b = glm::dot(m, d);
		float c = glm::dot(m, m) - sphereRadius * sphereRadius;
		// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
		if (c > 0.0f && b > 0.0f) return 0;
		float discr = b * b - c;
		// A negative discriminant corresponds to ray missing sphere
		if (discr < 0.0f) return 0;
		// Ray now found to intersect sphere, compute smallest t value of intersection
		minRayLength = -b - sqrt(discr);
		// If t is negative, ray started inside sphere so clamp t to zero
		if (minRayLength < 0.0f) minRayLength = 0.0f;
		
		pointOfIntersection = p + (minRayLength * d);
		return 1;
	}

	// Test if segment specified by points p0 and p1 intersects a box
	inline static int TestRayBox(const Vector3D& p0, const Vector3D& p1, const Vector3D& boxCenter, const Vector3D& boxMax, const Vector3D& boxMin)
	{
		Vector3D c = boxCenter;// (boxMin + boxMax) * 0.5f; // Box center-point
		Vector3D e = boxMin - c; // Box halflength extents
		Vector3D m = (p0 + p1) * 0.5f; // Segment midpoint
		Vector3D d = p1 - m; // Segment halflength vector
		m = m - c; // Translate box and segment to origin
		// Try world coordinate axes as separating axes
		float adx = abs(d.x);
		if (abs(m.x) > e.x + adx) return 0;
		float ady = abs(d.y);
		if (abs(m.y) > e.y + ady) return 0;
		float adz = abs(d.z);
		if (abs(m.z) > e.z + adz) return 0;
		// Add in an epsilon term to counteract arithmetic errors when segment is
		// (near) parallel to a coordinate axis (see text for detail)
		adx += FLT_EPSILON; ady += FLT_EPSILON; adz += FLT_EPSILON;
		// Try cross products of segment direction vector with coordinate axes
		if (abs(m.y * d.z - m.z * d.y) > e.y* adz + e.z * ady) return 0;
		if (abs(m.z * d.x - m.x * d.z) > e.x* adz + e.z * adx) return 0;
		if (abs(m.x * d.y - m.y * d.x) > e.x* ady + e.y * adx) return 0;
		// No separating axis found; segment must be overlapping AABB
		return 1;
	}

	// Intersect ray R(t) = p + t*d against AABB a. When intersecting,
	// return intersection distance tmin and point q of intersection
	inline static int IntersectRayBox(const Vector3D& rayStart, const Vector3D& direction, const Vector3D& boxMax, const Vector3D& boxMin, float& minRayLength, Vector3D& pointOfIntersection)
	{
		minRayLength = 0.0f; // set to -FLT_MAX to get first hit on line
		float tmax = FLT_MAX; // set to max distance ray can travel (for segment)5.3 Intersecting Lines, Rays, and (Directed) Segments 181
		// For all three slabs
		for (int i = 0; i < 3; i++) 
		{
			if (abs(direction[i]) < FLT_EPSILON) 
			{
				// Ray is parallel to slab. No hit if origin not within slab
				if (rayStart[i] < boxMin[i] || rayStart[i] > boxMax[i]) return 0;
			}
			else
			{
				// Compute intersection t value of ray with near and far plane of slab
				float ood = 1.0f / direction[i];
				float t1 = (boxMin[i] - rayStart[i]) * ood;
				float t2 = (boxMax[i] - rayStart[i]) * ood;
				// Make t1 be intersection with near plane, t2 with far plane
				if (t1 > t2)
				{
					std::swap(t1, t2);
					/*float temp = t1;
					t1 = t2;
					t2 = temp;*/
				}
				// Compute the intersection of slab intersection intervals
				if (t1 > minRayLength) minRayLength = t1;
				if (t2 > tmax) tmax = t2;
				// Exit with no collision as soon as slab intersection becomes empty
				if (minRayLength > tmax) return 0;
			}
		}

		// Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
		pointOfIntersection = rayStart + (direction * minRayLength);
		return 1;
	}

	inline static bool IntersectRayBox2(const Vector3D& rayStart, const Vector3D& direction, const Vector3D& boxMax, const Vector3D& boxMin)
	{
		float tmin = (boxMin.x - rayStart.x) / direction.x;
		float tmax = (boxMax.x - rayStart.x) / direction.x;

		if (tmin > tmax) std::swap(tmin, tmax);

		float tymin = (boxMin.y - rayStart.y) / direction.y;
		float tymax = (boxMax.y - rayStart.y) / direction.y;

		if (tymin > tymax) std::swap(tymin, tymax);

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
			tmin = tymin;

		if (tymax < tmax)
			tmax = tymax;

		float tzmin = (boxMin.z - rayStart.z) / direction.z;
		float tzmax = (boxMax.z - rayStart.z) / direction.z;

		if (tzmin > tzmax) std::swap(tzmin, tzmax);

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
			tmin = tzmin;

		if (tzmax < tmax)
			tmax = tzmax;

		return true;
	}

	inline static int TestSphereToSphere(const Vector3D& s1pos, const float& s1radius, const Vector3D& s2pos, const float& s2radius, float& distance)
	{

		//Calculate the squared distance between the centers of both spheres
		Vector3D vecDist = s2pos - s1pos;
		auto fDistSq = glm::dot(vecDist, vecDist);

		//Calculate the squared sum of both radii
		float fRadiiSumSquared = s1radius + s2radius;
		fRadiiSumSquared *= fRadiiSumSquared;

		distance = fDistSq - fRadiiSumSquared;

		//Check for collision
		//If the distance squared is less than or equal to the square sum
		//of the radii, then we have a collision
		if (fDistSq <= fRadiiSumSquared)
		{
			return true;
		}			

		//If not, then return false
		return false;
	}

	// Intersect segment S(t)=sa+t(sb-sa), 0<=t<=1 against cylinder specified by p, q and r
	inline static int IntersectRayCylinder(const Vector3D& rayStart, const Vector3D& rayEnd, const Vector3D& cylinderStart, const Vector3D& cylinderEnd, const float& cylinderRadius, float& t)
	{
		Vector3D d = cylinderEnd - cylinderStart, m = rayStart - cylinderStart, n = rayEnd - rayStart;
		float md = glm::dot(m, d);
		float nd = glm::dot(n, d);
		float dd = glm::dot(d, d);
		// Test if segment fully outside either endcap of cylinder
		if (md < 0.0f && md + nd < 0.0f) return 0; // Segment outside ‘p’ side of cylinder
		if (md > dd&& md + nd > dd) return 0;     // Segment outside ‘q’ side of cylinder
		float nn = glm::dot(n, n);
		float mn = glm::dot(m, n);
		float a = dd * nn - nd * nd;
		float k = glm::dot(m, m) - cylinderRadius * cylinderRadius;
		float c = dd * k - md * md;
		if (fabs(a) < FLT_EPSILON)
		{
			// Segment runs parallel to cylinder axis
			if (c > 0.0f) return 0; // ‘a’ and thus the segment lie outside cylinder
			// Now known that segment intersects cylinder; figure out how it intersects
			if (md < 0.0f) t = -mn / nn; // Intersect segment against ‘p’ endcap
			else if (md > dd) t = (nd - mn) / nn; // Intersect segment against ‘q’ endcap
			else t = 0.0f; // ‘a’ lies inside cylinder
			return 1;
		}
		float b = dd * mn - nd * md;
		float discr = b * b - a * c;
		if (discr < 0.0f) return 0; // No real roots; no intersection
		t = (-b - sqrt(discr)) / a;
		if (t < 0.0f || t > 1.0f) return 0; // Intersection lies outside segment
		if (md + t * nd < 0.0f) 
		{
			// Intersection outside cylinder on ‘p’ side
			if (nd <= 0.0f) return 0; // Segment pointing away from endcap
			t = -md / nd;
			// Keep intersection if Dot(S(t) - p, S(t) - p) <= r^2
			return k + 2 * t * (mn + t * nn) <= 0.0f;
		}
		else if (md + t * nd > dd) 
		{
			// Intersection outside cylinder on ‘q’ side
			if (nd >= 0.0f) return 0; // Segment pointing away from endcap
			t = (dd - md) / nd;
			// Keep intersection if Dot(S(t) - q, S(t) - q) <= r^2
			return k + dd - 2 * md + t * (2 * (mn - nd) + t * nn) <= 0.0f;
		}
		// Segment intersects cylinder between the end-caps; t is correct
		return 1;
	}
	
	inline static const bool GetPointsInXZFlatCircle(const float& radius, const Matrix4D& worldMatrix, const unsigned int& numberOfPoints, std::vector<Vector3D>& points)
	{
		if (numberOfPoints > 360)
		{
			return false;
		}

		points.clear();
		points.shrink_to_fit();
		float step = 360.0f / (float)numberOfPoints;

		float angle = 0.0f, x = 0.0f, z = 0.0f;
		Vector3D point = Vector3D();
		for (unsigned int i = 0; i < numberOfPoints; i++)
		{
			angle = glm::radians(i * step);
			x = radius * cos(angle) + 0.0f;
			z = radius * sin(angle) + 0.0f;

			point = Vector3D(x, 0.0f, z);
			point = worldMatrix * Vector4D(point, 1.0f);

			points.push_back(point);
		}

		return true;
	};
	inline static const bool GetPointsInXYFlatCircle(const float& radius, const Matrix4D& worldMatrix, const unsigned int& numberOfPoints, std::vector<Vector3D>& points)
	{
		if (numberOfPoints > 360)
		{
			return false;
		}

		points.clear();
		points.shrink_to_fit();
		float step = 360.0f / (float)numberOfPoints;

		float angle = 0.0f, x = 0.0f, y = 0.0f;
		Vector3D point = Vector3D();
		for (unsigned int i = 0; i < numberOfPoints; i++)
		{
			angle = glm::radians(i * step);
			x = radius * cos(angle) + 0.0f;
			y = radius * sin(angle) + 0.0f;

			point = Vector3D(x, y, 0.0f);
			point = worldMatrix * Vector4D(point, 1.0f);

			points.push_back(point);
		}

		return true;
	};
	inline static const bool GetPointsInYZFlatCircle(const float& radius, const Matrix4D& worldMatrix, const unsigned int& numberOfPoints, std::vector<Vector3D>& points)
	{
		if (numberOfPoints > 360)
		{
			return false;
		}

		points.clear();
		points.shrink_to_fit();
		float step = 360.0f / (float)numberOfPoints;

		float angle = 0.0f, z = 0.0f, y = 0.0f;
		Vector3D point = Vector3D();
		for (unsigned int i = 0; i < numberOfPoints; i++)
		{
			angle = glm::radians(i * step);
			y = radius * cos(angle) + 0.0f;
			z = radius * sin(angle) + 0.0f;

			point = Vector3D(0.0f, y, z);
			point = worldMatrix * Vector4D(point, 1.0f);

			points.push_back(point);
		}

		return true;
	};
	
	inline static bool GetPointsInFlatCircle(const float& radius, const Vector3D& center, const float& heightInY, const unsigned int& numberOfPoints, std::vector<Vector3D>& points)
	{
		if (numberOfPoints > 360)
		{
			return false;
		}

		points.clear();
		points.shrink_to_fit();
		float step = 360.0f / (float)numberOfPoints;

		for (unsigned int i = 0; i < numberOfPoints; i++)
		{
			float angle = glm::radians(i * step);
			float x = radius * cos(angle) + 0.0f;
			float z = radius * sin(angle) + 0.0f;
			points.push_back(Vector3D(x, heightInY, z));
		}

		return true;
	};

	// Returns the squared distance between point c and segment ab
	inline static const float SqDistPointSegment(const Vector3D& a, const Vector3D& b, const Vector3D& c)
	{
		Vector3D ab = b - a;
		Vector3D ac = c - a;
		Vector3D bc = c - b;
		float e = glm::dot(ac, ab);
		// Handle cases where c projects outside ab
		if (e <= 0.0f) return glm::dot(ac, ac);
		float f = glm::dot(ab, ab);
		if (e >= f) return glm::dot(bc, bc);
		// Handle cases where c projects onto ab
		return glm::dot(ac, ac) - e * e / f;
	}

	// Clamp n to lie within the range [min, max]
	inline static const float ClampN(const float& n, const float& min, const float& max)
	{
		if (n < min) return min;
		if (n > max) return max;
		return n;
	}
	// Computes closest points C1 and C2 of S1(s)=P1+s*(Q1-P1) and
	// S2(t)=P2+t*(Q2-P2), returning s and t. Function result is squared
	// distance between between S1(s) and S2(t)
	inline static const float ClosestPtSegmentSegment(const Vector3D& p1, const Vector3D& q1, const Vector3D& p2, const Vector3D& q2,
		float& s, float& t, const float& epsilon, Vector3D& c1, Vector3D& c2)
	{
		Vector3D d1 = q1 - p1; // Direction vector of segment S1
		Vector3D d2 = q2 - p2; // Direction vector of segment S2
		Vector3D r = p1 - p2;
		float a = glm::dot(d1, d1); // Squared length of segment S1, always nonnegative
		float e = glm::dot(d2, d2); // Squared length of segment S2, always nonnegative
		float f = glm::dot(d2, r);
		// Check if either or both segments degenerate into points
		if (a <= epsilon && e <= epsilon) 
		{
			// Both segments degenerate into points
			s = t = 0.0f;
			c1 = p1;
			c2 = p2; 
			return glm::dot(c1 - c2, c1 - c2);
		}
		if (a <= epsilon) 
		{
			// First segment degenerates into a point
			s = 0.0f;
			t = f / e; // s = 0 => t = (b*s + f) / e = f / e
			t = ClampN(t, 0.0f, 1.0f);
		}
		else
		{
			float c = glm::dot(d1, r);
			if (e <= epsilon) 
			{
				// Second segment degenerates into a point
				t = 0.0f;
				s = ClampN(-c / a, 0.0f, 1.0f); // t = 0 => s = (b*t - c) / a = -c / a
			}
			else 
			{
				// The general nondegenerate case starts here
				float b = glm::dot(d1, d2);
				float denom = a * e - b * b; // Always nonnegative
				// If segments not parallel, compute closest point on L1 to L2 and
				// clamp to segment S1. Else pick arbitrary s (here 0)
				if (denom != 0.0f) 
				{
					s = ClampN((b * f - c * e) / denom, 0.0f, 1.0f);
				}
				else s = 0.0f;
				// Compute point on L2 closest to S1(s) using
				// t = Dot((P1 + D1*s) - P2,D2) / Dot(D2,D2) = (b*s + f) / e
				t = (b * s + f) / e;
				// If t in [0,1] done. Else clamp t, recompute s for the new value
				// of t using s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1)= (t*b - c) / a
				// and clamp s to [0, 1]
				if (t < 0.0f)
				{
					t = 0.0f;
					s = ClampN(-c / a, 0.0f, 1.0f);
				}
				else if (t > 1.0f)
				{
					t = 1.0f;
					s = ClampN((b - c) / a, 0.0f, 1.0f);
				}
			}
		}
		c1 = p1 + d1 * s;
		c2 = p2 + d2 * t;
		return glm::dot(c1 - c2, c1 - c2);
	}
};