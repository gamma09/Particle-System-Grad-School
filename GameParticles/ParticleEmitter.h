#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include "Matrix.h"
#include "Vect4D.h"
#include "Particle.h"
#include "mem.h"
#include "Settings.h"

extern memSystem mem;

class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();

	void* operator new(size_t size) {
		return mem.Malloc(size);
	}

	void operator delete(void* memory) {
		mem.Free(memory);
	}
	
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
