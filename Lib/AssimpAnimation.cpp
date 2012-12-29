#include "StdAfx.h"
#include "AssimpAnimation.h"

namespace AssimpNet {

AssimpAnimation::AssimpAnimation(void)
{
	
}

AssimpAnimation::AssimpAnimation(aiAnimation* anim)
{
	this->m_pAnim = anim;
}

String^ AssimpAnimation::Name::get()
{
	return gcnew String(this->m_pAnim->mName.data);
}

double AssimpAnimation::Duration::get()
{
	return this->m_pAnim->mDuration;
}

double AssimpAnimation::TicksPerSecond::get()
{
	return this->m_pAnim->mTicksPerSecond;
}

int AssimpAnimation::NumChannels::get()
{
	return this->m_pAnim->mNumChannels;
}

List<AssimpAnimationChannel^>^ AssimpAnimation::Channels::get()
{
	if (this->m_channels == nullptr)
	{
		this->m_channels = gcnew List<AssimpAnimationChannel^>();
		for (int i = 0; i < this->m_pAnim->mNumChannels; i++)
		{
			AssimpAnimationChannel^ chan = gcnew AssimpAnimationChannel(this->m_pAnim->mChannels[i]);
			this->m_channels->Add(chan);
		}
	}
	return this->m_channels;
}

}
