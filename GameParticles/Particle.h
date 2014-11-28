#ifndef PARTICLE_H
#define PARTICLE_H

// include
#include "mem.h"
#include "Vect4D.h"
#include "Matrix.h"
#include <memory>

extern memSystem mem;

class __declspec(align(16)) Particle {
public:
	friend class ParticleEmitter;
	
	Particle();
	Particle(const Particle& p);
	~Particle();
	void Update(const float& time_elapsed, const float& life);
	void Randomize();

	void* operator new (size_t size) {
		return mem.Malloc(size);
	}

	void operator delete(void* memory) {
		mem.Free(memory);
	}

private:
	Vect4D position;
	Vect4D scale;
	Vect4D velocity;

	float rotation;
	float rotation_velocity;
};

#endif //PARTICLE_H
