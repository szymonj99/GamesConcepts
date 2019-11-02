// AirplaneSimulation.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream> // Used to limit the engine to X FPS
#include <cstdio> // Used to limit the engine to X FPS
#include <chrono> // Used to limit the engine to X FPS
#include <thread> // Used to limit the engine to X FPS
#include <math.h> // Used to round numbers up for FPS limiting.
#include <Windows.h> // Used to get device refresh rate.
using namespace tle; // TLEngine namespace

// Create variables used to limit FPS.
std::chrono::system_clock::time_point timePointA = std::chrono::system_clock::now();
std::chrono::system_clock::time_point timePointB = std::chrono::system_clock::now();

// START OF FPS LIMITER CONSTANTS
const double desiredFPS = GetDeviceCaps(GetDC(nullptr), VREFRESH); // Get the screen's refresh rate. Using nullptr gets the refreshrate of the main display.
const double milliseconds = 1000.0;
const double frameTime = milliseconds / desiredFPS;
// END OF FPS LIMITER CONSTANTS

// Create some constants
const float moveSpeed = 0.20;
const int initialSpeedMultiplier = 480;
const int speedMultiplier = initialSpeedMultiplier / desiredFPS;

// Function to move object with WASDQEZX keys.
void MoveObject(I3DEngine* myEngine, IModel* object)
{
	// The movement is relative.
	if (myEngine->KeyHeld(Key_W))
	{
		// Move object into the screen.
		object->MoveLocalZ(moveSpeed * speedMultiplier);
	}
	if (myEngine->KeyHeld(Key_S))
	{
		// Move object away from the screen.
		object->MoveLocalZ(-moveSpeed * speedMultiplier);
	}
	if (myEngine->KeyHeld(Key_A))
	{
		// Rotate object left.
		object->RotateLocalZ(moveSpeed * speedMultiplier);
	}
	if (myEngine->KeyHeld(Key_D))
	{
		// Rotate object right.
		object->RotateLocalZ(-moveSpeed * speedMultiplier);
	}
	if (myEngine->KeyHeld(Key_Up))
	{
		// Rotate object up.
		object->RotateLocalX(-moveSpeed * speedMultiplier);
	}
	if (myEngine->KeyHeld(Key_Down))
	{
		// Rotate object down.
		object->RotateLocalX(moveSpeed * speedMultiplier);
	}
}

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Programs\\TL-Engine\\Media" );
	// Add relative directory resources
	myEngine->AddMediaFolder("resources");

	/**** Set up your scene here ****/

	// Load "Comic Sans MS" font at 36 points
	IFont* myFont = myEngine->LoadFont("Comic Sans MS", 36);

	// Create mesh and model objects
	IMesh* airplaneMesh = myEngine->LoadMesh("Arrow.x"); // Change to airplane when I get the mesh.
	IMesh* gridMesh = myEngine->LoadMesh("Grid.x");

	// Create model from mesh
	IModel* airplane = airplaneMesh->CreateModel(0, 10, 0);
	IModel* grid = gridMesh->CreateModel();

	// Create camera and attach to arrow.
	ICamera* myCamera = myEngine->CreateCamera(kManual);
	myCamera->AttachToParent(airplane);

	// Define is the game paused.
	bool isPaused = false;

	// Define is mouse captured by engine and capture the mouse immediately.
	bool isMouseCaptured = true;
	myEngine->StartMouseCapture();

	// Define amount of frames since engine started.
	int totalFrames = 0;

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// START OF FPS LIMITER CODE ///////////////////////////////////
		// Set the timePointA to the time right now
		timePointA = std::chrono::system_clock::now();
		// Create a work_time double of type duration, measured in milliseconds (double data type), and set it to the time difference between timePointA and timePointB.
		std::chrono::duration<double, std::milli> const work_time = timePointA - timePointB;

		if (work_time.count() < frameTime)
		{
			// Set the delta_ms duration
			std::chrono::duration<double, std::milli> const delta_ms(frameTime - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		timePointB = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> const sleep_time = timePointB - timePointA;
		// END OF FPS LIMITER CODE ///////////////////////////////////

		if (!isPaused)
		{
			// Draw the scene
			myEngine->DrawScene();

			// Increment totalFrames
			totalFrames++;
			//Print totalFrames on screen.
			myFont->Draw(to_string(totalFrames), 0, 0);

			/**** Update your scene each frame here ****/

			// Control keyboard input.
			// Exit game.
			if (myEngine->KeyHit(Key_Escape))
			{
				myEngine->Stop();
			}
			// Toggle isPaused.
			if (myEngine->KeyHit(Key_P))
			{
				isPaused = !isPaused;
			}
			// Toggle mouse capture.
			if (myEngine->KeyHit(Key_Tab))
			{
				if (isMouseCaptured)
				{
					myEngine->StopMouseCapture();
				}
				else
				{
					myEngine->StartMouseCapture();
				}
				isMouseCaptured = !isMouseCaptured;
			}
			// Move the object with keyboard.
			MoveObject(myEngine, airplane);
		}
		else
		{
			// Toggle isPaused.
			if (myEngine->KeyHit(Key_P))
			{
				// To prevent the camera from moving when resuming game, call GetMouseMovementX/Y()
				myEngine->GetMouseMovementX();
				myEngine->GetMouseMovementY();
				isPaused = !isPaused;
			}
		}

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
