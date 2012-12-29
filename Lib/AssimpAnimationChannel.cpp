#include "StdAfx.h"
#include "AssimpAnimationChannel.h"

namespace AssimpNet {

AssimpAnimationChannel::AssimpAnimationChannel(void)
{
}

AssimpAnimationChannel::AssimpAnimationChannel(aiNodeAnim* nodeanim)
{
	this->m_pNodeAnim = nodeanim;
}

String^ AssimpAnimationChannel::Name::get()
{
	return gcnew String(this->m_pNodeAnim->mNodeName.data);
}

List<AnimVectorKey^>^ AssimpAnimationChannel::PositionKeys::get()
{
	if (this->m_poskeys == nullptr)
	{
		this->m_poskeys = gcnew List<AnimVectorKey^>();
		for (int i = 0; i < this->m_pNodeAnim->mNumPositionKeys; i++)
		{
			aiVectorKey vk = this->m_pNodeAnim->mPositionKeys[i];
			AnimVectorKey^ vkm = gcnew AnimVectorKey(vk.mTime,Vector3(vk.mValue.x,vk.mValue.y,vk.mValue.z));
			this->m_poskeys->Add(vkm);		
		}
	}
	return this->m_poskeys;
}

List<AnimVectorKey^>^ AssimpAnimationChannel::ScalingKeys::get()
{
	if (this->m_scalekeys == nullptr)
	{
		this->m_scalekeys = gcnew List<AnimVectorKey^>();
		for (int i = 0; i < this->m_pNodeAnim->mNumScalingKeys; i++)
		{
			aiVectorKey vk = this->m_pNodeAnim->mScalingKeys[i];
			AnimVectorKey^ vkm = gcnew AnimVectorKey(vk.mTime,Vector3(vk.mValue.x,vk.mValue.y,vk.mValue.z));
			this->m_scalekeys->Add(vkm);		
		}
	}

	return this->m_scalekeys;
}

List<AnimQuatKey^>^ AssimpAnimationChannel::RotationKeys::get()
{
	if (this->m_rotkeys == nullptr)
	{
		this->m_rotkeys = gcnew List<AnimQuatKey^>();
		for (int i = 0; i < this->m_pNodeAnim->mNumRotationKeys; i++)
		{
			aiQuatKey vk = this->m_pNodeAnim->mRotationKeys[i];
			Quaternion quat(vk.mValue.x,vk.mValue.y,vk.mValue.z,vk.mValue.w);
			quat.Normalize();
			AnimQuatKey^ vkm = gcnew AnimQuatKey(vk.mTime,quat);
			this->m_rotkeys->Add(vkm);		
		}
	}

	return this->m_rotkeys;
}


}