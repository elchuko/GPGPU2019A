#pragma once
#include "CDXShader.h"
#include <vector>
using namespace std;
class CParticleSystem :
	public CDXShader
{
public:
	struct PARTICLE
	{
		VECTOR4D Position;
		VECTOR4D Velocity;
		VECTOR4D Acceleration;
		VECTOR4D Force;
		VECTOR4D Color; //agregado
		float InvMass;
	};
	vector<PARTICLE> m_Particles;
	vector<PARTICLE> m_Results;
	struct PARAMS
	{
		VECTOR4D Gravity;
		float dt;
	}m_Params;
	CParticleSystem();
	~CParticleSystem();

	// Inherited via CDXShader
	virtual bool OnBegin(void * pParams, size_t size) override;
	virtual void OnCompute(int nPass) override;
	virtual long GetNumberOfPasses() override;
	virtual void OnEnd() override;
	virtual void InitParticles();
};

