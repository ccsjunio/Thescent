#pragma once

constexpr unsigned long long DISABLEDEFAULTRESPONSE_SphereToSphere = 0x0000000000000001;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_SphereToPlane = 0x0000000000000010;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_SphereToCapsule = 0x0000000000000100;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_SphereToBox = 0x0000000000001000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_CapsuleToCapsule = 0x0000000000010000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_CapsuleToPlane = 0x0000000000100000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_CapsuleToBox = 0x0000000001000000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_BoxToBox = 0x0000000010000000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_BoxToPlane = 0x0000000100000000;
constexpr unsigned long long DISABLEDEFAULTRESPONSE_MeshToMesh = 0x0000001000000000;