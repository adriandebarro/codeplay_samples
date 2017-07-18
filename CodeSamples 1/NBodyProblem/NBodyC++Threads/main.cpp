#include <iostream>

#include <Bodies/Particle.h>
#include <Scheduler/TaskScheduler.h>
#include <System/Util.h>
#include "./Application/ThreadApplication.h"



int main(int argc, char* argv)
{
	NBody::Application::NBodyThread *nbodyApplication = new NBody::Application::NBodyThread("./InputFiles/input_4096.txt", 0.5f, 4);
	nbodyApplication->Run();
	Core::System::IO::Pause();
	return 0;
}
