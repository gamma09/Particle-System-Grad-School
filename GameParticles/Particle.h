#ifndef PARTICLE_H
#define PARTICLE_H

// include
#include "mem.h"
#include "Vect4D.h"
#include "Matrix.h"
#include <memory>

class __declspec(align(16)) Particle {
public:
	friend class ParticleEmitter;
	
	Particle();
	Particle(const Particle& p);
	~Particle();
	void Update(const float& time_elapsed, const float& life);
	void Randomize();

private:
	Vect4D position;
	Vect4D scale;
	Vect4D velocity;

	float rotation;
	float rotation_velocity;
};

#endif //PARTICLE_H
