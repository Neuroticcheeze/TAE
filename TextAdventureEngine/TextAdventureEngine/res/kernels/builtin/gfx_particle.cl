typedef struct
{
	float2 mPosition;
	float2 mImpulse; //  |xy| = dir, len(xy) = mag

} sWindSource;

typedef struct
{
	float2 mAcceleration;
	float mDamping;
	//sWindSource * mWindSources;
	//uint		  mWindSourcesNum;

} sParticleInfo;












// Particle-State Structs
typedef struct
{
	float2 mPosition;
	float2 mVelocity;
	float mLife;
	float mLifeStep;

} sParticle_Format0;



__kernel void Proc_POS2VEL2LIFE2( __global sParticle_Format0 * a_Buff, const sParticleInfo a_Info, float a_Delta )
{
	__global sParticle_Format0 * thisParticle = a_Buff + get_global_id( 0 );

	thisParticle->mVelocity += a_Info.mAcceleration * a_Delta;
	thisParticle->mVelocity *= pow( a_Info.mDamping, a_Delta );
	thisParticle->mPosition += thisParticle->mVelocity * a_Delta;
	thisParticle->mLife = min( thisParticle->mLife + a_Delta * thisParticle->mLifeStep, 1.0F );
}