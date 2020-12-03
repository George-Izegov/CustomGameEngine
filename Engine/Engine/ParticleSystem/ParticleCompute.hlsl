#include "ParticleGlobals.h"

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float DeltaSeconds;
};

// A texture filled with random values for generating some variance in our particles when we spawn them
Texture2D								g_RandomBuffer			: register(t0);
SamplerState g_samWrapLinear		: register(s0);

RWStructuredBuffer<VertexType>	ParticleBufferVertex		: register(u0);
RWStructuredBuffer<ParticleGPU> ParticleParams		: register(u1);
RWStructuredBuffer<Emitter> EmitterParams		: register(u2);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float3 gravity = { 0.0, -9.8, 0.0 };

	// simulate particles
	for (uint i = 0; i < EmitterParams[0].MaxParticles; i++)
	{
		if (ParticleParams[i].isAlive)
		{
			uint ind = i * 4;

			ParticleParams[i].lifetime -= DeltaSeconds;
			if (ParticleParams[i].lifetime < 0.0f)
			{
				ParticleParams[i].isAlive = false;
				for (uint j = 0; j < 4; j++)
				{
					ParticleBufferVertex[ind + j] = (VertexType)0;
				}
				continue;
			}

			if (ParticleParams[i].lifetime < 1.0f)
			{
				for (uint j = 0; j < 4; j++)
				{
					ParticleBufferVertex[ind + j].color.w -= DeltaSeconds;
				}
			}
			float3 velocity = ParticleParams[i].velocity;
			float3 acceleration = EmitterParams[0].EffectedForce / ParticleParams[i].mass + gravity;

			float3 shift = velocity * DeltaSeconds + acceleration * DeltaSeconds * DeltaSeconds / 2.0f;

			// calculate rotation matrix to bilboard quads
			float dx = ParticleBufferVertex[ind + 0].position.x + EmitterParams[0].EmitterPosition.x - cameraPosition.x;
			float dz = ParticleBufferVertex[ind + 0].position.z + EmitterParams[0].EmitterPosition.z - cameraPosition.z;
			float angle = atan2(dx, dz);
			// calculate rotation matrix to bilboard quads
			float s, c;
			sincos(angle, s, c);

			// shift each vertex of a quad
			for (uint j = 0; j < 4; j++)
			{
				ParticleBufferVertex[ind + j].position += shift;
				ParticleBufferVertex[ind + j].sincos.x = s;
				ParticleBufferVertex[ind + j].sincos.y = c;
			}

			ParticleParams[i].velocity += acceleration * DeltaSeconds;

		}
	}


	EmitterParams[0].WaitTimeForSpawnParticle -= DeltaSeconds;

	if (EmitterParams[0].WaitTimeForSpawnParticle <= 0.0f)
	{
		int NumToSpawn = (int)(EmitterParams[0].ParticlePerSec * DeltaSeconds);

		if (NumToSpawn == 0)
		{
			NumToSpawn = 1;
			EmitterParams[0].WaitTimeForSpawnParticle = 1.0f / EmitterParams[0].ParticlePerSec;
		}
		else
		{
			EmitterParams[0].WaitTimeForSpawnParticle = 0.0f;
		}

		// spawn particles
		for (uint i = 0; i < EmitterParams[0].MaxParticles; i++)
		{
			if (!ParticleParams[i].isAlive)
			{
				int iPart = i % 1024;
				// Generate some random numbers from reading the random texture
				float2 uv = float2(iPart / 1024.0, ParticleParams[i].randFloat);
				float3 randomValues0 = g_RandomBuffer.SampleLevel(g_samWrapLinear, uv, 0).xyz;

				uv = float2(ParticleParams[i].randFloat, iPart / 1024.0);
				float3 randomValues1 = g_RandomBuffer.SampleLevel(g_samWrapLinear, uv, 0).xyz;

				uv = float2(ParticleParams[i].randFloat / 2 + iPart / 2048, ParticleParams[i].randFloat / 2 + iPart / 2048.0);
				float3 randomValues2 = g_RandomBuffer.SampleLevel(g_samWrapLinear, uv, 0).xyz;

				ParticleParams[i].lifetime = EmitterParams[0].ParticlesLifetime + randomValues2.z * EmitterParams[0].LifetimeDeviation;
				ParticleParams[i].isAlive = true;
				uint ind = i * 4;

				float3 pos1 = { -EmitterParams[0].ParticleSize, -EmitterParams[0].ParticleSize, 0.0f };
				ParticleBufferVertex[ind + 0].cornerPos = pos1;
				float3 pos2 = { -EmitterParams[0].ParticleSize, EmitterParams[0].ParticleSize, 0.0f };
				ParticleBufferVertex[ind + 1].cornerPos = pos2;
				float3 pos3 = { EmitterParams[0].ParticleSize, -EmitterParams[0].ParticleSize, 0.0f };
				ParticleBufferVertex[ind + 2].cornerPos = pos3;
				float3 pos4 = { EmitterParams[0].ParticleSize, EmitterParams[0].ParticleSize, 0.0f };
				ParticleBufferVertex[ind + 3].cornerPos = pos4;

				float2 tex1 = { 0.0f, 1.0f };
				ParticleBufferVertex[ind + 0].texCoord = tex1;
				float2 tex2 = { 0.0f, 0.0f };
				ParticleBufferVertex[ind + 1].texCoord = tex2;
				float2 tex3 = { 1.0f, 1.0f };
				ParticleBufferVertex[ind + 2].texCoord = tex3;
				float2 tex4 = { 1.0f, 0.0f };
				ParticleBufferVertex[ind + 3].texCoord = tex4;

				float3 pos = EmitterParams[0].ParticleSpawnDeviation * randomValues2;
				ParticleBufferVertex[ind + 0].position = pos;
				ParticleBufferVertex[ind + 1].position = pos;
				ParticleBufferVertex[ind + 2].position = pos;
				ParticleBufferVertex[ind + 3].position = pos;

				float4 col = EmitterParams[0].ParticleColor + EmitterParams[0].ColorDeviation * randomValues1.x;
				ParticleBufferVertex[ind + 0].color = col;
				ParticleBufferVertex[ind + 1].color = col;
				ParticleBufferVertex[ind + 2].color = col;
				ParticleBufferVertex[ind + 3].color = col;

				float3 vel = EmitterParams[0].StartVelocity + EmitterParams[0].VelDeviation * randomValues0;
				ParticleParams[i].velocity = vel;

				ParticleParams[i].mass = EmitterParams[0].ParticlesMass + EmitterParams[0].MassDeviation * randomValues0.x;

				NumToSpawn--;

				if (NumToSpawn <= 0)
				{
					break;
				}
			}
		
		}
	}

	
	


}