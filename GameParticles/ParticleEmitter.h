#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include "Matrix.h"
#include "Vect4D.h"
#include "Particle.h"
#include "mem.h"
#include "Settings.h"

class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();
	
	void SpawnParticles();
	void update();
	void draw();
	void reset();


private:

	float last_loop;
	float life;

	Particle* particles[NUM_PARTICLES];
};

#endif // PARTICLEEMITTER_H
