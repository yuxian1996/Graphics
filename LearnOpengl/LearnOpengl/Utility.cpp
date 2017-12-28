#include "Utility.h"

//bool Utility::Raycast(glm::vec3 iOriginal, glm::vec3 iDirection, float iLength, HitInfo& oHitInfo)
//{
//	std::vector<Model*> allModels = SceneManager::Instance()->GetAllModels();
//
//	//raycast with each model
//	for (int i = 0; i < allModels.size(); i++)
//	{
//		std::vector<Mesh> meshes = allModels[i]->GetAllMeshes();
//		//raycast with each mesh
//		for (int j = 0; j < meshes.size(); j++)
//		{
//			std::vector<Vertex> vertices = meshes[j].GetVertices();
//			std::vector<unsigned int> indices = meshes[j].GetIndices();
//
//			//raycast with each face
//			for (int k = 0; k < indices.size(); k+=3)
//			{
//				glm::vec3 point1, point2, point3, normal;
//				point1 = vertices[indices[k]].Position;
//				point2 = vertices[indices[k + 1]].Position;
//				point3 = vertices[indices[k + 2]].Position;
//				normal = vertices[indices[k]].Normal;
//
//				glm::vec3 faceNormal = glm::normalize(glm::cross(point2 - point1, point3 - point1));
//				if(glm::dot(faceNormal, normal) < 0)
//					faceNormal = -faceNormal;
//				float d = -glm::dot(point1, faceNormal);
//
//				float t = -(glm::dot(iOriginal, faceNormal) + d) / (glm::dot(iDirection, faceNormal));
//				glm::vec3 intersectionPoint = iOriginal + t * iDirection;
//
//				//check if intersection point is inside triangle
//				glm::vec3 vector1 = point3 - point1, vector2 = point2 - point1, vector = intersectionPoint - point1;
//				float t1, t2;
//				
//				//rarely happen?
//				if (abs(vector2.y *vector1.x - vector2.x * vector1.y) <= 0.00001)
//					continue;
//
//				t1 = (vector1.x * vector.y - vector.x * vector1.y) / (vector2.y * vector1.x - vector2.x * vector1.y);
//				if (vector1.x == 0)
//					t2 = (vector.y - t1* vector2.y) / vector1.y;
//				else
//				    t2 = (vector.x - t1 * vector2.x) / vector1.x;
//				if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1 && t1 + t2 <= 1 && t<iLength)
//				{
//					oHitInfo.hitObject = allModels[i];
//					oHitInfo.hitPosition = intersectionPoint;
//					return true;
//				}
//			}
//		}
//	}
//
//	oHitInfo = { nullptr, glm::vec3(0,0,0) };
//	return false;
//}
