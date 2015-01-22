#include <assert.h>

// Offset to local directory
// REALLY useful trick to insure that you link to your code not some other library
#include "DO_NOT_MODIFY\Timer\Timer.h"
#include "DO_NOT_MODIFY\Timer\GlobalTimer.h"
#include "DO_NOT_MODIFY\Event\EventHandler.h"
#include "DO_NOT_MODIFY\OpenGL\OpenGLInterface.h"
#include "ParticleEmitter.h"
#include <cstdio>
#include "mem.h"

// WIN32 - prototype
int main (int argc, char * const argv[]);

// WinMain required by windows for all win32 applications.
// This is our Windows entry point.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	nCmdShow;
	lpCmdLine;
	hPrevInstance;
	OpenGLDevice::SetHInstance(hInstance);
	main(__argc, __argv);
}

// main program
int main (int argc, char * const argv[]) {
	argc;
	argv;
	bool success = false;
	srand(1);

	memSystem::Instance().InitializeSystem();

	// initialize timers:------------------------------
	// Initialize timer
	timer::initTimer();

	// Create a global Timer
	globalTimer::create();

	// Create a timer objects
	timer updateTimer;
	timer drawTimer;

	// create a window:---------------------------------
	success = OpenGLDevice::InitWindow();
	assert(success);
	
	// create an emitter:-------------------------------
	ParticleEmitter emitter;

	// Get the inverse Camera Matrix:-------------------
	// This was calculated offline - the camera does not move

	// initialize the inverse camera matrix:
	// [ 1  0  0   0 ]
	// [ 0  1  0  -3 ]
	// [ 0  0  1 -10 ]
	// [ 0  0  0   1 ]
	Matrix inverseCameraMatrix(
		Vect4D(1.0f, 0.0f, 0.0f, 0.0f),
		Vect4D(0.0f, 1.0f, 0.0f, 0.0f),
		Vect4D(0.0f, 0.0f, 1.0f, 0.0f),
		Vect4D(0.0f, 3.0f, 10.0f, 1.0f));

	// counter for printing
	int i = 0;
	

	// main update loop... do this forever or until some breaks 
	while(OpenGLDevice::IsRunning()) {
		// start update timer ---------------------------------------
		updateTimer.tic();
		{
			// update the emitter
			emitter.update();
		}
		// stop update timer: -----------------------------------------
		updateTimer.toc();

		// start draw... end draw (the draw updates)
		OpenGLDevice::StartDraw();
		{
			// set matrix to Model View
			// push the inverseCameraMarix to stack
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(reinterpret_cast<float*>(&inverseCameraMatrix));
			glPushMatrix();

			// start draw timer: ----------------------------------------
			drawTimer.tic();
			{
				// draw particles
				emitter.draw();
			}
			// stop draw timer: -----------------------------------------
			drawTimer.toc();

			// pop matrix - needs to correspond to previous push
			glPopMatrix();

		}
		// finish draw update
		OpenGLDevice::EndDraw();

		// Love for Windows - allows the windows to behave to external events
		EventHandler::ProcessEvents();

		// update ouput every 50 times
		i++;
		if( i > 50 ) {
			i = 0;
			float updateTime = updateTimer.timeInSeconds();
			float drawTime = drawTimer.timeInSeconds();
			printf("LoopTime: update:%f ms  draw:%f ms  tot:%f\n",updateTime * 1000.0f, drawTime * 1000.0f, (updateTime + drawTime) *1000.0f);
		}
	}

	return 0;
}


// End of file