#include "DO_NOT_MODIFY\Timer\Timer.h"
#include "DO_NOT_MODIFY\Timer\GlobalTimer.h"
#include "DO_NOT_MODIFY\OpenGL\OpenGLInterface.h"

#include <assert.h>

#include "ParticleEmitter.h"
#include "Settings.h"
#include "mem.h"

static const float SPAWN_FREQUENCY = 0.0000001f;

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
	last_loop(  globalTimer::getTimerInSec() ),
	last_active_particle( -1 ),
	headParticle(0)
{
	// nothing to do
}

ParticleEmitter::~ParticleEmitter()
{
	// do nothing
}

void ParticleEmitter::SpawnParticle()
{
	// create another particle if there are ones free
	if( last_active_particle < NUM_PARTICLES-1 )
	{
	
		// create new particle
		Particle *newParticle = new Particle();

		// increment count
		last_active_particle++;

		// add to list
		this->addParticleToList( newParticle );

	}
}

void ParticleEmitter::update()
{
	// get current time
	float current_time = globalTimer::getTimerInSec();

	// spawn particles
	float time_elapsed = current_time - last_spawn;
	
	// update
	while( SPAWN_FREQUENCY < time_elapsed )
	{
		// spawn a particle
		this->SpawnParticle();
		// adjust time
		time_elapsed -= SPAWN_FREQUENCY;
		// last time
		last_spawn = current_time;
	}
	
	// total elapsed
	time_elapsed = current_time - last_loop;

	Particle *p = this->headParticle;
	// walk the particles

	while( p!= 0 )
	{
		// call every particle and update its position 
		p->Update(time_elapsed);

		// if it's live is greater that the max_life 
		// and there is some on the list
		// remove node
		if((p->life > MAX_LIFE) && (last_active_particle > 0))
		{
			// particle to remove
			Particle *s = p;

			// need to squirrel it away.
			p=p->next;

			// remove last node
			this->removeParticleFromList( s );

			// update the number of particles
			last_active_particle--;
		}
		else
		{
			// increment to next point
			p = p->next;
		}
	}

	//move a copy to vector for faster iterations in draw
	last_loop = current_time;
}
	   
void ParticleEmitter::addParticleToList(Particle *p )
{
	assert(p);
	if( this->headParticle == 0 )
	{ // first on list
		this->headParticle = p;
		p->next = 0;
		p->prev= 0;
	}
	else 
	{ // add to front of list
		headParticle->prev = p;
		p->next = headParticle;
		p->prev = 0;
		headParticle = p;
	}

}

void ParticleEmitter::removeParticleFromList( Particle *p )
{
	// make sure we are not screwed with a null pointer
	assert(p);

	if( p->prev == 0 && p->next == 0  )
	{ // only one on the list
		this->headParticle = 0;
	}
	else if( p->prev == 0 && p->next != 0  )
	{ // first on the list
		p->next->prev = 0;
		this->headParticle = p->next;
	}
	else if( p->prev!= 0 && p->next == 0 )
	{ // last on the list 
		p->prev->next = 0;
	}
	else//( p->next != 0  && p->prev !=0 )
	{ // middle of the list
		p->prev->next = p->next;
		p->next->prev = p->prev;
	}
	
	// bye bye
	delete p;
}

void ParticleEmitter::draw()
{
	// get the camera matrix from OpenGL
	// need to get the position
	Matrix cameraMatrix;

	// get the camera matrix from OpenGL
	glGetFloatv(GL_MODELVIEW_MATRIX, reinterpret_cast<float*>(&cameraMatrix));

	// iterate throught the list of particles
	
	for (Particle* it = this->headParticle; it; it = it->next)
	{
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
		transParticle.setTransMatrix(it->position);
		// rotation matrix
		Matrix rotParticle;
		rotParticle.setRotZMatrix(it->rotation);

		// pivot Point
		Matrix pivotParticle;
		Vect4D pivotVect(PIVOT_VECTOR);
		pivotVect *= it->life;
		pivotParticle.setTransMatrix(pivotVect);

		// scale Matrix
		Matrix scaleMatrix;
		scaleMatrix.setScaleMatrix(it->scale);

		// total transformation of particle
		scaleMatrix *= transCamera;
		scaleMatrix *= transParticle;
		scaleMatrix *= rotParticle;

		// set the transformation matrix
		glLoadMatrixf(reinterpret_cast<float*>(&scaleMatrix));

		// squirrel away matrix for next update
		it->coldData->currMtx = scaleMatrix;

		// draw the trangle strip
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// difference vector
		it->coldData->diffMtx = it->coldData->currMtx;
		it->coldData->diffMtx -= it->coldData->prevMtx;
		
		// clears or flushes some internal setting, used in debug, but is need for performance reasons
		// magic...  really it's magic.
		glGetError();
	}
}


// End of file