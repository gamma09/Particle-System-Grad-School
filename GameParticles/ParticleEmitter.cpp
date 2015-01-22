#include "DO_NOT_MODIFY\Timer\Timer.h"
#include "DO_NOT_MODIFY\Timer\GlobalTimer.h"
#include "DO_NOT_MODIFY\OpenGL\OpenGLInterface.h"

#include <assert.h>

#include "ParticleEmitter.h"
#include "mem.h"

// We know beforehand that the camera is not moving, so we can do this to save us time
static const Vect4D CAMERA_MATRIX_ROW_3(0.0f, -3.0f, -10.0f, 1.0f);

static const unsigned char squareColors[] = 
{
	255,  255,  000,  255,
	000,  255,  255,  255,
	000,  000,  000,  000,
	255,  000,  255,  255,
}; 

static const float squareVertices[] = 
{
	-0.015f,  -0.015f, 0.0f,
	 0.015f,  -0.015f, 0.0f,
	-0.015f,   0.015f, 0.0f,
	 0.015f,   0.015f, 0.0f,
};

ParticleEmitter::ParticleEmitter()
	: last_loop(  globalTimer::getTimerInSec() )
{
	SpawnParticles();
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::SpawnParticles() {
	life = 0.0f;

	for (int i = 0; i < NUM_PARTICLES; i++) {
		particles[i] = new Particle();
	}
}

void ParticleEmitter::update()
{
	// total elapsed
	float current_time = globalTimer::getTimerInSec();
	float time_elapsed = current_time - last_loop;
	last_loop = current_time;
	
	// update
	life += time_elapsed;
	if (life > MAX_LIFE) {
		reset();
	}else{
		for (int i = 0; i < NUM_PARTICLES; i++)
			particles[i]->Update(time_elapsed, life);
	}
}

void ParticleEmitter::reset() {
	life = 0.0f;

	for (int i = 0; i < NUM_PARTICLES; i++) {
		*(particles[i]) = Particle();
	}	
}

void ParticleEmitter::draw()
{

	// iterate throught the list of particles
	for (int i = 0; i < NUM_PARTICLES; i++) {

		// OpenGL goo... don't worry about this
		glVertexPointer(3, GL_FLOAT, 0, squareVertices);
		glEnableClientState(GL_VERTEX_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
		glEnableClientState(GL_COLOR_ARRAY);

		// scale Matrix
		Matrix transformMatrix(particles[i]->scale);

		transformMatrix.translate(CAMERA_MATRIX_ROW_3);
		transformMatrix.translate(particles[i]->position);
		transformMatrix.rotate(particles[i]->rotation);

		// set the transformation matrix
		glLoadMatrixf(reinterpret_cast<float*>(&transformMatrix));

		// draw the trangle strip
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		// clears or flushes some internal setting, used in debug, but is need for performance reasons
		// magic...  really it's magic.
		glGetError();
	}
}


// End of file