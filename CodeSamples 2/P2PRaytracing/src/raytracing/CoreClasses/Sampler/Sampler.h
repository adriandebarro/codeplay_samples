#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Sampler.h
//			  Author:		Adrian De Barro
//			    Date:		13/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <vector>
#include <math.h>
#include <random>


#include "..\Point\Point2D\Point2D.h"
#include "..\Point\Point3D\Point3D.h"
#include "..\UtilClasses\Common.h"
#include "..\Sampler\RandomNumberGenerator\PawluLostja.h"
#include "..\Matrix\Matrix3.h"
#include "..\Basis\OrthonormalBasis.h"
#include "../Debugging/MatlabUtilityDebuger.h"

using std::vector;
using std::swap;
using Engine::Debugging::MatlabUtilityDebuger;
//#define NO_ROTATION
namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Sampler
	//		   Description: Parent CLass for all type of samplers
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class Sampler
	{
	public:
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Sampler() : numSamples(0), numSets(0), count(0), jump(0)
		{}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Number of Samples, Number of sets, count, jump
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Sampler(int _numSamples, int _numSets, float p_cosineWeighting = 0.f , int p_samplingType = 0, int _count = 0, int _jump = 0) : m_cosineWeighting(p_cosineWeighting)
		{
			int testNumSamples = GetSquaredNumber(_numSamples);
			numSamples = testNumSamples * testNumSamples;
			m_samplingType = p_samplingType;
			numSets = _numSets;
			count = _count;
			jump = _jump;
			m_needRotation = true;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GenerateSamples
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual void GenerateSamples() = 0;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetupShuffledIndices
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetupShuffledIndices(void);
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ShuffleSampled
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void ShuffleSampled(void);
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SampleUnitSquare
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point2D SampleUnitSquare(void);
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetNumSamples
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		int GetNumSamples(void);
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: MapSamplesToUnitDisk
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void MapSamplesToUnitDisk(void);
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: MapSamplesToHemisphere
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void MapSamplesToHemisphere(float, bool);
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SampleHemispherical
		//		Return Value: Point3D
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D SampleHemispherical();
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SampleHemispherical
		//		Return Value: VOID
		//		  Parameters: int
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D SampleHemispherical(int);
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SampleHemispherical
		//		Return Value: VOID
		//		  Parameters: int, float, float
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D SampleHemispherical(int, float, float);
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SampleHemispherical
		//		Return Value: VOID
		//		  Parameters: int, Point3D, Point3D
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D SampleHemispherical(int,Point3D, Point3D, Point3D&, Point3D&, Twister& p_twisterInstance);

		Point3D SampleHemispherical(int, Point3D, Point3D, Point3D&, Point3D&, Twister& p_twisterInstance, vector<Point3D>& p_sampleVector);
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ConvertSamplesToHemisphere
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void ConvertSamplesToHemisphere(void);
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ConvertSamplesToHemisphere
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		int GetSamplesPerPacket()
		{
			return numSamples;
		}
	protected:
		//number of sample points 
		int numSamples;
		//number of sample sets
		int numSets;
		//sample points in a unit square
		vector<Point2D> samples;
		//shuffled samples array indices
		vector<int> shuffledIndices;
		//the current number of sample points
		unsigned long count;
		vector<Point2D> diskSamples;
		vector<Point3D> hemisphericalSamples;
		int jump;
		Matrix3 rotationMatrix;
		bool m_needRotation;
		Engine::Twister twister;
		OrthonomalBasis m_arbitraryAxis;
		int m_samplingType = 0;
		float m_cosineWeighting = 0;

	};
	//----------------------------------------------------------------------------------------------------------------
	void Engine::Sampler::ShuffleSampled()
	{
		iterator_traits<vector<Point2D>::iterator>::difference_type i, n;
		n = (samples.end() - samples.begin());

		for (i = n - 1; i > 0; -- i)
		{
			swap(samples.begin()[i], samples.begin()[i + 1]);
		}
	}
	//----------------------------------------------------------------------------------------------------------------
	Point2D Engine::Sampler::SampleUnitSquare()
	{
		//if (count % numSamples == 0)
		//	jump = (GenerateRandomInt() % numSets) * numSamples;

		//return (samples[jump + shuffledIndices[jump + count++ % numSamples]]);
		int randomNumSets = rand() % numSets;
		int pickedLocation = randomNumSets * numSets + count;
		
		if (++count == numSamples)
		{
			count = 0;
		}
		return samples[pickedLocation];
	}
	//----------------------------------------------------------------------------------------------------------------
	Point3D Engine::Sampler::SampleHemispherical()
	{
		int randomNumSets = rand() % numSets;
		int pickedLocation = randomNumSets * numSamples + count;
		if (++count == numSamples)
		{
			count = 0;
		}
		return hemisphericalSamples[pickedLocation];
	}
	//----------------------------------------------------------------------------------------------------------------
	Point3D Engine::Sampler::SampleHemispherical(int p_currentIndex)
	{		
		int randomNumSets = rand() % numSets;
		int pickedLocation = randomNumSets * numSamples + p_currentIndex;
		return hemisphericalSamples[pickedLocation];
	}
	//----------------------------------------------------------------------------------------------------------------
	/*
		naive implementation 
	*/
	Point3D Engine::Sampler::SampleHemispherical(int p_currentIndex, Point3D p_normal, Point3D p_dir, Point3D& p_arbitraryAxis_X, Point3D& p_arbitraryAxis_Z, Twister& p_twisterInstance)
	{
		Point3D currentNormal(0, 0, 1);
		if (p_currentIndex == 0)
		{
			p_arbitraryAxis_X = p_normal.CrossProduct(currentNormal);
			p_arbitraryAxis_X.Normalize();

			p_arbitraryAxis_Z = p_arbitraryAxis_X.CrossProduct(p_normal);
			p_arbitraryAxis_Z.Normalize();

			if ((p_arbitraryAxis_X.x == 0 || std::isnan(p_arbitraryAxis_X.x)) && (p_arbitraryAxis_X.y == 0 || std::isnan(p_arbitraryAxis_X.y)) && (p_arbitraryAxis_X.z == 0 || std::isnan(p_arbitraryAxis_X.z)) && p_normal.z > 0)
			{
				p_arbitraryAxis_X = Point3D(1, 0, 0);
				p_arbitraryAxis_Z = Point3D(0, 1, 0);
			}
			else if ((p_arbitraryAxis_X.x == 0 || std::isnan(p_arbitraryAxis_X.x)) && (p_arbitraryAxis_X.y == 0 || std::isnan(p_arbitraryAxis_X.y)) && (p_arbitraryAxis_X.z == 0 || std::isnan(p_arbitraryAxis_X.z)) && p_normal.z < 0)
			{
				p_arbitraryAxis_X = Point3D(-1, 0, 0);
				p_arbitraryAxis_Z = Point3D(0, -1, 0);
			}

			_ASSERT(p_arbitraryAxis_X.x != 0 || p_arbitraryAxis_X.y != 0 || p_arbitraryAxis_X.z != 0);
			_ASSERT(p_arbitraryAxis_Z.x != 0 || p_arbitraryAxis_Z.y != 0 || p_arbitraryAxis_Z.z != 0);


			/*Point3D W = p_w.normalize();
			Point3D V, U;
			if (std::fabsf(W.x) > std::fabsf(W.y)) {
			float invLen = 1.f / std::sqrtf(W.x*W.x + W.z*W.z);
			V = Point3D(-W.z * invLen, 0.f, W.x * invLen);
			}
			else {
			float invLen = 1.f / std::sqrt(W.y*W.y + W.z*W.z);
			V = Point3D(0.f, W.z * invLen, -W.z * invLen);
			}

			U = W.CrossProduct(V);

			p_arbitraryAxis_X = U;
			p_arbitraryAxis_Z = V;*/
		}

		Point3D selectedPoint(0);
		Point3D outputPoint(0);
		//jittered
		if (m_samplingType == 0)
		{
			int randomNumSets = rand() % numSets;
			int pickedLocation = randomNumSets * numSamples + p_currentIndex;
			selectedPoint = hemisphericalSamples[pickedLocation];
		}
		else
		{
			int randomNumSets = rand() % numSets;
			int randomSample = rand() % numSamples;
			int pickedLocation = randomNumSets * numSamples + p_currentIndex;
			selectedPoint = hemisphericalSamples[pickedLocation];
		}

		//outputPoint = Point3D(selectedPoint.dot(p_arbitraryAxis_X), selectedPoint.dot(p_arbitraryAxis_Z), selectedPoint.dot(p_w));
		outputPoint = selectedPoint.x * p_arbitraryAxis_X + selectedPoint.y *p_arbitraryAxis_Z + selectedPoint.z * p_normal;

		outputPoint.Normalize();

		_ASSERT(outputPoint.x != 0 || outputPoint.y != 0 || outputPoint.z != 0);
		return outputPoint;
	}

	Point3D Engine::Sampler::SampleHemispherical(int p_currentIndex, Point3D p_normal, Point3D p_dir, Point3D& p_arbitraryAxis_X, Point3D& p_arbitraryAxis_Z, Twister& p_twisterInstance, vector<Point3D>& p_samplesVector)
	{
#ifndef NO_ROTATION
		Point3D currentNormal(0, 0, 1);

		if (p_currentIndex == 0)
		{
			p_arbitraryAxis_X = p_normal.CrossProduct(currentNormal);
			p_arbitraryAxis_X.Normalize();

			p_arbitraryAxis_Z = p_arbitraryAxis_X.CrossProduct(p_normal);
			p_arbitraryAxis_Z.Normalize();

			if ((p_arbitraryAxis_X.x == 0 || std::isnan(p_arbitraryAxis_X.x)) && (p_arbitraryAxis_X.y == 0 || std::isnan(p_arbitraryAxis_X.y)) && (p_arbitraryAxis_X.z == 0 || std::isnan(p_arbitraryAxis_X.z)) && p_normal.z > 0)
			{
				p_arbitraryAxis_X = Point3D(1, 0, 0);
				p_arbitraryAxis_Z = Point3D(0, 1, 0);
			}
			else if ((p_arbitraryAxis_X.x == 0 || std::isnan(p_arbitraryAxis_X.x)) && (p_arbitraryAxis_X.y == 0 || std::isnan(p_arbitraryAxis_X.y)) && (p_arbitraryAxis_X.z == 0 || std::isnan(p_arbitraryAxis_X.z)) && p_normal.z < 0)
			{
				p_arbitraryAxis_X = Point3D(-1, 0, 0);
				p_arbitraryAxis_Z = Point3D(0, -1, 0);
			}

			_ASSERT(p_arbitraryAxis_X.x != 0 || p_arbitraryAxis_X.y != 0 || p_arbitraryAxis_X.z != 0);
			_ASSERT(p_arbitraryAxis_Z.x != 0 || p_arbitraryAxis_Z.y != 0 || p_arbitraryAxis_Z.z != 0);


			/*Point3D W = p_w.normalize();
			Point3D V, U;
			if (std::fabsf(W.x) > std::fabsf(W.y)) {
				float invLen = 1.f / std::sqrtf(W.x*W.x + W.z*W.z);
				V = Point3D(-W.z * invLen, 0.f, W.x * invLen);
			}
			else {
				float invLen = 1.f / std::sqrt(W.y*W.y + W.z*W.z);
				V = Point3D(0.f, W.z * invLen, -W.z * invLen);
			}

			U = W.CrossProduct(V);
		
			p_arbitraryAxis_X = U;
			p_arbitraryAxis_Z = V;*/
		}

		Point3D selectedPoint(0);
		Point3D outputPoint(0);

		//jittered
		if (m_samplingType == 0)
		{
			int randomNumSets = rand() % numSets;
			int pickedLocation = randomNumSets * numSamples + p_currentIndex;
			selectedPoint = hemisphericalSamples[pickedLocation];
		}
		else
		{
			int randomNumSets = rand() % numSets;
			int randomSample = rand() % numSamples;
			int pickedLocation = randomNumSets * numSamples + p_currentIndex;
			selectedPoint = hemisphericalSamples[pickedLocation];
		}

		p_samplesVector.push_back(selectedPoint);
		
		//outputPoint = Point3D(selectedPoint.dot(p_arbitraryAxis_X), selectedPoint.dot(p_arbitraryAxis_Z), selectedPoint.dot(p_w));
		outputPoint = selectedPoint.x * p_arbitraryAxis_X + selectedPoint.y *p_arbitraryAxis_Z + selectedPoint.z * p_normal;

		outputPoint.Normalize();

		_ASSERT(outputPoint.x != 0 || outputPoint.y != 0 || outputPoint.z != 0);
		return outputPoint;

#else
		// nl = n.dot(r.d)<0 ? n : n*-1

		//assuming the current principle axis is the z axis
		Point3D currentNormal(0, 0, 1);
		int randomNumSets = rand() % numSets;
		int pickedLocation = randomNumSets * numSamples + p_currentIndex;
		Point2D selectedPoint = samples[pickedLocation];

		float r1 = selectedPoint.x, r2 = 90 - selectedPoint.y, rs2 = sqrt(r2);

		Point3D w = p_normal;
		Point3D u = (currentNormal).CrossProduct(w).normalize();
		Point3D v = w.CrossProduct(u);

		float sinTheta = sin(r2);
		float cosPhi = cos(r1);
		float cosTheta = cos(r2);
		float sinPhi = sin(r1);

		Point3D output = (((sinTheta*cosPhi) * u) + (sinPhi*sinTheta * v) + cosTheta * w).normalize();
		return output;

#endif
	}


	//----------------------------------------------------------------------------------------------------------------
	Point3D Engine::Sampler::SampleHemispherical(int p_currentIndex, float xRand, float yRand)
	{
		int randomNumSets = rand() % numSets;
		int pickedLocation = randomNumSets * numSamples + p_currentIndex;

		Point2D currentSample = samples[pickedLocation];
		xRand = currentSample.x;
		yRand = currentSample.y;
		return hemisphericalSamples[pickedLocation];

	}
	//----------------------------------------------------------------------------------------------------------------
	void Engine::Sampler::SetupShuffledIndices()
	{
		shuffledIndices.reserve(numSamples * numSets);
		vector<int>  indices;

		for (int j = 0; j < numSamples; j++)
			indices.push_back(j);

		for (int p = 0; p < numSets; p++)
		{
			//random_shuffle(indices.begin(), indices.end());

			for (int j = 0; j < numSamples; j++)
				shuffledIndices.push_back(indices[j]);
		}
	
	}
	//----------------------------------------------------------------------------------------------------------------
	int Engine::Sampler::GetNumSamples()
	{
		return ((int)samples.size());
	}
	//----------------------------------------------------------------------------------------------------------------
	void Engine::Sampler::MapSamplesToUnitDisk()
	{
		int size = samples.size();
		float r, phi;
		Point2D sp;
		//will be to have a size which is at least 'size' sixe loarge
		diskSamples.reserve(size);


		for (int j = 0; j < size; j ++)
		{
			sp.x = 2.0 * samples[j].x - 1.0;
			sp.y = 2.0 * samples[j].y - 1.0;

			if (sp.x > -sp.y) //sector 1 and sector 2
			{
				if (sp.x > sp.y)
				{
					r = sp.x;
					phi = sp.y / sp.x;
				}
				else
				{
					r = sp.y;
					phi = sp.y / sp.x;
				}
			}
			else
			{
				if (sp.x < sp.y)
				{
					r = -sp.x;
					phi = 4 + (sp.y / sp.x);
				}
				else
				{
					r = -sp.y;
					phi = 6 - (sp.x / sp.y);
				}
			}

			phi *= PI_NUM / 4.0f;
			diskSamples.push_back(Point2D(r * cos(phi), r * sin(phi)));
		}
	}
	//----------------------------------------------------------------------------------------------------------------
	//mapping to the normal 0,1,0
	void Sampler::MapSamplesToHemisphere(float e, bool p_needRotation)
	{
		int size = samples.size();
		hemisphericalSamples.reserve(numSamples * numSets);
		m_needRotation = p_needRotation;
			
		for (int j = 0; j < size; j++)
		{
			float cosPhi = cos(2.0 * PI_NUM * diskSamples[j].x);
			float sinPhi = sin(2.0 * PI_NUM * diskSamples[j].y);
			float cosTheta = pow((1.0 - samples[j].y), 1.0/ (e + 1.0));
			float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

			float pu = sinTheta * cosPhi;
			float pv = sinTheta * sinPhi;
			float pw = cosTheta;

			Point3D testPoint(pu, pv, pw);
			if (testPoint.TestPointData())
			{
				cout << "error" << endl;
			}
			hemisphericalSamples.push_back(testPoint);
		}
		//Engine::Debugging::MatlabUtilityDebuger::VisualizeHemisphericalSamplesSamples(hemisphericalSamples, numSamples, numSets);
	}
	//----------------------------------------------------------------------------------------------------------------
	void Sampler::ConvertSamplesToHemisphere(void)
	{
		MapSamplesToHemisphere(m_cosineWeighting, false);
	}
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------
