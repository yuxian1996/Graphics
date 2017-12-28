#include "Model.h"

#include <vector>
//#include <float.h>

#define MAXSIZE 1E5
#define MINSIZE 0.01

#pragma once

struct OctreeNode
{
	int faceIndex;
	int meshIndex;
	float cubeSize;
	glm::vec3 cubeCenter;
	glm::vec3 faceCenter;
	struct OctreeNode **childern;
	Model* model;
};

struct HitInfo
{
	Model* hitObject;
	glm::vec3 hitPosition;
};


class SceneManager
{
public:
	SceneManager() { mRoot = { -1, -1, MAXSIZE, glm::vec3(0,0,0), glm::vec3(0,0,0), nullptr, nullptr }; };
	~SceneManager() {};

	static SceneManager* Instance();

	void Destroy();

	void AddModel (Model* iModel);

	bool Raycast(glm::vec3 iOriginal, glm::vec3 iDirection, float iLength, HitInfo& oHitInfo);

	/*Draw the whole scene */
	void Draw();

	const std::vector<Model*>& GetAllModels() const { return mAllModels; };
	const OctreeNode& GetSceneRoot() const { return mRoot; };
private: 
	
	float RaycastNode(const glm::vec3 & iOriginal, const glm::vec3 & iDirection, float iLength, OctreeNode* iNode, HitInfo& oHitInfo);
	bool IntersectCube(const glm::vec3& iOriginal, const glm::vec3& iDirection, const glm::vec3& cubeCenter, float cubeSize);
	float IntersectFace(const glm::vec3& iOriginal, const glm::vec3& iDirection, float iLength, const glm::vec3& iPoint1, const glm::vec3& iPoint2, const glm::vec3& iPoint3, const glm::vec3& iNormal, glm::vec3& oHitPosition, bool isTriangle = true);

	/*Octree functions*/
	/*Insert an octree node*/
	void InsertNode(OctreeNode* iParent, OctreeNode* iNode);
	void DeleteNode();
	void FindParent(OctreeNode* iNode); 
	void DestroyTree(OctreeNode* iNode);
	bool IsEmptyNode(const OctreeNode& iNode) const;
	void CreateChildNodes(OctreeNode* iParent);

	/*Calculate the index and center of child*/
	/*     	top 
           | 0  1 |         
		   | 2  3 |
		   bottom
           | 4  5 |
		   | 6  7 |  */
	int CalculateChildIndex(OctreeNode const* iParent, OctreeNode* iChild);


	static SceneManager* gpSceneManager;

	OctreeNode mRoot;
	std::vector<Model*> mAllModels;
};