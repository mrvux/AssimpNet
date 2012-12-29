#pragma once

#include "AssimpAnimationChannel.h"

namespace AssimpNet {

public ref class AssimpAnimation
{
public:
	AssimpAnimation(void);

	property String^ Name { String^ get(); }
	property double Duration { double get(); }
	property double TicksPerSecond { double get(); }
	property int NumChannels { int get(); }
	property List<AssimpAnimationChannel^>^ Channels { List<AssimpAnimationChannel^>^ get(); }

internal:
	AssimpAnimation(aiAnimation* anim);
private:
	aiAnimation* m_pAnim;
	List<AssimpAnimationChannel^>^ m_channels;
};

}


