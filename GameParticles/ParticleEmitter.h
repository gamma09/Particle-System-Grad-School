#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include "Matrix.h"
#include "Vect4D.h"
#include "Particle.h"
#include "mem.h"

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
	
	void SpawnParticle();
	void update();
	void draw();

	void addParticleToList(Particle *p );
	void removeParticleFromList( Particle *p );


private:

	float	last_spawn;
	float	last_loop;
	int		last_active_particle;
	
	Particle *headParticle;
};

#endif // PARTICLEEMITTER_H
