#ifndef PARTICLE_H
#define PARTICLE_H

// include
#include "mem.h"
#include "Vect4D.h"
#include "Matrix.h"
#include <memory>

extern memSystem mem;

struct __declspec(align(16)) ParticleColdData {
public:
	ParticleColdData();
	~ParticleColdData();
	
	void* operator new(size_t size) {
		return mem.Malloc(size);
	}

	void operator delete(void* memory) {
		mem.Free(memory);
	}

public:
	Matrix currMtx;
	Matrix diffMtx;
	Matrix prevMtx;
};

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
	Particle *next;
	Particle *prev;
	float rotation;
	float rotation_velocity;

	Vect4D position;
	Vect4D scale;
	Vect4D velocity;

	std::shared_ptr<ParticleColdData> coldData;
};

#endif //PARTICLE_H
