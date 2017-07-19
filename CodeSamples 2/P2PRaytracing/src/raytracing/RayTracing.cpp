																		//                        ,---.
																		//                       /    |
																		//                      /     |
																		//  Gandalf            /      |
																		//                    /       |
																		//               ___,'        |
																		//             <  -'          :
																		//              `-.__..--'``-,_\_
																		//                 |o/ <o>` :,.)_`>
																		//                 :/ `     ||/)
																		//                 (_.).__,-` |\
																		//                 /( `.``   `| :
																		//                 \'`-.)  `  ; ;
																		//                 | `       /-<
																		//                 |     `  /   `.
																		// ,-_-..____     /|  `    :__..-'\
																		///,'-.__\\  ``-./ :`      ;       \
																		//`\ `\  `\\  \ :  (   `  /  ,   `. \
																		//  \` \   \\   |  | `   :  :     .\ \
																		//   \ `\_  ))  :  ;     |  |      ): :
																		//  (`-.-'\ ||  |\ \   ` ;  ;       | |
																		//   \-_   `;;._   ( `  /  /_       | |
																		//    `-.-.// ,'`-._\__/_,'         ; |
																		//       \:: :     /     `     ,   /  |
																		//        || |    (        ,' /   /   |
																		//        ||                ,'   / SSt|
//----------------------------------------------------------------------------------------------------------------
//			Filename:		RayTracing.cpp
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------


#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define BOOST_ALL_DYN_LINK

#include <winsock2.h>
#include <vector>
#include <chrono>
#include <string>
#include <thread>

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast/lexical_cast_old.hpp>

//testing module
#include <boost/math/distributions/exponential.hpp>

#include "Integrators/RadianceDetails/RadiancePoint.h"

#include "World/AbstractWorld.h"
#include "World/SingleThreadWorld.h"
#include "World/MultiThreadedWorld.h"
#include "CoreClasses/GeometricObjects/GeometricObjects.h"
#include "CoreClasses/GeometricObjects/Objects.h"
#include "./CoreClasses/Point/Point3D/Point3D.h"
#include "CoreClasses/UtilClasses/Common.h"
#include "CoreClasses/Logger/Logger.h"
#include "CoreClasses\Camera\Camera.h"
#include "GuiHelper/gw.h"
#include "CoreClasses/Camera/Camera.h"
#include "CoreClasses/Image Representation/ImageSaver/ImageSaver.h"
#include "Integrators/RadianceDetails/RadiancePoint.h"
#include "Networking/ObservableEventsList/ICObservableEventsList.h"
#include "Integrators/IrradianceCache.h"
#include "Networking/ObservableEvents/InsertEvent.h"
#include "Networking/ObservableEventsList/ICObservableEventsList.h"
#include "Networking/PeerInitializer/PeerInitializer.h"
#include "CoreClasses/Camera/Paths/Checkpoints/Checkpoints.h"
#include "CoreClasses/Logger/SnapshotExtracter.h"
#include "CoreClasses/Camera/Paths/RandomPathGenerator/RandomPathGenerator.h"

#include <boost/bind.hpp>

#include "Loaders/ICSampleLoader.h"

#include "CoreClasses/AssetsManager/SceneManager.h"

#include "System/DumpFileCreator.h"
#include "System/SystemHelper.h"

#include "Networking/ObservableEventsList/ICObservableEventsList.h"
#include "Networking/ObservableEvents/InsertEvent.h"
#include "Networking/ConreteNeighborManagers/PdfNeighborManager.h"
#include "Networking/ObservableEvents/InsertEvent.h"
#include "Networking/ObservableEvents/InvalidationEvent.h"

#include "AccelerationStructures/CachingStructure/LockingOctree.h"

#include "CoreClasses/Logger/CommunicationLogger/CommunicationFileLogger.h"
#include "Networking/TestingClasses/ObservableEventProducer.h"
#include "./CoreClasses/Logger/PostInfoLogger.h"
#include "./CoreClasses/Logger/PostContextInfo.h"

#define SCRWIDTH	600
#define SCRHEIGHT	600
#define SCENE_TYPE 9

#define CAMERA_DELTA 0.5
#define AMOUNT_FRAMES_TOTAL 100
#define VIEW_PLANE_EYE_DISTANCE 1
#define MULTIPLE_FRAMES
#define PATH_AVAILABLE
#define ON_NETWORK


#include <Peer/PeerTestingFramework/PeerTestingFramework.h>


typedef boost::shared_mutex Lock;
typedef boost::unique_lock<Lock> WriteLock;
typedef boost::shared_lock<Lock> ReadLock;

using boost::program_options::typed_value;
using boost::program_options::options_description;

using namespace Engine::Networking;
using namespace Engine;
using namespace Engine::Logs;
using Engine::LockingOctree;

namespace po = boost::program_options;
using namespace boost::math;
using namespace Engine::Networking;

int amountOfPeers = 5;
int peerIndex = 0;
string startupIpAddress = "";
int pathNumber = 0;
string outputFileName = "";
float deltaIntervalBenchmarks = 0.5f;
int useNetwork = 0;
int socketNumber = 0;
string peerName = "";
int sceneType = 0;
int windowSize = 0;
int numberThreads = 0;
int gateKeeper = 1;
float fltFailedCommTm = 0.5f;
float fltSleepAfterCommTm = 0.25f;
string ipAddress = "";
int useRandomPath = 0;
string pathConfigFile = "";
int saveImages = 0;
float alphaVarIC = 0.2;

float deltaFrames = 1;
int adaptableFramePosition = 0;
//time in seconds
float totalTimeForSequence = 300;
bool hasOracleInfo = false;
string peerInfoFile = "";
string oracleDetails = "";
bool updateOracle = false;
bool delayAfterReady = false;
int perFrameDelay = 0;
int gateKeeperNoReply = 0;
int loggingType = 0;
int amountSegments = 3;
int useTwoLevelIC = 0;
float globalICAlpha = 0.2f;
string pathToObjFile = "";
bool waitAfterFrameForUpdates = false;
string jsonSceneDetails = "";
bool useEventSimulator = false;
bool useRestLogger = false;
string restLoggerDetails = "";
bool testRestCommunicationServer = false;
bool staticScene = false;
int amountBounces = 1;
bool produceInvalidations = false;
int windowHeight = 0;
int windowWidth = 0;
string sceneName = "";
int staticFrameCounter = 0;
int sampleRandomizerType = 0;
float cosineWeighting = 0;
int numSamples = 0;
int numSets = 0;
bool shadingNormals = false;
bool dumpICCache = false;
bool loadIcCache = false;
string cacheFilePath = "";
string contextParamsPath = "";
string boundingBoxPath = "";
int connectionTier = 0;
int waitingTimeAfterReady = 500;
int cacheSizePeer = 5;
int invlaidationType = 0;

///-------------------------------------------------------------------------------------------------
/// <summary>	Main entry-point,  </summary>
/// TODO -- Move options to a class implementing the options pattern
/// 
/// <remarks>	Adrian, 19/07/2017. </remarks>
///
/// <param name="argc">	Number of command-line arguments. </param>
/// <param name="argv">	Array of command-line argument strings. </param>
///
/// <returns>	Exit-code for the process - 0 for success, else an error code. </returns>
///-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{

	SetUnhandledExceptionFilter(Engine::System::DumpFile::unhandled_handler);

	po::options_description desc("Allowed options");

	//allowed parameters - need to move to a hashmap
	desc.add_options()
		("help", "This is a pre alpha testing prototype")
		("amountPeers", po::value<string>(), "Amount of peers to be expected on network")
		("peerIndex", po::value<string>(), "Peer index in the whole affair")
		("gatekeeperIp", po::value<string>(), "Ip address of the first peer to communicate with")
		("pathNumber", po::value<string>(), "Path number to use for render")
		("outputFileName", po::value<string>(), "Data Saver file name")
		("intervalForBenchmark", po::value<string>(), "Delta for taking data")
		("timeAfterCommAtempt", po::value<string>(), "Time to wait before reattempting to connect with another peer in seconds")
		("timeSleepAfterFailedAttempt", po::value<string>(), "Time to wait after communication attempt failed, in seconds")
		("useNetwork", po::value<string>(), "Variable to use the network or not")
		("socketNumber", po::value<string>(), "Socket number for listening on the network")
		("gateKeeper", po::value<string>(), "Make instance a gate keeper")
		("peerName", po::value<string>(), "Name of the instance")
		("sceneType", po::value<string>(), "Scene to be rendered")
		("sceneFilePath", po::value<string>(), "Path to the data")
		("windowHeight", po::value<string>(), "Height of render window")
		("windowWidth", po::value<string>(), "Width of render window")
		("windowSize", po::value<string>(), "Setting equal size for the width and height of the render")
		//amount of threads as workers
		("mt", po::value<string>(), "Number of threads")
		//ipaddress  or Randomkey to use for random seed
		("ipAddress", po::value<string>(), "Ip Address used only for random seed (default = 127.0.0.1)")
		//if to use random path-_
		("useRandomPath", po::value<string>(), "1 to use random path and no for not using a random path (default = 0)")
		//path to the config file
		("pathConfigFile", po::value<string>(), "Specify if you are using a random path (default = "")")
		//boolean i
		("saveBackups", po::value<string>(), "Specify if backups will be taken to onedrive account (default = 0)")
		("alphaVarIC", po::value<string>(), "The Alpha constant for the 1 level IC and the local global cache for calculations of diffuse interreflections (default = 0.2)")
		("globalAlphaVarIC", po::value<string>(), "The Alpha constant for the global cache when using two level irradiance cache")
		//ADD THEM TO CAMERA AND ADD METHOD
		("adaptableFramePosition", po::value<string>(), "Allows the frame to be moved according to the time constraint imposed on the system (default = 0)")
		("totalTimeLimitForSequence", po::value<string>(), "Total time animation should take (default = 300 seconds)")
		("deltaFrames", po::value<string>(), "The change in between frames (default = 1.0)")
		//Oracle parameters
		("updateOracle", po::value<string>(), "Update Oracle (default = 0)")
		("oracleDetails", po::value<string>(), "Coomunication details of the oracle to whom data will be sent (default = 127.0.0.1)")
		("perFrameDelay", po::value<string>(), "Apply a delay per frame in seconds (default = 0)")
		("delayAfterReady", po::value<string>(), "Wait for 80 seconds before exiting from the network (default = 0)")
		("gateKeeprNoReply", po::value<string>(), "GateKeeper Never replies back with peers (default = 0)")
		("loggingType", po::value<string>(), "Defines the type of logging to be used through out execution 0 for after frame logging, 1 for parallel logging (default = 0)")
		("amountPointsPerSegment", po::value<string>(), "Difines the amount of segments that will be created between two checkpoints (default = 3)")
		("useTwoLevelCache", po::value<string>(), "Use two level irradiance cache or use the one level irradiance cache (defualt = 0)")
		("queuedInvalidationIc", po::value<string>(), "Octtree is updates after the current frame has been rendered (default = 0)")
		("jsonSceneDetails", po::value<string>(), "Scene details in json format (defaullt='')")
		("testingSimulator", po::value<string>(), "Testing environment to be used to test the performance and the stability of the events")
		("logSenderInfo", po::value<string>(), "Log to external Rest server when Sender is transmitting data (default = 0)")
		("restServerInfo", po::value<string>(), "Rest logger details. Do not add http:// tag before the restServerinfo should only incorprate the server ip follwed by the port number (default = '')")
		("restCommunicationTest", po::value<string>(), "used to check that the rest functionality works with the server")
		("staticScene", po::value<string>(), "Allow the camera to stay in one fixed position (default = 0)")
		("amountBounces", po::value<string>(), "Specify the amount of bounces for the IC (default = 1)")
		("produceInvalidations", po::value<string>(), "Produce invalidations while executing (default = 0)")
		("sceneName", po::value<string>(), "Scene name (default = '')")
		("sampleRandomizerType", po::value<string>(), "Type of randomizer used for samples over hemisphere (default = 0)")
		("cosineWeightingSampling", po::value<string>(), "The cosine weighting for the distribution of samples on a unit hemisphere (default = 0))")
		("numSamples", po::value<string>(), "The amount of rays to be used for a sample (NO DEFAULT)")
		("numSets", po::value<string>(), "The amount of different sets of samples to be produced (NO DEFAULT)")
		("useShadingNormals", po::value<string>(), "Use shading normals or use face normals (default = 0)")
		("dumpIcCache", po::value<string>(), "Once ready dump the IC (defualt = false)")
		("loadIcCache", po::value<string>(), "Load cache from .raw file (default = false)")
		("icCachePath", po::value<string>(), "Path for the  binary file" )
		//context parameters
		("connectionTier", po::value<string>(), "connection tier of the current device")
		("contextParamsDetails", po::value<string>(), "Path for the  binary file")
		("sceneBoundingBoxes", po::value<string>(), "Bounding boxes for the contexts and bounded invalidations")
		("sleepTimeBeforeQuit", po::value<string>(), "Sleeping time before quitting")
		("cacheSizePeer", po::value<string>(), "Cache size limit for each peer")
		("invalidationType", po::value<string>(), "Invalidation type")
		;

	po::options_description hidden("Hidden options");

	hidden.add_options()
		("configInputFile", po::value<string>(), "Input file containing the configuration")
		("secondaryConfigInputFile", po::value<string>(), "Input file containing the configuration")
		;

	po::options_description cmdlineOptions;
	cmdlineOptions.add(desc).add(hidden);
	try
	{
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, cmdlineOptions), vm);

		if (vm.count("configInputFile"))
		{
			ifstream ifs(vm["configInputFile"].as<string>());
			if (!ifs)
			{
				cout << "First command line path is invalid" << endl;
				system("pause");
				return 0;
			}
			else
			{
				po::store(po::parse_config_file(ifs, desc), vm);
				po::notify(vm);
			}
		}

		if (vm.count("secondaryConfigInputFile"))
		{
			ifstream ifss(vm["secondaryConfigInputFile"].as<string>());

			if (!ifss)
			{
				cout << "Second command line is invalid: " << vm["secondaryConfigInputFile"].as<string>() << endl;
				system("pause");
				return 0;
			}
			else
			{
				po::store(po::parse_config_file(ifss, desc), vm);
				po::notify(vm);
			}
		}

		try
		{
			if (vm.count("gateKeeprNoReply"))
			{
				gateKeeperNoReply = std::stoi(vm["gateKeeprNoReply"].as<string>());
			}

			if (vm.count("help"))
			{
				cout << desc << endl;
				system("pause");
				return 0;
			}

			if (vm.count("peerInformation"))
			{
				peerInfoFile = vm["peerInformation"].as<string>();
			}

			if (vm.count("oracleDetails"))
			{
				oracleDetails = vm["oracleDetails"].as<string>();
			}

			if (vm.count("deltaFrames"))
			{
				deltaFrames = std::stof(vm["deltaFrames"].as<string>());
			}

			if (vm.count("totalTimeLimitForSequence"))
			{
				totalTimeForSequence = std::stof(vm["totalTimeLimitForSequence"].as<string>());
			}

			if (vm.count("amountPeers"))
			{
				amountOfPeers = std::stoi(vm["amountPeers"].as<string>());
			}

			if (vm.count("updateOracle"))
			{
				updateOracle = std::stoi(vm["updateOracle"].as<string>()) == 0 ? false : true;
			}

			if (vm.count("alphaVarIC"))
			{
				alphaVarIC = std::stof(vm["alphaVarIC"].as<string>());
			}

			if (vm.count("adaptableFramePosition"))
			{
				adaptableFramePosition = std::stoi(vm["adaptableFramePosition"].as<string>());
			}


			if (vm.count("peerIndex"))
			{
				peerIndex = std::stoi(vm["peerIndex"].as<string>());
			}

			if (vm.count("gatekeeperIp"))
			{
				startupIpAddress = vm["gatekeeperIp"].as<string>();
			}
			else
			{
				startupIpAddress = "123.0.0.5";
			}

			if (vm.count("saveBackups"))
			{
				saveImages = std::stoi(vm["saveBackups"].as<string>());
			}
			else
			{
				saveImages = 0;
			}


			if (vm.count("pathNumber"))
			{
				pathNumber = std::stoi(vm["pathNumber"].as<string>());
			}
			else
			{
				pathNumber = 0;
			}

			if (vm.count("outputFileName"))
			{
				outputFileName = vm["outputFileName"].as<string>();
			}
			else
			{
				outputFileName = "";
			}

			if (vm.count("perFrameDelay"))
			{
				perFrameDelay = std::stoi(vm["perFrameDelay"].as<string>());
			}
			else
			{
				perFrameDelay = 0;
			}

			if (vm.count("intervalBenchmark"))
			{
				deltaIntervalBenchmarks = std::stof(vm["intervalBenchmark"].as<string>());
			}
			else
			{
				deltaIntervalBenchmarks = 0.5f;
			}

			if (vm.count("useNetwork"))
			{
				useNetwork = std::stoi(vm["useNetwork"].as<string>());
			}
			else
			{
				useNetwork = 1;
			}

			if (vm.count("socketNumber"))
			{
				socketNumber = std::stoi(vm["socketNumber"].as<string>());
			}
			else
			{
				socketNumber = 0;
			}

			if (vm.count("peerName"))
			{
				peerName = vm["peerName"].as<string>();
			}
			else
			{
				peerName = "peer";
			}

			if (vm.count("sceneType"))
			{
				sceneType = std::stoi(vm["sceneType"].as<string>());
			}
			else
			{
				sceneType = 7;
			}

			if (vm.count("windowHeight"))
			{
				windowHeight = std::stoi(vm["windowHeight"].as<string>());
			}
			else
			{
				windowHeight = 600;
			}

			if (vm.count("windowWidth"))
			{
				windowWidth = std::stoi(vm["windowWidth"].as<string>());
			}
			else
			{
				windowWidth = 600;
			}

			if (vm.count("windowSize"))
			{
				windowHeight = windowWidth = std::stoi(vm["windowSize"].as<string>());
			}

			if (vm.count("mt"))
			{
				numberThreads = std::stoi(vm["mt"].as<string>());
			}
			else
			{
				numberThreads = 7;
			}

			if (vm.count("gateKeeper"))
			{
				gateKeeper = std::stoi(vm["gateKeeper"].as<string>());
			}
			else
			{
				gateKeeper = 1;
			}

			if (vm.count("timeAfterCommAtempt"))
			{
				fltSleepAfterCommTm = std::stof(vm["timeAfterCommAtempt"].as<string>());
			}

			if (vm.count("timeSleepAfterFailedAttempt"))
			{
				fltFailedCommTm = std::stof(vm["timeSleepAfterFailedAttempt"].as<string>());
			}

			if (vm.count("ipAddress"))
			{
				ipAddress = vm["ipAddress"].as<string>();
			}

			if (vm.count("useRandomPath"))
			{
				useRandomPath = std::stoi(vm["useRandomPath"].as<string>());
			}
			else
			{
				useRandomPath = 0;
			}

			if (vm.count("pathConfigFile"))
			{
				pathConfigFile = vm["pathConfigFile"].as<string>();
			}

			if (vm.count("delayAfterReady"))
			{
				delayAfterReady = std::stoi(vm["delayAfterReady"].as<string>()) == 0 ? false : true;
			}

			if (vm.count("loggingType"))
			{
				loggingType = std::stoi(vm["loggingType"].as<string>());
			}

			if (vm.count("amountPointsPerSegment") )
			{
				amountSegments = std::stoi(vm["amountPointsPerSegment"].as<string>());
			}
			else if(!vm.count("gateKeeper"))
			{
				cout << "Amount segments not added" << endl;
				return 0;
			}

			if (vm.count("useTwoLevelCache"))
			{
				useTwoLevelIC = std::stoi(vm["useTwoLevelCache"].as<string>());
			}

			if (vm.count("globalAlphaVarIC"))
			{
				globalICAlpha = std::stof(vm["globalAlphaVarIC"].as<string>());
			}

			if (vm.count("sceneFilePath"))
			{
				pathToObjFile = vm["sceneFilePath"].as<string>();
			}

			if (vm.count("queuedInvalidationIc"))
			{
				waitAfterFrameForUpdates = std::stoi(vm["queuedInvalidationIc"].as<string>()) == 1;
			}

			if (vm.count("jsonSceneDetails"))
			{
				jsonSceneDetails = vm["jsonSceneDetails"].as<string>();
			}
			else if (!vm.count("gateKeeper"))
			{
				cout << "Please define the .json file describing the scene details" << "\n";
				if (!vm.count("testingSimulator"))
					System::SystemUtil::SystemPause();
			}

			if (vm.count("testingSimulator"))
			{
				useEventSimulator = std::stoi(vm["testingSimulator"].as<string>()) == 1 ? true : false;
			}

			if (vm.count("logSenderInfo"))
			{
				useRestLogger = std::stoi(vm["logSenderInfo"].as<string>()) == 1 ? true : false;
			}

			if (vm.count("restServerInfo"))
			{
				restLoggerDetails = vm["restServerInfo"].as<string>();
			}

			if (vm.count("restCommunicationTest"))
			{
				testRestCommunicationServer = std::stoi(vm["restCommunicationTest"].as<string>()) == 1 ? true : false;
			}

			if (vm.count("staticScene"))
			{
				staticScene = std::stoi(vm["staticScene"].as<string>()) == 0 ? false : true;
			}

			if (vm.count("amountBounces"))
			{
				amountBounces = std::stoi(vm["amountBounces"].as<string>());
			}

			if (vm.count("produceInvalidations"))
			{
				produceInvalidations = std::stoi(vm["produceInvalidations"].as<string>()) == 0 ? false : true;
			}

			if (vm.count("sceneName"))
			{
				sceneName = vm["sceneName"].as<string>();
			}

			if(vm.count("sampleRandomizerType"))
			{
				sampleRandomizerType = stoi(vm["sampleRandomizerType"].as<string>());
			}

			if (vm.count("cosineWeightingSampling"))
			{
				cosineWeighting = std::stof(vm["cosineWeightingSampling"].as<string>());
			}
			 
			if (vm.count("numSamples"))
			{
				numSamples = std::stoi(vm["numSamples"].as<string>());
			}
			else if (!vm.count("gateKeeper"))
			{
				cout << "number of samples was not defined the executable will exit" << endl;
				Engine::System::SystemUtil::SystemPause();
				return 0;
			}

			if (vm.count("numSets"))
			{
				numSets = std::stoi(vm["numSets"].as<string>());
			}
			else if (!vm.count("gateKeeper"))
			{
				cout << "numSets was not defined the executable will exit" << endl;
				Engine::System::SystemUtil::SystemPause();
				return 0;
			}

			if (vm.count("useShadingNormals"))
			{
				Common::USE_VERTEX_NORMALS_SHADING = std::stoi(vm["useshadingNormals"].as<string>()) == 1;
			}

			if(vm.count("dumpIcCache"))
			{
				dumpICCache = std::stoi(vm["dumpIcCache"].as<string>()) == 1;
			}

			if(vm.count("loadIcCache"))
			{
				if(std::stoi(vm["loadIcCache"].as<string>()) == 1)
				{
					loadIcCache = true;
				}
			}

			if (vm.count("contextParamsDetails"))
			{
				contextParamsPath = vm["contextParamsDetails"].as<string>();
				connectionTier = std::stoi(vm["connectionTier"].as<string>());
				boundingBoxPath = vm["sceneBoundingBoxes"].as<string>();
			}
			else if (updateOracle)
			{
				cout << "Oracle activated but no context param details" << endl;
				System::SystemUtil::SystemPause();
				return 0;
			}
			
			if (vm.count("cacheSizePeer"))
			{
				cacheSizePeer = std::stoi(vm["cacheSizePeer"].as<string>());
			}

			if (vm.count("sleepTimeBeforeQuit"))
			{
				waitingTimeAfterReady = std::stoi(vm["sleepTimeBeforeQuit"].as<string>());
			}

			if (vm.count("invalidationType"))
			{
				invlaidationType = std::stoi(vm["invalidationType"].as<string>());
			}

		}
		catch (exception e)
		{
			cout << "Error occured while parsing the file " << __LINE__ << " " << __FILE__ << endl;
			System::SystemUtil::SystemPause();
			return 0;
		}

		if (testRestCommunicationServer)
		{
			cout << "Testing connectivity to the restful server" << "\n";
			Engine::Logs::RESTapi::PostInfoLogger<RadiancePoint, IrradianceCache, WAIT_FREE_TYPE_OCTREE>::GetPeerView(restLoggerDetails);
			System::SystemUtil::SystemPause();
			return 0;
		}

		//Peer acts as a gate keeper
		if (gateKeeper == 0)
		{
			char testChar = ' ';
			Network::Peer::AbstarctPeer<string, IrradianceCache, Update, WAIT_FREE_TYPE_OCTREE>* testPeer = new Network::Peer::P2pPeer<string, IrradianceCache, Update, WAIT_FREE_TYPE_OCTREE>(gateKeeperNoReply);
			cin >> testChar;
			cout << "closing gatekeeper" << endl;
			//testPeer->CloseGateKeeper();
		}
		else if (useEventSimulator)
		{
			Engine::Networking::PeerInitializer<RadiancePoint, IrradianceCache, Update, WAIT_FREE_TYPE_OCTREE>* initPeer = new PeerInitializer<RadiancePoint, IrradianceCache, Update, WAIT_FREE_TYPE_OCTREE>();
			ObservableEventsList<RadiancePoint, IrradianceCache, WAIT_FREE_TYPE_OCTREE>* observableEventsList
				= new Engine::Networking::ObservableEvents::ICObservableEventsList<RadiancePoint, IrradianceCache, WAIT_FREE_TYPE_OCTREE>(
					0,
					16,
					nullptr,
					nullptr,
					nullptr,
					false,
					useRestLogger,
					restLoggerDetails);

			Network::Peer::AbstarctPeer<RadiancePoint, IrradianceCache, Update, WAIT_FREE_TYPE_OCTREE>* testPeer =
				new Networking::Testing::EventProducer::ObservableEventProducer<RadiancePoint, IrradianceCache, Update, WAIT_FREE_TYPE_OCTREE>(startupIpAddress, socketNumber, 1, "test", observableEventsList, nullptr, 16);

			((Networking::Testing::EventProducer::ObservableEventProducer<RadiancePoint, IrradianceCache, Update, WAIT_FREE_TYPE_OCTREE>*)testPeer)->GetSenderThread().join();
		}
		else
		{
			#undef ELPP_HANDLE_SIGABRT
			Engine::VidSaver *saver = nullptr;

			Engine::AbstractWorld * currentWorld = new Engine::MultiThreadWorld(sceneType, pathToObjFile, jsonSceneDetails, windowWidth, windowHeight, numberThreads);
			SnapShotExtracter* snapshotTaker = nullptr;

			// If distributed 
			if (useNetwork == 1)
			{
				P2pPeer<Engine::RadiancePoint, IrradianceCache, Update, LockingOctree>* currentPeer;
				try
				{
					bool firstTime = false;
					gwOpen("Test Window", windowWidth, windowWidth);

					cout << startupIpAddress << endl;

					if (useRandomPath == 0 && pathConfigFile != "" && adaptableFramePosition == 0 && !useEventSimulator)
					{
						_ASSERT(useRandomPath == 0 && pathConfigFile != "");
						//test for alphavar
						currentWorld->Build(pathConfigFile, pathNumber, useTwoLevelIC, alphaVarIC, globalICAlpha, numSamples, numSets, amountSegments, amountBounces, sampleRandomizerType , cosineWeighting);
					}
					else
					{
						cout << "issue with path configuration" << "\n";
					}

					clock_t startTime;

					PeerInitializer<RadiancePoint, IrradianceCache, Update, WAIT_FREE_TYPE_OCTREE>* initPeer = new PeerInitializer<RadiancePoint, IrradianceCache, Update, WAIT_FREE_TYPE_OCTREE>();
					AbstarctPeer<RadiancePoint, IrradianceCache, Update, WAIT_FREE_TYPE_OCTREE, StatePeer>* myPeer =
						initPeer->InitializePeer(amountOfPeers, cacheSizePeer, startupIpAddress, socketNumber, peerIndex, peerName, peerIndex, useTwoLevelIC,
							currentWorld->GetIrradainceCache(), currentWorld->GetCamera(), currentWorld->GetSceneManager(), currentWorld->GetTracer()->GetAccelerationStructure()->m_boundingBox,
							boundingBoxPath, &startTime,fltSleepAfterCommTm, fltFailedCommTm, true, updateOracle ,waitAfterFrameForUpdates, useRestLogger, restLoggerDetails, connectionTier,
							contextParamsPath);
					
					Engine::Loaders::ICSampleLoader* icSamplesLoader = new Engine::Loaders::ICSampleLoader(useTwoLevelIC == 1? ((TwoLayerIrradianceCache*)currentWorld->GetIrradainceCache())->GetGlobalIC() : (IrradianceCache*)currentWorld->GetIrradainceCache());
					
					if (loadIcCache)
					{
						cout << "loading samples" << endl;

						if (icSamplesLoader->PopulateICFromBinaryFile(sceneName))
							cout << "samples loaded succesfully" << endl;
						else
							cout << "error while loading samples" << endl;
					}
					else
					{
						cout << "samples not loaded" << endl;
					}

					if (saveImages == 1)
						saver = new Engine::VidSaver(windowWidth, windowHeight, sceneName, currentWorld->GetRgbBuffer());

					snapshotTaker = new SnapShotExtracter(useTwoLevelIC, currentWorld->GetIrradainceCache(), pathNumber, currentWorld->GetCamera(), loggingType, outputFileName);

					vector<InvalidationDetails*> changeList;

					if (adaptableFramePosition == 1)
						currentWorld->GetCamera()->SetTotalTimeLimit(totalTimeForSequence);

					int index = 0;

					clock_t fullStartTime = clock();
					clock_t fullEndTime;
					int amountOfFrames = 0;

					while (currentWorld->CheckIfMoreFrames())
					{
						amountOfFrames++;

						startTime = clock();
						Engine::Logger::LogInfo("New Frame");
						while (!currentWorld->RenderScene())
						{
							gwDoEvents();
						}

						clock_t endTime = clock();
						double timeInSeconds = ((double)endTime - startTime) / (double)CLOCKS_PER_SEC;

						if (loggingType == 0)
						{
							snapshotTaker->EnterEndOfFrameDetails(timeInSeconds);
						}

						Engine::Logger::LogTrace("time taken is equal to " + to_string(timeInSeconds));
						Engine::Logger::LogTrace("Frame Ready " + index);
						
						if (waitAfterFrameForUpdates)
							initPeer->GetObservableEventsList()->PushQueuedEvents();

						if (saveImages == 1 && saver != nullptr)
							saver->SaveFrame();


						if (!staticScene)
						{
							if (adaptableFramePosition == 0)
								currentWorld->MoveCamera();
							else
								currentWorld->MoveCamera(timeInSeconds);
						}
						else
						{
							if (staticFrameCounter == 0) staticFrameCounter++;
							else break;
						}

						//check if any invalidations have occured
						if (produceInvalidations)
						{
							bool foundUpdates = currentWorld->GetSceneManager()->GetLightManager()->CheckForNewEvents(changeList, currentWorld->GetCamera()->GetCameraPosition());

							if (foundUpdates)
							{
								cout << "Invalidation occured" << endl;
								initPeer->AddInvalidationEvents(changeList, invlaidationType, initPeer->GetBoudningBoxIndex(currentWorld->GetCamera()->GetCameraPosition()));
								changeList.erase(changeList.begin(), changeList.end());
							}
						}

						//if updates from the invlaidation and after are pushed after the frame was finished 
						if (waitAfterFrameForUpdates)
						{
							initPeer->PushPendingUpdates();
						}

						// Add artificial delay if required
						if (perFrameDelay > 0)
						{
							this_thread::sleep_for(std::chrono::seconds(perFrameDelay));
						}
					}

					//Benchmark time 
					fullEndTime = clock();

					double timeInSeconds = ((double)fullEndTime - fullStartTime) / (double)CLOCKS_PER_SEC;

					cout << "Time Taken is " << timeInSeconds << endl;

					snapshotTaker->TotalTimeTaken(timeInSeconds);
					
					// Log the amount of stored samples in the Irradiance Cache 
					if (useTwoLevelIC == 0)
						snapshotTaker->TotalUsedNetworkPoints(((IrradianceCache*)currentWorld->GetIrradainceCache())->GetOctree()->GetAmountRadiancePoint());
					else if (useTwoLevelIC == 1)
						snapshotTaker->TotalUsedNetworkPoints(((TwoLayerIrradianceCache*)currentWorld->GetIrradainceCache())->GetGlobalIC()->GetOctree()->GetAmountRadiancePoint());

					// Stop benchmarking
					snapshotTaker->StopBenchMarks();

					cout << "PEER READY!!" << endl;

					if(dumpICCache)
					{
						if (icSamplesLoader->ExportSamples(sceneName))
							cout << "IC samples exported successfully" << endl;
						else
							cout << "IC samples not exported successfully" << endl;
					}

					// Wait for further connections 
					if (delayAfterReady)
						this_thread::sleep_for(std::chrono::seconds(waitingTimeAfterReady));
					
					// ShutDown Peer
					myPeer->Exitlistening();
					if(updateOracle)
						initPeer->GetContextUpdater()->TurnOffContextUpdates();

					//wait for running contexts to finish 
					if (updateOracle)
					{
						while (initPeer->GetContextInitializer()->CheckForRunningContexts())
						{
							this_thread::sleep_for(std::chrono::seconds(30));
						}
					}
				}
				catch (exception e)
				{
					string outputException = e.what();
					Engine::Logger::LogError(outputException);
					CloseWindow();
				}

				Engine::Logger::LogInfo("Render Ready");
				CloseWindow();
				delete currentWorld;
				return 0;
			}
			//Render Only
			else
			{
				try
				{
					bool firstTime = false;
					gwOpen("Test Window", windowWidth, windowWidth);

					//print the gatekeeper IP address
					cout << startupIpAddress << endl;

					if (useRandomPath == 0 && pathConfigFile != "" && adaptableFramePosition == 0 && !useEventSimulator)
					{
						_ASSERT(useRandomPath == 0 && pathConfigFile != "");
						currentWorld->Build(pathConfigFile, pathNumber, useTwoLevelIC, alphaVarIC, globalICAlpha, numSamples, numSets, amountSegments, amountBounces, sampleRandomizerType, cosineWeighting);
					}
					else
					{
						cout << "Path configuration issue" << "\n";
					}

					Engine::Loaders::ICSampleLoader* icSamplesLoader = new Engine::Loaders::ICSampleLoader((IrradianceCache*)currentWorld->GetIrradainceCache());

					if (loadIcCache)
					{
						cout << "loading samples" << endl;

						if (icSamplesLoader->PopulateICFromBinaryFile(sceneName))
							cout << "sampoles loaded succesfully" << endl;
						else
							cout << "error while loading samples" << endl;

						cout << "points loaded from file: " << ((IrradianceCache*)currentWorld->GetIrradainceCache())->GetNetworkRadianceCount() << endl;
					}

					Engine::VidSaver *saver = nullptr;

					if (saveImages == 1)
						saver = new Engine::VidSaver(windowWidth, windowHeight, sceneName, currentWorld->GetRgbBuffer());

					
					snapshotTaker = new SnapShotExtracter(useTwoLevelIC, currentWorld->GetIrradainceCache(), pathNumber, currentWorld->GetCamera(), loggingType, outputFileName);

					vector<InvalidationDetails*> changeList;

					if (adaptableFramePosition == 1)
						currentWorld->GetCamera()->SetTotalTimeLimit(totalTimeForSequence);

					int index = 0;

					clock_t fullStartTime = clock();
					clock_t fullEndTime;
					int amountOfFrames = 0;

					while (currentWorld->CheckIfMoreFrames())
					{
						amountOfFrames++;

						clock_t startTime = clock();
						Engine::Logger::LogInfo("New Frame");
						while (!currentWorld->RenderScene())
						{
							gwDoEvents();
						}

						clock_t endTime = clock();
						double timeInSeconds = ((double)endTime - startTime) / (double)CLOCKS_PER_SEC;

						if (loggingType == 0)
						{
							snapshotTaker->EnterEndOfFrameDetails(timeInSeconds);
						}

						Engine::Logger::LogTrace("time taken is equal to-> " + to_string(timeInSeconds));
						Engine::Logger::LogTrace("Frame Ready " + index);
						// print the count of IC samples present in the cache
						cout << "-----" << ((IrradianceCache*)currentWorld->GetIrradainceCache())->GetLocalRadianceCount() << "----" << endl;
						if (saveImages == 1 && saver != nullptr)
							saver->SaveFrame();

						Engine::Logger::LogInfo("ready after saving");

						if (!staticScene)
						{
							if (adaptableFramePosition == 0)
								currentWorld->MoveCamera();
							else
								currentWorld->MoveCamera(timeInSeconds);
						}
						else
						{
							if (staticFrameCounter == 0) staticFrameCounter++;
							else break;
						}

						if (perFrameDelay > 0)
						{
							this_thread::sleep_for(std::chrono::seconds(perFrameDelay));
						}
					}

					fullEndTime = clock();

					double timeInSeconds = ((double)fullEndTime - fullStartTime) / (double)CLOCKS_PER_SEC;

					cout << "Time Taken is " << timeInSeconds << endl;

					snapshotTaker->TotalTimeTaken(timeInSeconds);
						
					if (useTwoLevelIC == 0)
						snapshotTaker->TotalUsedNetworkPoints(((IrradianceCache*)currentWorld->GetIrradainceCache())->GetOctree()->GetAmountRadiancePoint());
					else if (useTwoLevelIC == 1)
						snapshotTaker->TotalUsedNetworkPoints(((TwoLayerIrradianceCache*)currentWorld->GetIrradainceCache())->GetGlobalIC()->GetOctree()->GetAmountRadiancePoint());
					
					snapshotTaker->StopBenchMarks();

					if (dumpICCache)
					{
						if (icSamplesLoader->ExportSamples(sceneName))
							cout << "IC samples exported successfully" << endl;
						else
							cout << "IC samples not exported successfully" << endl;
					}

					Engine::Logger::LogInfo("Render Ready");

					//std::system("pause");
					CloseWindow();
					delete currentWorld;
					return 0;
				}
				catch (exception ex)
				{
					cout << ex.what() << endl;
					System::SystemUtil::SystemPause();
					return 0;
				}
			}
		}
	}
	catch (exception ex)
	{
		cout << ex.what() << "at line " << __LINE__ << endl;
		System::SystemUtil::SystemPause();
		return 0;
	}
	return 0;
}
///-------------------------------------------------------------------------------------------------
// End of RayTracing.cpp
///-------------------------------------------------------------------------------------------------

