#pragma once
#include <MathLibrary.h>
/*Assimp stuff we don't want to have lurking around.*/
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include <vector>
#include <array>
#include "sVertex_xyz_rgba_n_uv2_bt_4Bones.h"
#include "sModelDrawInfo.h"
#include <ModelDef.h>
#include <ISkinnedMesh.h>
#include <IDisposable.h>

//This was created by the one and only Michael Feeney @ Fanshawe College.
//Inspired by this page: http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html
class SkinnedMeshComponent : public ISkinnedMesh
{
public:
	// Is STATIC const, so can be assigned a value here
	// (non static const have to be set in the cpp file)
	static const int MAX_BONES_PER_VERTEX = 4;
private:
	std::string name;
	bool isActive;
	struct sVertexBoneData
	{
		//std::array<unsigned int, MAX_BONES_PER_VERTEX> ids;
		std::array<float, MAX_BONES_PER_VERTEX> ids;
		std::array<float, MAX_BONES_PER_VERTEX> weights;

		void AddBoneData(unsigned int BoneID, float Weight);
	};

	struct sBoneInfo
	{
		std::string boneName;
		Matrix4D BoneOffset;
		Matrix4D FinalTransformation;
		Matrix4D ObjectBoneTransformation;
	};

	Matrix4D AIMatrixToGLMMatrix(const aiMatrix4x4& mat);

public:
	struct sAnimationInfo
	{
		std::string friendlyName;
		std::string fileName;
		float approximatedDuration = 0.0f;
		const aiScene* pAIScene;
		Assimp::Importer* pImporter;
	};

	std::string fileName;
	std::string friendlyName;
	const aiScene* pScene;
	Assimp::Importer mImporter;
	std::map< std::string /*animation FRIENDLY name*/, sAnimationInfo > mapAnimationFriendlyNameTo_pScene;		// Animations
	std::vector<sVertexBoneData> vecVertexBoneData;	//Bones;
	Matrix4D mGlobalInverseTransformation;
	//	std::vector<sMeshEntry> mMeshEntries;
	std::map<std::string /*BoneName*/, unsigned int /*BoneIndex*/> m_mapBoneNameToBoneIndex;	//mMapping;
	std::vector<sBoneInfo> mBoneInfo;
	unsigned int mNumBones;	//mNums;
	// Mesh and VAO and VBO infor
	//unsigned int m_VBO_ID;				// Vertex buffer object 
	unsigned int m_numberOfVertices;
	//unsigned int m_indexBuf_ID;			// Index buffer referring to VBO
	unsigned int m_numberOfIndices;
	unsigned int m_numberOfTriangles;
	//bool m_bVBO_created;
	//// Remember: a VAO is a VBO+shaderProgram combination (i.e. you need 1 VAO ++PER++ VBO+shaderID)
	//// (this is only an issue if you are using mulitple shaders; if you are using a single "uber" shader, you're fine)
	//unsigned int m_VAO_ID;				// Vertex Array Object
	//unsigned int m_shaderIDMatchingVAO;	// The shader prod ID matching this VBO
	//bool m_bVAO_created;
//	// Actual vertex data
//	vert_XYZ_RGBA_N_STU_TanBi_4Bones* m_pVertexData;
	
	std::string currentAnimation, lastAnimation;
	std::vector<Matrix4D> currentFinalTransformation, currentOffsets, currentObjectBoneTransformation;
	
	SkinnedMeshComponent();
	~SkinnedMeshComponent();

	// mesh we draw
	const bool LoadMeshFromFile(const std::string &friendlyName, const std::string &filename);	
	// Only want animations
	const bool LoadMeshAnimation(const std::string &friendlyName, const std::string &filename);	
	const bool LoadMeshAnimation(const std::string& friendlyName, const std::string& filename, const float& approximatedDuration);

	// Added to assist with the forward kinematics (id and bone names)
	void GetListOfBoneIDandNames( std::vector<std::string> &vecBoneNames );

	// Looks in the animation map and returns the total time
	float FindAnimationTotalTime( std::string animationName );
	
//	// Returns NULL (0) if there is no mesh at that index
//	cMesh* CreateMeshObjectFromCurrentModel( unsigned int meshIndex = 0 );
	sModelDrawInfo* CreateModelDrawInfoObjectFromCurrentModel( unsigned int meshIndex = 0 );
	const bool GetModelDrawInfo(ModelDrawInfo& mdi, unsigned int meshIndex = 0);

//	void BoneTransform(float TimeInSeconds, 
//					   std::vector<Matrix4D> &FinalTransformation, 
//					   std::vector<Matrix4D> &Globals, 
//					   std::vector<Matrix4D> &Offsets);
	const bool BoneTransform(const float& TimeInSeconds, 
					   const std::string& animationName,		// Now we can pick the animation
					   std::vector<Matrix4D> &FinalTransformation, 
					   std::vector<Matrix4D> &Globals, 
					   std::vector<Matrix4D> &Offsets);
	const bool PlayAnimation(const float& TimeInSeconds, const std::string& animationName);

	const unsigned int GetNums() { return this->mNumBones; }

	const float GetDuration();
	const float GetDuration(const std::string& animationName);
	
	bool Initialize();
	
	void CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, aiQuaternion& out);
	void CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out);
	void CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out);

	void CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, Quaternion& out);
	void CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, Vector3D& out);
	void CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, Vector3D& out);

	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	const aiNodeAnim* FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString nodeOrBoneName);
	
	//void ReadNodeHeirarchy(float AnimationTime, 
	//					   const aiNode* pNode, 
	//					   const Matrix4D &parentTransformMatrix);

	void ReadNodeHeirarchy(float AnimationTime, 
						   std::string animationName,		// Now with more "choose animation"
						   const aiNode* pNode, 
						   const Matrix4D &parentTransformMatrix);

	void LoadBones(const aiMesh* Mesh, std::vector<sVertexBoneData> &Bones);
	
	void Dispose();
	const std::string GetName();
	void SetName(const std::string& name);
	const std::string GetType();

	ICloneable* Clone();
	const bool IsActive();
	void Activate(const bool& value = true);
	void SetCurrentAnimation(const std::string& animationName);
	const std::string& GetCurrentAnimation();
	std::vector<Matrix4D> GetCurrentFinalTransformation();
	std::vector<Matrix4D> GetCurrentOffsets();
	std::vector<Matrix4D> GetCurrentObjectBoneTransformation();
	const bool GetAllBones(std::map<std::string, unsigned int>& boneMap);
	const bool FindBoneByID(const unsigned int& id, std::string& boneName);
	const bool FindBoneByName(const std::string& boneName, unsigned int& id);
	const unsigned long long GetTypeID();
};