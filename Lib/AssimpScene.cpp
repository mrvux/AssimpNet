#include "StdAfx.h"
#include "AssimpScene.h"

namespace AssimpNet {

AssimpScene::AssimpScene(void)
{
	m_meshes = gcnew List<AssimpMesh^>();
}

AssimpScene::~AssimpScene(void)
{
	if (this->m_pScene != NULL)
	{
		for (int i = 0; i < this->m_pScene->mNumMeshes;i++)
		{
			delete this->m_meshes[i];
		}

		aiReleaseImport(this->m_pScene);

		//delete this->m_pScene;
	}
	//this->m_pScene->mMeshes[0]->mBones[0]->mWeights[0].
}

AssimpScene::AssimpScene(String^ path,bool preloadmesh,bool threadedload)
{
	char* p = (char*)(void*)Marshal::StringToHGlobalAnsi(path);
	this->m_pScene = aiImportFile(p,aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded);

	bool threaded = preloadmesh && threadedload;

	
	//Load meshes
	m_meshes = gcnew List<AssimpMesh^>();
	for (int i = 0; i < this->m_pScene->mNumMeshes;i++)
	{
		m_meshes->Add(gcnew AssimpMesh(this->m_pScene, this->m_pScene->mMeshes[i]));
	}

	if (preloadmesh)
	{
		#pragma omp parallel if (threaded)
		{
			#pragma omp for
			for (int i = 0; i < this->m_pScene->mNumMeshes;i++)
			{
				m_meshes[i]->Initialize();
			}
		}
	}

	//Load cameras
	m_cameras = gcnew List<AssimpCamera^>();
	for (int i = 0; i < this->m_pScene->mNumCameras;i++)
	{
		m_cameras->Add(gcnew AssimpCamera(this->m_pScene->mCameras[i]));
	}

	//Load animations
	m_animations = gcnew List<AssimpAnimation^>();
	for (int i = 0; i < this->m_pScene->mNumAnimations;i++)
	{
		m_animations->Add(gcnew AssimpAnimation(this->m_pScene->mAnimations[i]));
	}

	//Load materials
	m_materials = gcnew List<AssimpMaterial^>();
	for (int i = 0; i < this->m_pScene->mNumMaterials;i++)
	{
		m_materials->Add(gcnew AssimpMaterial(this->m_pScene->mMaterials[i]));
	}

	

	this->m_rootnode = gcnew AssimpNode(nullptr,this->m_pScene->mRootNode);
}

AssimpScene::AssimpScene(aiScene* scene)
{
	this->m_pScene = scene;
}

int AssimpScene::MeshCount::get()
{
	return this->m_pScene->mNumMeshes;
}

List<AssimpMesh^>^ AssimpScene::Meshes::get()
{
	return this->m_meshes;
}

AssimpNode^ AssimpScene::RootNode::get()
{
	return this->m_rootnode;
}

List<AssimpCamera^>^ AssimpScene::Cameras::get()
{ 
	return this->m_cameras;
}

List<AssimpAnimation^>^ AssimpScene::Animations::get()
{ 
	return this->m_animations;
}

List<AssimpMaterial^>^ AssimpScene::Materials::get()
{ 
	return this->m_materials;
}

}