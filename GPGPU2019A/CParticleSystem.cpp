#include "stdafx.h"
#include "CParticleSystem.h"
#include <iostream>

//vector<vector<CParticleSystem::PARTICLE>> AllParticlesVector;
vector<CParticleSystem::PARTICLE> AllParticlesVector;
bool flag = false;

CParticleSystem::CParticleSystem()
{
}


CParticleSystem::~CParticleSystem()
{
}

void CParticleSystem::InitParticles()
{
	//AllParticlesVector = entry;
	//Inicio de crear particulas para cada uno de los vectores de cohetes
		//Creamos particular
		/*
		vector<CParticleSystem::PARTICLE> Test;
		Test.resize(500);
		*/
		AllParticlesVector.clear();
		AllParticlesVector.resize(500);
		VECTOR4D randPosition = { rand() % 1024, rand() % 1024, rand() % 1024, rand() % 1024 };
		VECTOR4D randColor = { float((rand() % 255 + 150)) / 255, float((rand() % 255 + 150)) / 255, float((rand() % 255 + 150)) / 255, 1 };
		for (int i = 0; i < AllParticlesVector.size(); i++)
		{
			float theta = 2.0f*3.141592f*rand() / RAND_MAX;
			float v = 10.0f + 100.0f*rand() / RAND_MAX;
			//Test[i].Position = { 512,512,0,1 };
			/*
			Test[i].Position = randPosition;
			Test[i].Velocity = { v*cos(theta),v*sin(theta),0,0 };
			Test[i].Force = { 0,0,0,0 };
			Test[i].InvMass = 1;
			Test[i].Color = randColor;
			*/
			AllParticlesVector[i].Position = randPosition;
			AllParticlesVector[i].Velocity = { v*cos(theta),v*sin(theta),0,0 };
			AllParticlesVector[i].Force = { 0,0,0,0 };
			AllParticlesVector[i].InvMass = 1;
			AllParticlesVector[i].Color = randColor;

		}
		m_Params.Gravity = { 0,10,0,0 };
		m_Params.dt = 0.01;
		//g_PS.OnBegin(&Test[0], Test.size());
		OnBegin(&AllParticlesVector[0], AllParticlesVector.size());

}

bool CParticleSystem::OnBegin(void * pParams, size_t size)
{
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));

	// Constant buffer
		dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		dbd.Usage = D3D11_USAGE_DYNAMIC;
		dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		dbd.ByteWidth = 16 * ((sizeof(PARAMS) + 15) / 16);
		ID3D11Buffer* pCB = nullptr;
		m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pCB);
		/*
		if(flag)
			m_vConstantBuffers.pop_back(); //***********************************
		*/
		m_vConstantBuffers.push_back(pCB);

		// Structured buffer GPU
		dbd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		dbd.CPUAccessFlags = 0;
		dbd.StructureByteStride = sizeof(PARTICLE);
		dbd.Usage = D3D11_USAGE_DEFAULT;
		dbd.ByteWidth = size * sizeof(PARTICLE);
		dbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pCB);
		m_vConstantBuffers.push_back(pCB);
		ID3D11UnorderedAccessView* pUAV = nullptr;
		m_pOwner->GetDevice()->CreateUnorderedAccessView(pCB, NULL, &pUAV); //Esta creando UAVs de Mas, casi una por cada cohete.
		/*
		if(flag)
			m_vUAVs.pop_back();
		*/
		m_vUAVs.push_back(pUAV);
		flag = true;
		

	// Subir la información al GPU.
	memset(&m_Particles, 0, sizeof(m_Particles));
	m_Particles.resize(size);
	memcpy(&m_Particles[0], pParams, size*sizeof(PARTICLE));
	dbd.BindFlags = 0;
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_STAGING;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pCB);
	D3D11_MAPPED_SUBRESOURCE dms;
	m_pOwner->GetContext()->Map(pCB, 0, D3D11_MAP_WRITE, 0, &dms);
	memcpy(dms.pData, &m_Particles[0], sizeof(PARTICLE)*size);
	m_pOwner->GetContext()->Unmap(pCB, 0);
	m_pOwner->GetContext()->CopyResource(m_vConstantBuffers[1], pCB);
	pCB->Release();
	return true;
}

void CParticleSystem::OnCompute(int nPass)
{
	switch (nPass)
	{
	case 0: //Simulate
	{
		PARAMS Temp = m_Params;
		D3D11_MAPPED_SUBRESOURCE dms;
		m_pOwner->GetContext()->Map(m_vConstantBuffers[0], 0,
			D3D11_MAP_WRITE_DISCARD, 0, &dms);
		memcpy(dms.pData, &Temp, sizeof(PARAMS));
		m_pOwner->GetContext()->Unmap(m_vConstantBuffers[0], 0);
		m_pOwner->GetContext()->CSSetConstantBuffers(0,
			(UINT)m_vConstantBuffers.size(), &m_vConstantBuffers[0]);
		//Salida grafica
		m_pOwner->GetContext()->CSSetUnorderedAccessViews(1, 1, &m_vUAVs[0],0);
		//Entrada/Salida PARTICULAS
		m_pOwner->GetContext()->CSSetUnorderedAccessViews(0, 1, &m_vUAVs[1], 0);
		//Instalar el shader
		m_pOwner->GetContext()->CSSetShader(m_vShaders[0], 0, 0);
		//Dispatch
		m_pOwner->GetContext()->Dispatch((m_Particles.size() + 255) / 256, 1, 1);
	}
	break;

	case 1: //Plot
		//Salida grafica
		m_pOwner->GetContext()->CSSetUnorderedAccessViews(1, 1, &m_vUAVs[0], 0);
		//Entrada/Salida PARTICULAS
		m_pOwner->GetContext()->CSSetUnorderedAccessViews(0, 1, &m_vUAVs[1], 0);
		//Instalar el shader
		m_pOwner->GetContext()->CSSetShader(m_vShaders[1], 0, 0);
		//Dispatch
		m_pOwner->GetContext()->Dispatch((m_Particles.size() + 255) / 256, 1, 1);

		//m_pOwner->GetContext()->Dispatch((m_Particles.size() + 255) / 256, 1, 1);
	}
}

long CParticleSystem::GetNumberOfPasses()
{
	return 2;
}

void CParticleSystem::OnEnd()
{

	//Descargar el buffer estructurado del GPU -> m_Particles
	D3D11_BUFFER_DESC dbd;
	m_vConstantBuffers[1]->GetDesc(&dbd);
	//m_Results.resize(dbd.ByteWidth / sizeof(VECTOR4D));
	m_Results.resize(500);
	dbd.BindFlags = 0;
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	dbd.Usage = D3D11_USAGE_STAGING;
	ID3D11Buffer* pStaging = nullptr;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &pStaging);
	m_pOwner->GetContext()->CopyResource(pStaging, m_vConstantBuffers[1]);
	D3D11_MAPPED_SUBRESOURCE dms;
	m_pOwner->GetContext()->Map(pStaging, 0, D3D11_MAP_READ, 0, &dms); //Aqui esta tronando por algo
	memcpy(&m_Results[0], dms.pData, dbd.ByteWidth);
	//if (m_Results[0].Color.x <= 0 && m_Results[0].Color.y <= 0 && m_Results[0].Color.z <= 0)
	if (m_Results[0].Color.x <= 0)
	{
		InitParticles();
	}
	m_pOwner->GetContext()->Unmap(pStaging, 0);
	pStaging->Release();

}
