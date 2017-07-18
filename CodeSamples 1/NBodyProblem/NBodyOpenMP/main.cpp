#include <iostream>
#include <omp.h>

#include <System/Util.h>

#include "./Application/OpenMPApplication.h"



int main(int argc, char* argv)
{
	NBody::Application::OpenMPApplication *openMpApp = new NBody::Application::OpenMPApplication("./InputFiles/input_4096.txt", 0.5f, 4);
	openMpApp->Run();

	Core::System::IO::Pause();
	return 0;
}
