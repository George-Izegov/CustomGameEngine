
struct ParticleGPU
{
	float3 velocity;
	float lifetime;
	float mass;
	bool isAlive;
	float randFloat;
};

struct Emitter
{
	float3 EmitterPosition;
	float3 EffectedForce;
	int ParticlePerSec;
	float ParticleSize;
	float3 ParticleSpawnDeviation;
	float3 StartVelocity;
	float3 VelDeviation;
	float4 ParticleColor;
	float4 ColorDeviation;
	float ParticlesLifetime;
	float LifetimeDeviation;
	float ParticlesMass;
	float MassDeviation;

	int MaxParticles;

	// private members
	float WaitTimeForSpawnParticle;
};

struct VertexType
{
	float3 position;
	float4 color;
	float3 cornerPos;
	float2 sincos;
	float2 texCoord;
};