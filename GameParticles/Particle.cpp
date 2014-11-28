#include "Particle.h"
#include "Matrix.h"
#include "Vect4D.h"

const static __m128 STARTING_VELOCITY = _mm_setr_ps(0.0f, 1.0f, 0.0f, 1.0f);
const static __m128 STARTING_SCALE = _mm_set_ps1(1.0f);
const static Vect4D Z_AXIS(0.0f, -0.25f, 1.0f, 1.0f);

ParticleColdData::ParticleColdData() :
	currMtx(),
	diffMtx(),
	prevMtx()
{
}

ParticleColdData::~ParticleColdData()
{
}

Particle::Particle() :
	life(0.0f),
	position(),
	scale(STARTING_SCALE),
	rotation(0.0f),
	velocity(STARTING_VELOCITY),
	rotation_velocity(0.5f),
	coldData(new ParticleColdData)
{
	Randomize();
}

Particle::Particle(const Particle& p) :
	next(p.next),
	prev(p.prev),
	position(p.position),
	scale(p.scale),
	life(p.life),
	rotation(p.rotation),
	velocity(p.velocity),
	rotation_velocity(p.rotation_velocity),
	coldData(p.coldData)
{
}

Particle::~Particle() {
	// nothing to do
}

void Particle::Update(const float& time_elapsed) {
	// Rotate the matrices
	coldData->prevMtx = coldData->currMtx;

	float scale = coldData->diffMtx.Determinant();

	if (scale > 1.0f) {
		scale = 1.0f / scale;
	}

	float rot_vel = rotation_velocity;
	rot_vel *= time_elapsed;
	rot_vel *= 2.01f;
	rotation += rot_vel;
	rotation += scale;

	// serious math below - magic secret sauce
	life += time_elapsed;
	Vect4D vel = velocity;
	vel *= time_elapsed;
	position += vel;

	Vect4D v = position.Cross(Z_AXIS).norm();
	v *= 0.05f * life;
	position += v;
}

static const __m128 VEL_VARIANCE = _mm_setr_ps(0.001f, 0.004f, 0.0004f, 1.0f);
static const __m128 POS_VARIANCE = _mm_setr_ps(0.001f, 0.001f, 0.001f, 1.0f);
static const __m128 SIGN_STARTER = _mm_setr_ps(-1.0f, -1.0f, -1.0f, 1.0f);
static const __m128 SIGN_MULTIPLIER = _mm_set_ps1(2.0f);

void Particle::Randomize()
{
	// Add some randomness...

	// Ses it's ugly - I didn't write this so don't bitch at me
	// Sometimes code like this is inside real commerical code ( so now you know how it feels )
	
	// x - variance
	__m128 var = _mm_mul_ps(POS_VARIANCE, _mm_setr_ps(static_cast<float>(rand() % 1000), static_cast<float>(rand() % 1000), static_cast<float>(rand() % 1000), 1.0f));
	__m128 sign = _mm_add_ps(SIGN_STARTER, _mm_mul_ps(SIGN_MULTIPLIER, _mm_setr_ps(static_cast<float>(rand() % 2), static_cast<float>(rand() % 2), static_cast<float>(rand() % 2), 0.0f)));
	var = _mm_mul_ps(var, sign);
	position *= var;
	
	var = _mm_mul_ps(VEL_VARIANCE, _mm_setr_ps(static_cast<float>(rand() % 1000), static_cast<float>(rand() % 1000), static_cast<float>(rand() % 1000), 0.0f));
	sign = _mm_add_ps(SIGN_STARTER, _mm_mul_ps(SIGN_MULTIPLIER, _mm_setr_ps(static_cast<float>(rand() % 2), static_cast<float>(rand() % 2), static_cast<float>(rand() % 2), 0.0f)));
	velocity += _mm_mul_ps(var, sign);
	
	// correct the sign
	float scaleVar = static_cast<float>(rand() % 1000) * 0.002f;
	float scaleSign = static_cast<float>(rand() % 2);
	
	if(scaleSign == 0)
	{
		scaleVar *= -1.0f;
	}
	scale *= scaleVar;
}

// End of file


