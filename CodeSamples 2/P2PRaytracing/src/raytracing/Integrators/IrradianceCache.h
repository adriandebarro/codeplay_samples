#pragma once 
//----------------------------------------------------------------------------------------------------------------
//			Filename:		IrradianceCache.h
//			  Author:		Adrian De Barro
//			    Date:		13/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <memory>
#include <string>
#include <iostream>
#include <mutex>
#include <assert.h>
#include <atomic>

#include <boost/limits.hpp>

#include <Peer/Peer2Peer-Peer/P2pPeer.h>

#include "..\Integrators\Integrator.h"
#include "..\AccelerationStructures\CachingStructure\Octree.h"
#include "..\Scene\Scene.h"
#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "..\AccelerationStructures\BVH.h"
#include "..\CoreClasses\Ray\Ray.h"
#include "..\CoreClasses\Sampler\Sampler.h"
#include "../CoreClasses\Sampler\Jittered.h"
#include "../CoreClasses/Sampler/RandomSampler.h"

#include "..\Tracer\Tracer.h"
#include "..\CollisionDetails\ShadeRec.h"
#include "..\CoreClasses\UtilClasses\Common.h"
#include "..\CoreClasses\Logger\Logger.h"
#include "..\Material\Material.h"
#include "../Integrators/RadianceDetails/RadiancePoint.h"
#include "../CoreClasses/GeometricObjects/GeometricObjects.h"
#include "../CoreClasses/GeometricObjects/Quad.h"
#include "../CoreClasses/Point/Point3D/Point3D.h"

#include "../AccelerationStructures/CachingStructure/LockingOctree.h"
#include "../AccelerationStructures/CachingStructure/WaitFreeOctree.h"

#include "../Networking/ObservableEventsMonitor/ICUpdatesMonitor.h"
#include "../AccelerationStructures/AccelerationStructure.h"

#include "../Networking/PeerInfo/Update.h"

#include "../CoreClasses/Debugging/MatlabUtilityDebuger.h"


#include "../Scene/Scene.h"


using std::string;
using namespace Network::Peer;
using Engine::Networking::Update;
using Engine::AccelerationStructures::AccelerationStructure;



namespace Engine
{
	#define MAXH_DEPTH 1
	//#define DEBUG_MODE
	#define USE_ORTHAGONAL_SHIFTING true
	#define DEBUG_SAMPLES
	#define AMOUNT_DEVISABLE_CONSTANT 60;
	#define MONITORING_MODE
	#define DEBUG_MODE

#ifdef WAIT_FREE
	typedef Engine::AccelerationStructures::OctTree::WaitFreeOctTree<RadiancePoint, 14, 2000> WAIT_FREE_TYPE_OCTREE;
#else
	typedef LockingOctree WAIT_FREE_TYPE_OCTREE;
#endif 

	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: IrradianceCache
	//		   Description: The algorithm taking care of the in-direct lighting
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class IrradianceCache :public Integrator
	{
	public:

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		IrradianceCache() : m_currentShadeRec(ShadeRec())
		{
			m_currentShadeRec = ShadeRec();
			m_networkRadianceCount = ATOMIC_VAR_INIT(0);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Scene*, Tracer*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		IrradianceCache(Scene* p_scene, Tracer* p_tracerPtr, int p_numSamples = 32, int p_numSets = 2500) : m_currentShadeRec(ShadeRec())
		{
			#ifndef MULTI_THREADED
				octreeStructure = new Octree(p_scene);
			#else 
				#ifdef WAIT_FREE
					//octreeStructure = new Engine::AccelerationStructures::OctTree::WaitFreeOctTree<RadiancePoint, 7, 2000>(Point3D(0), Point3D(100));
				#else
					octreeStructure = new LockingOctree(p_scene);
				#endif
			#endif

			accelerationStructure = new BVH(p_scene->m_sceneObjects, MIDDLE_POINT, true);
			m_scenePtr = p_scene;

			m_numSamples = p_numSamples;
			m_numSets = p_numSets;

			m_sampler = new Jittered(p_numSamples, p_numSets, 0);
			m_sampler->GenerateSamples();
			m_sampler->MapSamplesToUnitDisk();
			m_sampler->MapSamplesToHemisphere(0, true);
			m_tracerPtr = p_tracerPtr;
			m_currentShadeRec = ShadeRec();
			GetMinmumRayLength();
			m_networkRadianceCount = ATOMIC_VAR_INIT(0);
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Constructor. </summary>
		///
		/// <remarks>	Adrian, 21/09/2015. </remarks>
		///
		/// <param name="p_scene">	  	[in,out] If non-null, the scene. </param>
		/// <param name="p_tracerPtr">	[in,out] If non-null, the tracer pointer. </param>
		/// <param name="p_peerClass">	[in,out] If non-null, the peer class. </param>
		///-------------------------------------------------------------------------------------------------

		IrradianceCache(Scene* p_scene, Tracer* p_tracerPtr, P2pPeer<RadiancePoint, IrradianceCache, Update, LockingOctree>* p_peerClass, int p_numSamples, int p_numSets) : m_currentShadeRec(ShadeRec())
		{
			#ifndef MULTI_THREADED
						octreeStructure = new Octree(p_scene);
			#else 
				#ifdef WAIT_FREE
							octreeStructure = new WAIT_FREE_TYPE_OCTREE(Point3D(0), Point3D(100));
				#else
							octreeStructure = new LockingOctree(p_scene);
				#endif
			#endif

			m_numSamples = p_numSamples;
			m_numSets = p_numSets;

			accelerationStructure = new BVH(p_scene->m_sceneObjects, MIDDLE_POINT, true);
			m_scenePtr = p_scene;
			m_sampler = new Jittered(p_numSamples, p_numSets, 0);
			m_sampler->GenerateSamples();
			m_sampler->MapSamplesToUnitDisk();
			m_sampler->MapSamplesToHemisphere(0, true);
			m_tracerPtr = p_tracerPtr;
			m_currentShadeRec = ShadeRec();
			GetMinmumRayLength();
			m_networkRadianceCount = ATOMIC_VAR_INIT(0);
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Scene*, BVH*, Tracer*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		IrradianceCache(Scene* p_scene, AccelerationStructure* p_bvh, Tracer* p_tracerPtr, float p_AlphaConstant, int p_numSamples, int p_numSets, int p_amountBounces = 1, int p_sampleRandomizerType = 0, int p_cosineWeighting = 0) : m_currentShadeRec(ShadeRec())
		{
			#ifndef MULTI_THREADED
				octreeStructure = new Octree(p_scene);
			#else 
			#ifdef WAIT_FREE
				octreeStructure = new WAIT_FREE_TYPE_OCTREE(p_bvh);
			#else
				octreeStructure = new LockingOctree(p_scene);
			#endif
			#endif

			m_cosineWeighting = p_cosineWeighting;
			m_numSamples = p_numSamples;
			m_numSets = p_numSets;

			accelerationStructure = p_bvh;
			m_scenePtr = p_scene;

			m_samplerType = p_sampleRandomizerType;
			if (p_sampleRandomizerType == 0)
			{
				m_sampler = new Jittered(p_numSamples, p_numSets, p_cosineWeighting);
				m_sampler->GenerateSamples();
				m_sampler->MapSamplesToUnitDisk();
			}
			else
			{
				m_sampler = new Engine::Sampling::RandomSampler(p_numSamples, p_numSets, p_cosineWeighting);
				m_sampler->GenerateSamples();
				m_sampler->MapSamplesToUnitDisk();
			}

			m_sampler->MapSamplesToHemisphere(m_cosineWeighting, false);
			m_tracerPtr = p_tracerPtr;
			m_currentShadeRec = ShadeRec();
			GetMinmumRayLength();
			m_AlphaConstant = p_AlphaConstant;
			m_amountBounces = p_amountBounces;
			m_networkRadianceCount = ATOMIC_VAR_INIT(0);
		}


		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetIndirectLightingAtPoint
		//		Return Value: RGBColor
		//		  Parameters: CollisionDetails
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using Integrator::GetIndirectLightingAtPoint;
		RGBColor GetIndirectLightingAtPoint(ShadeRec& p_intersectionDetails)
		{
			m_currentShadeRec = ShadeRec(p_intersectionDetails);

			RGBColor output;

			if (Common::USE_VERTEX_NORMALS_SHADING)
				output = GetOrCreate(p_intersectionDetails.m_shadingNormal, p_intersectionDetails);
			else
				output = GetOrCreate(p_intersectionDetails.GetNormal(), p_intersectionDetails);

			return output;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetOrCreate
		//		Return Value: RGBColor
		//		  Parameters: RadiancePoint*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor GetOrCreate(Point3D p_normal, ShadeRec& p_collisionDetails)
		{
			RadiancePoint* radiancePoint = nullptr;
			vector<int> availableRadiancePoints;
			//create new radiane point
			if (!octreeStructure->GetPoints(p_collisionDetails, p_normal, availableRadiancePoints, m_AlphaConstant))
			{
				RGBColor toReturn = FireSamples(p_collisionDetails);
				//return toReturn;
				return RGBColor(1, 0, 0);
			}
			else if (availableRadiancePoints.size() > 0)
			{
				#ifndef WAIT_FREE
					//cant put it higher up, since the writer in the FireSamples wont be able to write 
					ReadLock r_lock(octreeStructure->m_myLock);
					//-------------------------------------------Readers Lock------------------------------------------
				#endif
				if (Common::USE_VERTEX_NORMALS_SHADING)
					return ComputeInterpolation(availableRadiancePoints, p_collisionDetails.localHitPoint, p_collisionDetails.m_shadingNormal);
				else
					return ComputeInterpolation(availableRadiancePoints, p_collisionDetails.localHitPoint, p_collisionDetails.GetNormal());
			}

			return RGBColor(0);
		}

		RadiancePoint* GetInterpolationOrCompute(ShadeRec& p_collisionDetails, Point3D p_normal)
		{
			vector<int> availableRadiancePoints;
			bool foundPoints = octreeStructure->GetPoints(p_collisionDetails, p_normal, availableRadiancePoints, m_AlphaConstant);

			vector<int> filteredVector;
			if (availableRadiancePoints.size() > 0)
			{
				for (int sampleIndex = 0; sampleIndex < availableRadiancePoints.size(); sampleIndex++)
				{
					RadiancePoint*  currentSample = octreeStructure->GetPointByIndex(availableRadiancePoints[sampleIndex]);
					if (currentSample != nullptr)
						filteredVector.push_back(availableRadiancePoints[sampleIndex]);
				}
			}

			if (foundPoints && filteredVector.size() > 0)
			{

				if (Common::USE_VERTEX_NORMALS_SHADING)
					return ComputeRadiancePointFromInterpolation(filteredVector, p_collisionDetails.localHitPoint, p_collisionDetails.m_shadingNormal);
				else
					return ComputeRadiancePointFromInterpolation(filteredVector, p_collisionDetails.localHitPoint, p_collisionDetails.GetNormal());
			}
			else
			{
				RadiancePoint* currentSample = ProduceRadiancePoint(p_collisionDetails);
				return currentSample;
			}
		}



		bool GetInterpolation(RGBColor& p_outputColor, ShadeRec& p_collisionDetails, Point3D p_normal)
		{
			vector<int> availableRadiancePoints;
			bool foundPoints = octreeStructure->GetPoints(p_collisionDetails, p_normal, availableRadiancePoints, m_AlphaConstant);

			if (foundPoints)
			{
				p_outputColor = ComputeInterpolation(availableRadiancePoints, p_collisionDetails.localHitPoint, p_normal);
				return true;
			}
			return false;
		}


		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ComputeInterpolation
		//		Return Value: RGBColor
		//		  Parameters: vector<RadiancePoint*>, RadiancePoint*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor ComputeInterpolation(vector<int>& points, Point3D p_position, Point3D p_normal)
		{
			float output = 0;
			RGBColor summationIrradianceWeight = 0;
			float summationWeight = 1;

			for (int index = 0; index < points.size(); index++)
			{
				RadiancePoint* currentPoint = octreeStructure->GetPointByIndex(points[index]);
				//TODO: Fix this part, this computation is repeated twice
				float currentWeight = 0.0f;

				if (currentPoint == nullptr)
					continue;
				else
					currentWeight = GetRadiancePointWeight(currentPoint, p_position, p_normal);

				currentPoint->m_usedCounter++;

				summationIrradianceWeight = summationIrradianceWeight + currentPoint->radiance*currentWeight;
				summationWeight += currentWeight;
			}

			#ifdef SHADING_NORMAL_TEST
				RGBColor toReturn(p_normal.x, p_normal.y, p_normal.z);
				return toReturn;
			#else
				return summationIrradianceWeight / summationWeight;
			#endif

		}

		RadiancePoint* ComputeRadiancePointFromInterpolation(vector<int>& points, Point3D p_position, Point3D p_normal)
		{
			float output = 0;
			RGBColor summationIrradianceWeight = 0;
			float summationWeight = 0;
			float minRayLength = boost::numeric::bounds<float>::highest();

			for (int index = 0; index < points.size(); index++)
			{
				RadiancePoint* currentPoint = octreeStructure->GetPointByIndex(points[index]);

				float currentWeight = GetRadiancePointWeight(currentPoint, p_position, p_normal);

				minRayLength = min(minRayLength, currentPoint->m_rayLength);

				summationIrradianceWeight = summationIrradianceWeight + currentPoint->radiance*currentWeight;
				summationWeight += currentWeight;
				currentPoint->m_usedCounter++;
				if (currentPoint->GotFrom() == 1)
				{
					currentPoint->m_usedCounter++;
				}
			}

			RGBColor radiance = summationIrradianceWeight / summationWeight;
			RadiancePoint* toReturn = new RadiancePoint(p_position, p_normal, minRayLength, radiance);

			return toReturn;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: FireSamples
		//		Return Value: RGBColor
		//		  Parameters: ShadeRec
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor FireSamples(ShadeRec& sr)
		{
			RadiancePoint* newRadiancePoint = new RadiancePoint(0);
			newRadiancePoint->m_position = sr.localHitPoint;

			newRadiancePoint->m_shadingNormal = sr.m_shadingNormal;
			newRadiancePoint->hitPointNormal = sr.GetNormal();

			#ifdef SHADING_NORMAL_TEST
						RGBColor toReturn = RGBColor(newRadiancePoint->m_shadingNormal.x, newRadiancePoint->m_shadingNormal.y, newRadiancePoint->m_shadingNormal.z);
						return toReturn;
			#else
				return FireSamplesNAdd(newRadiancePoint, (Triangle*)sr.m_hitTriangle);
			#endif
		}

		RadiancePoint* ProduceRadiancePoint(ShadeRec& sr)
		{
			RadiancePoint* newRadiancePoint = new RadiancePoint(0);
			newRadiancePoint->m_position = sr.localHitPoint;
			newRadiancePoint->hitPointNormal = Point3D(sr.normal.x, sr.normal.y, sr.normal.z);
			newRadiancePoint->m_shadingNormal = sr.m_shadingNormal;
			FireSamplesNAdd(newRadiancePoint, (Triangle*)sr.m_hitTriangle);
			AddToUpdates(newRadiancePoint);
			return newRadiancePoint;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: FireSamples
		//		Return Value: RGBColor
		//		  Parameters: RadiancePoint*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor FireSamplesNAdd(RadiancePoint* p_currentPoint, Triangle* p_intersectedTriangle)
			//RGBColor FireSamplesNAdd(RadiancePoint* p_currentPoint)
		{
			ShadeRec rec;
			Sample(p_currentPoint, m_amountBounces, p_intersectedTriangle);

			#ifndef WAIT_FREE
				octreeStructure->InsertPoint(*p_currentPoint);
			#else
				octreeStructure->InsertPoint(p_currentPoint);
			#endif
			
			AddToUpdates(p_currentPoint);
			//IncrementLocalRadiancePointCount();
			return p_currentPoint->radiance;
		}

		void InsertImportedPoint(RadiancePoint* p_currentSample)
		{

			#ifndef WAIT_FREE
				octreeStructure->InsertPoint(*p_currentSample);
			#else
				octreeStructure->InsertPoint(p_currentSample);
			#endif

			AddToUpdates(p_currentSample);
		}

		void AddUpdate(RadiancePoint* p_currentPoint)
		{
			#ifndef WAIT_FREE
				octreeStructure->InsertPoint(*p_currentPoint);
			#else
				octreeStructure->InsertPoint(p_currentPoint);
			#endif

			IncrementLocalRadiancePointCount();
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: Sample
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: 
		//----------------------------------------------------------------------------------------------------------------
		RGBColor Sample(RadiancePoint* p_collisionPoint, int currentDepth, Triangle* currentTriangle)
			//RGBColor Sample(RadiancePoint* p_collisionPoint, int currentDepth)
		{
			//[AD] debugging
			//int count = 0;

			vector<ShadeRec*> collisions;
			vector<Point3D> samples;

			Point3D arbitraryAxis_Z(0);
			Point3D arbitraryAxis_X(0);

			/*if(currentTriangle->GetMaterial()->GetMaterialName() == "material_3")
			{
				cout << "here!!" << endl;
			}*/

			if (currentDepth == 1)
			{
				Twister randomNumberGenerator;

				RGBColor resultingOutput;
				for (int sampleIndex = 0; sampleIndex < m_numSamples; ++sampleIndex)
				{
					try
					{
						Point3D currentSample(0);

						if (Common::USE_VERTEX_NORMALS_SHADING)
							currentSample = m_sampler->SampleHemispherical(sampleIndex, p_collisionPoint->m_shadingNormal, p_collisionPoint->m_collisionRay.d, arbitraryAxis_X, arbitraryAxis_Z, randomNumberGenerator);
						else
							currentSample = m_sampler->SampleHemispherical(sampleIndex, p_collisionPoint->hitPointNormal, p_collisionPoint->m_collisionRay.d, arbitraryAxis_X, arbitraryAxis_Z, randomNumberGenerator);

						_ASSERT(currentSample.x != 0 || currentSample.y != 0 || currentSample.z != 0);
						currentSample.normalize();
						samples.push_back(currentSample);
						//Ray currentRay(p_collisionPoint->GetEpsillonHitPoint(), p_collisionPoint->hitPointNormal);

						Ray currentRay(p_collisionPoint->GetEpsillonHitPoint(), currentSample);
						ShadeRec* currentCollision = new ShadeRec();
						FireSamplesAtPoint(currentRay, currentCollision);
						////[AD] debugging
						//if (currentCollision->hitAnObject && currentCollision->m_hitTriangle->GetMaterial()->GetMaterialName() == "pod_tepih")
						//	count += 1;

						collisions.push_back(currentCollision);
					}
					catch (exception e)
					{
						std::string excep = e.what();
						cout << "The following exception took place " << excep << "\n";
					}
				}

				p_collisionPoint->ComputeRadianceNHarmonivValue(collisions, m_minimumRayLength);

				////[AD] debugging
				//if (count > 50)
				//	cout << "Qbadnih b" << count << " " << p_collisionPoint->radiance.ToString() <<endl;
				//_ASSERT(p_collisionPoint->hitPointNormal.x != 0 || p_collisionPoint->hitPointNormal.y != 0  p_collisionPoint->hitPointNormal.z != 0);
				std::for_each(collisions.begin(), collisions.end(), std::default_delete<ShadeRec>());
				return p_collisionPoint->radiance;
			}
			else
			{
				int amountSamples = m_sampler->GetNumSamples();

				RGBColor resultingOutput;

				int nextDepth = --currentDepth;

				for (int sampleIndex = 0; sampleIndex < m_numSamples; ++sampleIndex)
				{
					//got next direction from sampler
					Point3D direction = m_sampler->SampleHemispherical(sampleIndex);
					//created the next direction 
					Ray currentRay(p_collisionPoint->GetEpsillonHitPoint(), direction);
					ShadeRec *rec = new ShadeRec();
					double tmin = INT_MAX;
					//caculated intersection

					FireSamplesAtPoint(currentRay, rec);

					//bool intersection = accelerationStructure->TestIntersection(currentRay, rec, tmin);
					//sample next position 
					if (rec->hitAnObject)
					{
						RadiancePoint* currentPoint = new RadiancePoint(rec->localHitPoint);
						//recursvley sample next position 
						RGBColor tempColor = Sample(currentPoint, nextDepth, currentTriangle);

						//if (tempColor.r > 0 || tempColor.g > 0 || tempColor.b > 0)
						//	cout << "found light !!" << endl;
						resultingOutput += tempColor;
						currentPoint->SetRadiance(resultingOutput);
						//add 
						#ifndef WAIT_FREE
							octreeStructure->InsertPoint(*currentPoint);
						#else
							octreeStructure->InsertPoint(currentPoint);
						#endif
						AddToUpdates(currentPoint);
					}
					//average the irradiance at the point
					resultingOutput = resultingOutput / m_numSamples;

				}

				// [AD] need to fix here 
				p_collisionPoint->radiance = resultingOutput / m_numSamples;

				AddUpdate(p_collisionPoint);

				return resultingOutput;
			}
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Adds to the updates. </summary>
		///
		/// <remarks>	Adrian, 25/09/2015. </remarks>
		///
		/// <param name="p_radiancePoint">	[in,out] If non-null, the radiance point. </param>
		///-------------------------------------------------------------------------------------------------

		void AddToUpdates(RadiancePoint* p_radiancePoint)
		{
			mtx.lock();
				m_localUpdates.push_back(p_radiancePoint);

				if (p_radiancePoint->GotFrom() == 0)
					IncrementLocalRadiancePointCount();
				else
					IncrementNetworkRadiancePointCount();
			mtx.unlock();
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Increment local radiance point count. </summary>
		///
		/// <remarks>	Adrian, 19/10/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		void IncrementLocalRadiancePointCount()
		{
			m_localRadianceCount++;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Increment network radiance point count. </summary>
		///
		/// <remarks>	Adrian, 19/10/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		void IncrementNetworkRadiancePointCount()
		{
			std::atomic_fetch_add(&m_networkRadianceCount, 1);
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: FireSamplesAtPoint
		//		Return Value: VOID
		//		  Parameters: Ray, CollisionDetails
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void FireSamplesAtPoint(Ray currentRay, ShadeRec* details)
		{
			double tmin = INT_MAX;
			bool intersectionTest = accelerationStructure->TestIntersection(currentRay, *details, tmin);
			if (intersectionTest)
			{
				details->hitAnObject = true;
				Material* currentMaterial = nullptr;
				if (details->m_hitTriangle != nullptr)
					currentMaterial = details->m_hitTriangle->GetMaterial();
				else
					currentMaterial = details->object->GetMaterial();

				RGBColor colorAtPoint;

				//details->myColor = currentMaterial->Shade(*details, m_scenePtr->m_sceneLight, m_tracerPtr, colorAtPoint, NULL, false) * colorAtPoint;
				//details->myColor = currentMaterial->Shade(*details, m_scenePtr->m_sceneLight, m_tracerPtr, colorAtPoint, NULL, false);
				RGBColor energyArPoint = currentMaterial->Shade(*details, m_scenePtr->m_sceneLight, m_tracerPtr, colorAtPoint);
				details->myColor = energyArPoint * colorAtPoint;
			}
			else
			{
				details->ray = Ray(currentRay);
				details->myColor = RGBColor(0);
			}
		}

		bool CheckForUpdates()
		{
			return m_localUpdates.size() > 0;
		}

		vector<RadiancePoint*> GetLocalUpdates()
		{
			mtx.lock();
			vector<RadiancePoint*> temp = m_localUpdates;
			EmptyLocalUpdates();
			mtx.unlock();

			return temp;
		}

		void EmptyLocalUpdates()
		{
			m_localUpdates.clear();
#ifdef _DEBUG
			assert(m_localUpdates.size() == 0);
#endif
		}

		#ifndef MULTI_THREADED
			Octree* GetOctree()
			{
				return octreeStructure;
			}
		#else
			#ifdef WAIT_FREE
				WAIT_FREE_TYPE_OCTREE* GetOctree()
				{
					return octreeStructure;
				}
			#else
				LockingOctree* GetOctree()
				{
					return octreeStructure;
				}
			#endif
		#endif


		int GetLocalRadianceCount()
		{
			return m_localRadianceCount;
		}

		int GetNetworkRadianceCount()
		{
			return std::atomic_load(&m_networkRadianceCount);
		}

		void DontCreateSamples()
		{
			m_TestFlagCreateSamples = false;
		}

		void CreateSamples()
		{
			m_TestFlagCreateSamples = true;
		}

		float GetRadiancePointWeight(RadiancePoint* p_currentRadiancePoint, Point3D p_position, Point3D p_normal)
		{
			if(m_AlphaConstant == 0)
				return 0;
			else if (p_currentRadiancePoint)
			{
				float length = (p_currentRadiancePoint->m_position - p_position).length();
				float demA = (length / p_currentRadiancePoint->m_rayLength);
				float demB = (sqrt(1 - DOT(p_currentRadiancePoint->hitPointNormal, p_normal)));
				float denominator = 1 / max((demA + demB), 0.05f);
				return (denominator)-(1 / m_AlphaConstant);
			}
			else
			{
				return -5;
			}
		}

		using Integrator::InvalidateCache;
		virtual void InvalidateCache()
		{
			octreeStructure->InvalidateOctTree();
		}

		virtual void PartialInvalidate(BoundingBox* p_boundingBox)
		{
			octreeStructure->PartiallyInvalidateOctree(p_boundingBox);
		}


		~IrradianceCache()
		{}

	private:
		/// <summary>	The caching structure. </summary>
		#ifndef MULTI_THREADED
			Octree* octreeStructure;
		#else
			#ifdef WAIT_FREE
				WAIT_FREE_TYPE_OCTREE* octreeStructure;
			#else
				LockingOctree *octreeStructure;
			#endif
		#endif
			/// <summary>	The scene. </summary>
		Scene* m_scenePtr;
		/// <summary>	The acceleration structure. </summary>
		AccelerationStructure* accelerationStructure;
		/// <summary>	The sampler </summary>
		Sampler* m_sampler;
		/// <summary>	The tracer. </summary>
		Tracer* m_tracerPtr;
		/// <summary>	The accepted minimum ray length</summary>
		double m_minimumRayLength;
		/// <summary>	The current shade record. </summary>
		ShadeRec& m_currentShadeRec;
		/// <summary>	The local updates. </summary>
		vector<RadiancePoint*> m_localUpdates;

		std::mutex mtx;
		/// <summary>	true to test flag dont create. </summary>
		bool m_TestFlagCreateSamples = true;

		int m_localRadianceCount = 0;
		std::atomic<int> m_networkRadianceCount;

		float m_AlphaConstant;
		float m_amountBounces;
		
		int m_samplerType = 0;
		float m_cosineWeighting = 0;
		int m_numSamples;
		int m_numSets;
	

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetMinimumRayLength
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void GetMinmumRayLength()
		{
			double rayLength = accelerationStructure->m_boundingBox->GetLongestLengthAxis();
			m_minimumRayLength = rayLength / AMOUNT_DEVISABLE_CONSTANT;
			//m_minimumRayLength = 10.0f;
		}
	protected:
	};
	//----------------------------------------------------------------------------------------------------------------

}
//----------------------------------------------------------------------------------------------------------------