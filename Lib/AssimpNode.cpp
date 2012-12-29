#include "StdAfx.h"
#include "AssimpNode.h"

namespace AssimpNet {

AssimpNode::AssimpNode(AssimpNode^ parent,aiNode* node)
{
	this->m_pNode = node;
	this->m_parent = parent;
	this->localtr = this->GetTransform(node->mTransformation);
	
	aiMatrix4x4 mGlobal = node->mTransformation;
	aiNode* par = node->mParent;
	while(par)
	{
		mGlobal = par->mTransformation * mGlobal;
		par = par->mParent;
	}

	this->relativetr = this->GetTransform(mGlobal);
	
	this->m_children = gcnew List<AssimpNode^>();
	for (int i = 0; i < node->mNumChildren; i++)
	{
		this->m_children->Add(gcnew AssimpNode(this,node->mChildren[i]));
	}

}

int AssimpNode::MeshCount::get()
{
	return this->m_pNode->mNumMeshes;
}

String^ AssimpNode::Name::get()
{
	return gcnew String(this->m_pNode->mName.data);
}

List<int>^ AssimpNode::MeshIndices::get()
{
	List<int>^ res = gcnew List<int>();
	for (int i = 0; i < this->m_pNode->mNumMeshes;i++)
	{
		res->Add(this->m_pNode->mMeshes[i]);
	}
	return res;
}

List<AssimpNode^>^ AssimpNode::Children::get()
{
	return this->m_children;
}



Matrix AssimpNode::LocalTransform::get()
{
	return this->localtr;
}


Matrix AssimpNode::RelativeTransform::get()
{
	return this->relativetr;
}

Matrix AssimpNode::GetTransform(aiMatrix4x4 tr)
{
	SlimDX::Matrix roottr;
	roottr.M11 = tr.a1;
	roottr.M21 = tr.a2;
	roottr.M31 = tr.a3;
	roottr.M41 = tr.a4;

	roottr.M12 = tr.b1;
	roottr.M22 = tr.b2;
	roottr.M32 = tr.b3;
	roottr.M42 = tr.b4;

	roottr.M13 = tr.c1;
	roottr.M23 = tr.c2;
	roottr.M33 = tr.c3;
	roottr.M43 = tr.c4;

	roottr.M14 = tr.d1;
	roottr.M24 = tr.d2;
	roottr.M34 = tr.d3;
	roottr.M44 = tr.d4;

	return roottr;
}





}