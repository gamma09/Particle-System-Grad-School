#include "DO_NOT_MODIFY\Timer\Timer.h"
#include "DO_NOT_MODIFY\Timer\GlobalTimer.h"
#include "DO_NOT_MODIFY\OpenGL\OpenGLInterface.h"

#include <assert.h>

#include "ParticleEmitter.h"
#include "mem.h"

static const float SPAWN_FREQUENCY = 0.0000001f; // 

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

static const __m128 PIVOT_VECTOR = _mm_setr_ps(20.0f, 0.0f, 1000.0f, 1.0f);

ParticleEmitter::ParticleEmitter()
:	last_spawn( globalTimer::getTimerInSec() ),
	last_loop(  globalTimer::getTimerInSec() )
{
	SpawnParticles();
}

ParticleEmitter::~ParticleEmitter()
{
	// do nothing
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
	// get the camera matrix from OpenGL
	// need to get the position
	Matrix cameraMatrix;

	// get the camera matrix from OpenGL
	glGetFloatv(GL_MODELVIEW_MATRIX, reinterpret_cast<float*>(&cameraMatrix));

	// iterate throught the list of particles
	for (int i = 0; i < NUM_PARTICLES; i++) {
		// get the position from this matrix
		Vect4D camPosVect = cameraMatrix.getRow(Matrix::MATRIX_ROW_3);

		// OpenGL goo... don't worrry about this
		glVertexPointer(3, GL_FLOAT, 0, squareVertices);
		glEnableClientState(GL_VERTEX_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
		glEnableClientState(GL_COLOR_ARRAY);

		// camera position
		Matrix transCamera;
		transCamera.setTransMatrix(camPosVect);

		// particle position
		Matrix transParticle;
		transParticle.setTransMatrix(particles[i]->position);
		// rotation matrix
		Matrix rotParticle;
		rotParticle.setRotZMatrix(particles[i]->rotation);

		// pivot Point
		Matrix pivotParticle;
		Vect4D pivotVect(PIVOT_VECTOR);
		pivotVect *= life;
		pivotParticle.setTransMatrix(pivotVect);

		// scale Matrix
		Matrix scaleMatrix;
		scaleMatrix.setScaleMatrix(particles[i]->scale);

		// total transformation of particle
		scaleMatrix *= transCamera;
		scaleMatrix *= transParticle;
		scaleMatrix *= rotParticle;

		// set the transformation matrix
		glLoadMatrixf(reinterpret_cast<float*>(&scaleMatrix));

		// draw the trangle strip
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		// clears or flushes some internal setting, used in debug, but is need for performance reasons
		// magic...  really it's magic.
		glGetError();
	}
}


// End of file