#pragma once

namespace VVVV { namespace Assimp { namespace Lib {

	public ref class AnimVectorKey
	{
	public:
		AnimVectorKey(void) {}
		AnimVectorKey(double time, Vector3 val) { this->Time = time; this->Value = val; }
		double Time;
		Vector3 Value;
	};

	public ref class AnimQuatKey
	{
	public:
		AnimQuatKey(void) {}
		AnimQuatKey(double time, Quaternion quat) { this->Time = time; this->Value = quat; }
		double Time;
		Quaternion Value;
	};

	public ref class AssimpAnimationChannel
	{
	public:
		AssimpAnimationChannel(void);
		property String^ Name { String^ get(); }
		property List<AnimVectorKey^>^ PositionKeys { List<AnimVectorKey^>^ get(); }
		property List<AnimVectorKey^>^ ScalingKeys { List<AnimVectorKey^>^ get(); }
		property List<AnimQuatKey^>^ RotationKeys { List<AnimQuatKey^>^ get(); }
	internal:
		AssimpAnimationChannel(aiNodeAnim* nodeanim);
	private:
		aiNodeAnim* m_pNodeAnim;
		List<AnimVectorKey^>^ m_poskeys;
		List<AnimVectorKey^>^ m_scalekeys;
		List<AnimQuatKey^>^ m_rotkeys;


	};

}}}