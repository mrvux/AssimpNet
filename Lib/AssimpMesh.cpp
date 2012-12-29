#include "StdAfx.h"
#include "AssimpMesh.h"

namespace AssimpNet {

AssimpMesh::AssimpMesh(void)
{
	this->m_maxBonePerVertex = 0;
}

AssimpMesh::~AssimpMesh(void)
{
	if (this->m_vertices != nullptr)
	{
		delete this->m_vertices;
	}
}

AssimpMesh::AssimpMesh(const aiScene* scene,aiMesh* mesh)
{
	this->m_pMesh = mesh;
	this->m_bonenames = gcnew List<String^>();
	this->m_bonemats = gcnew List<Matrix>();

	
	for( unsigned int a = 0; a < this->m_pMesh->mNumBones; a++)	
	{
		const aiBone* bone = this->m_pMesh->mBones[a];
		this->m_bonenames->Add(gcnew String(bone->mName.data));

		this->m_bonemats->Add(this->GetTransform(bone->mOffsetMatrix));
	}
}

void AssimpMesh::Initialize()
{
	this->LoadIndices();
	this->LoadVertices();
}
		
	

aiNode* AssimpMesh::FindNodeRecursivelyByName(aiNode* root, const aiString name)
{
	if (root->mName == name)
	{
		return root;
	}
	else
	{
		for (int i = 0; i < root->mNumChildren; i++)
		{
			aiNode* node = this->FindNodeRecursivelyByName(root->mChildren[i],name);
			if (node)
			{
				return node;
			}
		}
	}

	return NULL;
}


List<String^>^ AssimpMesh::BoneNames::get()
{
	return this->m_bonenames;
}

List<Matrix>^ AssimpMesh::BoneMatrices::get()
{
	return this->m_bonemats;
}


int AssimpMesh::MaxBonePerVertex::get()
{
	return this->m_maxBonePerVertex;
}

int AssimpMesh::MaterialIndex::get()
{
	return this->m_pMesh->mMaterialIndex;
}

int AssimpMesh::VerticesCount::get()
{
	return this->m_pMesh->mNumVertices;
}

bool AssimpMesh::HasNormals::get()
{
	return this->m_pMesh->HasNormals();
}

int AssimpMesh::UvChannelCount::get()
{
	return m_pMesh->GetNumUVChannels();
}

int AssimpMesh::ColorChannelCount::get()
{
	return this->m_pMesh->GetNumColorChannels();
}

System::IntPtr AssimpMesh::GetUvPointer(int idx)
{
	if (idx < this->m_pMesh->GetNumUVChannels())
	{
		return System::IntPtr(this->m_pMesh->mTextureCoords[idx]);
	}
	else
	{
		return System::IntPtr::Zero;
	}
}


SlimDX::BoundingBox AssimpMesh::BoundingBox::get()
{
	if (this->m_vertices == nullptr)
	{
		this->Initialize();
	}
	return this->m_bb;
}

Matrix AssimpMesh::GetTransform(aiMatrix4x4 tr)
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

void AssimpMesh::LoadIndices()
{
	if (this->m_indices == nullptr)
	{
		this->m_indices = gcnew List<int>();
		for (int i = 0; i < this->m_pMesh->mNumFaces ; i++)
		{
			aiFace f = this->m_pMesh->mFaces[i];
			
			if (f.mNumIndices == 3)
			{
				this->m_indices->Add(f.mIndices[0]);
				this->m_indices->Add(f.mIndices[1]);
				this->m_indices->Add(f.mIndices[2]);
			}
		}
	}
}

void AssimpMesh::LoadVertices()
{
	if (this->m_vertices == nullptr)
	{
		int vertexsize = this->CalculateVertexSize();
		int totalsize = vertexsize * this->m_pMesh->mNumVertices;
		this->m_vertices = gcnew DataStream(totalsize,true,true);
		this->Write(this->m_vertices);
	}
}



List<int>^ AssimpMesh::Indices::get()
{
	if (this->m_indices == nullptr)
	{
		this->LoadIndices();
	}
	return this->m_indices;
}

DataStream^ AssimpMesh::Vertices::get()
{
	if (this->m_vertices == nullptr)
	{
		this->LoadVertices();
	}
	return this->m_vertices;
}

System::IntPtr AssimpMesh::PositionPointer::get()
{ 
	return System::IntPtr(this->m_pMesh->mVertices);
}

System::IntPtr AssimpMesh::NormalsPointer::get()
{ 
	if (this->m_pMesh->HasNormals())
	{
		return System::IntPtr(this->m_pMesh->mNormals);
	}
	else
	{
		return System::IntPtr::Zero;
	}
}

void AssimpMesh::Write(DataStream^ vertices)
{
	aiColor4D col;
	aiVector3D v3;

	Vector3 min = Vector3(float::MaxValue,float::MaxValue,float::MaxValue);
	Vector3 max = Vector3(float::MinValue,float::MinValue,float::MinValue);


	// collect weights on all vertices. Quick and careless
	std::vector<std::vector<aiVertexWeight> > weightsPerVertex( this->m_pMesh->mNumVertices);
	for( unsigned int a = 0; a < this->m_pMesh->mNumBones; a++)	
	{
		const aiBone* bone = this->m_pMesh->mBones[a];
		for( unsigned int b = 0; b < bone->mNumWeights; b++)
		{
			weightsPerVertex[bone->mWeights[b].mVertexId].push_back( aiVertexWeight( a, bone->mWeights[b].mWeight));
		}
	}
	
	for (int i = 0; i < this->m_pMesh->mNumVertices; i++)
	{
		v3 = this->m_pMesh->mVertices[i];
		vertices->Write(v3.x);
		vertices->Write(v3.y);
		vertices->Write(v3.z);

		min.X = v3.x < min.X ? v3.x :min.X;
		min.Y = v3.y < min.Y ? v3.y :min.Y;
		min.Z = v3.z < min.Z ? v3.z :min.Z;

		max.X = v3.x > max.X ? v3.x :max.X;
		max.Y = v3.y > max.Y ? v3.y :max.Y;
		max.Z = v3.z > max.Z ? v3.z :max.Z;

		//Normal
		if (this->m_pMesh->HasNormals())
		{
			v3 = this->m_pMesh->mNormals[i];
			vertices->Write(v3.x);
			vertices->Write(v3.y);
			vertices->Write(v3.z);
		}

		//Tangents / Bitan
		if (this->m_pMesh->HasTangentsAndBitangents())
		{
			v3 = this->m_pMesh->mTangents[i];
			vertices->Write(v3.x);
			vertices->Write(v3.y);
			vertices->Write(v3.z);

			v3 = this->m_pMesh->mBitangents[i];
			vertices->Write(v3.x);
			vertices->Write(v3.y);
			vertices->Write(v3.z);
		}

		//Color
		for (int j = 0; j < m_pMesh->GetNumColorChannels(); j++)
		{
			col = this->m_pMesh->mColors[j][i];
			vertices->Write(col.r);
			vertices->Write(col.g);
			vertices->Write(col.b);
			vertices->Write(col.a);
		}

		//UV
		for (int j = 0; j < m_pMesh->GetNumUVChannels(); j++)
		{
			int numcomp = this->m_pMesh->mNumUVComponents[j];
			if (numcomp == 1)
			{
				v3 = this->m_pMesh->mTextureCoords[j][i];
				vertices->Write(v3.x);
			}
			if (numcomp == 2)
			{
				v3 = this->m_pMesh->mTextureCoords[j][i];
				vertices->Write(v3.x);
				vertices->Write(v3.y);
			}
			if (numcomp == 3)
			{
				v3 = this->m_pMesh->mTextureCoords[j][i];
				vertices->Write(v3.x);
				vertices->Write(v3.y);
				vertices->Write(v3.z);
			}
		}

		if( this->m_pMesh->HasBones())	
		{
			float indices[4] = { 0,0,0,0 };
			float weight[4] = { 0.0f,0.0f,0.0f,0.0f };

			if (weightsPerVertex[i].size() == 0)
			{
				float d = 0;
			}

			this->m_maxBonePerVertex = weightsPerVertex[i].size() > this->m_maxBonePerVertex ? weightsPerVertex[i].size() : this->m_maxBonePerVertex;

			float wsum = 0;
			for( unsigned int a = 0; a < weightsPerVertex[i].size(); a++)
			{
				indices[a] = weightsPerVertex[i][a].mVertexId;
				weight[a] = weightsPerVertex[i][a].mWeight;
				wsum += weight[a];
			}

			if (wsum > 1)
			{
				float d = 0;
			}

			vertices->Write(indices[0]);
			vertices->Write(indices[1]);
			vertices->Write(indices[2]);
			vertices->Write(indices[3]);
			vertices->Write(weight[0]);
			vertices->Write(weight[1]);
			vertices->Write(weight[2]);
			vertices->Write(weight[3]);
		}
	}

	this->m_bb = SlimDX::BoundingBox(min,max);
}

List<InputElement>^ AssimpMesh::GetInputElements()
{
	List<SlimDX::Direct3D11::InputElement>^ result = gcnew List<InputElement>();

	int offset = 0;

	//Position
	InputElement pos("POSITION",0,SlimDX::DXGI::Format::R32G32B32_Float,0,0);
	offset += 12;
	result->Add(pos);

	//Normal
	if (this->m_pMesh->HasNormals())
	{
		InputElement norm("NORMAL",0,SlimDX::DXGI::Format::R32G32B32_Float,offset,0);
		offset += 12;
		result->Add(norm);
	}

	//Tangents / Bitan
	if (this->m_pMesh->HasTangentsAndBitangents())
	{
		InputElement tang("TANGENT",0,SlimDX::DXGI::Format::R32G32B32_Float,offset,0);
		offset += 12;
		result->Add(tang);
		InputElement bitan("BINORMAL",0,SlimDX::DXGI::Format::R32G32B32_Float,offset,0);
		offset += 12;
		result->Add(bitan);
	}

	for (int i = 0; i < m_pMesh->GetNumColorChannels(); i++)
	{
		InputElement col("COLOR",i,SlimDX::DXGI::Format::R32G32B32A32_Float,offset,0);
		offset += 16;
		result->Add(col);
	}

	for (int i = 0; i < m_pMesh->GetNumUVChannels(); i++)
	{
		int numcomp = this->m_pMesh->mNumUVComponents[i];
		SlimDX::DXGI::Format fmt = SlimDX::DXGI::Format::R32_Float;
		int stride = 4;
		if (numcomp == 2)
		{
			fmt = SlimDX::DXGI::Format::R32G32_Float;
			stride = 8;
		}
		if (numcomp == 3)
		{
			fmt = SlimDX::DXGI::Format::R32G32B32_Float;
			stride = 12;
		}
		InputElement tc("TEXCOORD",i,fmt,offset,0);
		offset += stride;
		result->Add(tc);
	}

	if (m_pMesh->HasBones())
	{
		InputElement inds("BLENDINDICES",0,SlimDX::DXGI::Format::R32G32B32A32_Float,offset,0);
		offset += 16;
		result->Add(inds);
		InputElement weight("BLENDWEIGHT",0,SlimDX::DXGI::Format::R32G32B32A32_Float,offset,0);
		offset += 16;
		result->Add(weight);
	}

	return result;
}

int AssimpMesh::CalculateVertexSize()
{
	//Position
	int result = 3 * sizeof(float);

	//Add normals
	if (this->m_pMesh->HasNormals()) { result += 3 * sizeof(float); }

	//Tangent and bitangents (goes together)
	if (this->m_pMesh->HasTangentsAndBitangents()) { result += 6 * sizeof(float); }

	//Color
	result += this->m_pMesh->GetNumColorChannels() * 4 * sizeof(float);

	//Texture coordinates, need to iterate since can be 1/2/3d
	for (int i = 0; i < this->m_pMesh->GetNumUVChannels() ; i++)
	{
		result += this->m_pMesh->mNumUVComponents[i] * sizeof(float);
	}

	if (m_pMesh->HasBones())
	{
		result += 4 * sizeof(float); //blend weights
		result += 4 * sizeof(float); //blend indices;
	}

	return result;
}


}