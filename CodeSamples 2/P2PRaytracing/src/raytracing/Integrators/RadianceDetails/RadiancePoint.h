#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		RadiancePoint.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <string>
#include <math.h>

#include "..\..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\..\CoreClasses\RGBObject\RGBColor.h"
#include "..\..\CoreClasses\Normal\Normal.h"
#include "..\..\CoreClasses\Ray\Ray.h"
#include "../../CoreClasses/UtilClasses/Common.h"
#include "../../CollisionDetails/ShadeRec.h"

enum DISTANCE_SURFACES
{
	HARMONIC_MEAN,
	MIN_RAY_LENGH
};

using std::string;

namespace Engine
{
	const int RADIANCE_POINT_SIZE = 40;
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: RadiancePoint
	//		   Description: Indirect lighting light details for a single point.
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class RadiancePoint
	{
	public:
		RGBColor radiance;
		Point3D m_position;
		float m_rayLength;
		Point3D hitPointNormal;
		Point3D m_shadingNormal;
		Ray m_collisionRay;
		int m_usedCounter = 0;
		bool m_alreadyChecked = false;

		RadiancePoint()
		{}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RadiancePoint(int p_acquiredFrom)
		{
			m_position = Point3D(0);
			hitPointNormal = Point3D();
			m_rayLength = 0;
			radiance = RGBColor(0);
			m_type = 0;
			m_acquiredFrom = p_acquiredFrom;
		}

		RadiancePoint(Point3D p_position, Point3D p_normal, float p_minRayLength, RGBColor p_radianceValue)
		{
			m_position = p_position;
			hitPointNormal = p_normal;
			m_rayLength = p_minRayLength;
			radiance = p_radianceValue;
			m_acquiredFrom = 0;
		}

		RadiancePoint(const RadiancePoint& p_radiancePoint)
		{
			m_position = p_radiancePoint.m_position;
			hitPointNormal = p_radiancePoint.hitPointNormal;
			m_rayLength = p_radiancePoint.m_rayLength;
			radiance = p_radiancePoint.radiance;
			m_acquiredFrom = p_radiancePoint.m_acquiredFrom;
			m_alreadyChecked = p_radiancePoint.m_alreadyChecked;
			m_shadingNormal = p_radiancePoint.m_shadingNormal;
			m_position = p_radiancePoint.m_position;
			m_usedCounter = p_radiancePoint.m_usedCounter;
		}


		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Radiance Point position, Normal, Ray, Name
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RadiancePoint(Point3D p_position, Point3D p_normal, Ray p_collsionRay = Ray())
		{
			m_position = p_position;
			hitPointNormal = p_normal;
			m_collisionRay = p_collsionRay;
			m_acquiredFrom = 0;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Position, RayLength
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RadiancePoint(Point3D p_position, float p_rayLength = INT_MAX)
		{
			m_position = p_position;
			m_rayLength = p_rayLength;
			m_acquiredFrom = 0;
		}
		
		RadiancePoint(string p_input)
		{
			m_acquiredFrom = 1;
			FromString(p_input);
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetEpsillonHitPoint
		//		Return Value: Point3D
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D GetEpsillonHitPoint(void)
		{
			return m_position + (Common::kEpsilon * hitPointNormal);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetWeighting
		//		Return Value: float
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		//Get Weigth : Calculate the weight of  the currnet Point with regards another point
		float GetWeighting(Point3D p_position, Point3D p_normal, float p_alphaConstant)
		{
			float length = (m_position - p_position).length();
			float demA = (length/ m_rayLength);
			//float demA = (length / 10);

			float demB = (sqrt(1 - DOT(hitPointNormal, p_normal)));
			float denominator =1 / max((demA + demB), 0.05f);
			//float denominator =1 / max((demA), 0.05f);
			return (denominator) - (1 / p_alphaConstant);
			//return 0.1;
			//return denominator;
		}

		RadiancePoint* operator = (RadiancePoint* p_rhs)
		{
			return p_rhs;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ComputeRadiance
		//		Return Value: VOID
		//		  Parameters: vector<ShadRec>&
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void ComputeRadiance(vector<ShadeRec>& radianceSamples)
		{
			for (int sampleIndex = 0; sampleIndex < radianceSamples.size(); sampleIndex++)
			{
				//RGBColor currentRadiance = radianceSamples[sampleIndex].
				radiance += radianceSamples[sampleIndex].myColor;
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ComputeRadianceNHarmonivValue
		//		Return Value: VOID
		//		  Parameters: vector<ShadeRec*>&, minRayLengths
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void ComputeRadianceNHarmonivValue(vector<ShadeRec*>& p_ShadeRecList, float p_minRayLengths)
		{
			double invRaySummation = 0.0f;

#define __max max 
#undef max 
			double minRayLength = std::numeric_limits<double>::max();
#define max __max

			int sampleCount = 0;
			radiance = RGBColor(0);

			for (int sampleIndex = 0; sampleIndex < p_ShadeRecList.size(); sampleIndex++)
			{
				if (p_ShadeRecList[sampleIndex]->hitAnObject)
				{
					double rayVectorLength = (p_ShadeRecList[sampleIndex]->localHitPoint - m_position).length();

					minRayLength = min(rayVectorLength, minRayLength);

					RGBColor a = p_ShadeRecList[sampleIndex]->myColor;

					//if(a.r < 1 && a.g < 1 && a.b < 1)
					{
						radiance = radiance + p_ShadeRecList[sampleIndex]->myColor;
						sampleCount++;
					}
				}
			}
	
			m_rayLength = std::max<float>(std::max<float>(minRayLength, p_minRayLengths), kEpsilon);
			
			//average
			if (sampleCount > 0)
			{
				radiance = radiance / sampleCount;
			}
		}

		void SetChecked()
		{
			m_alreadyChecked = true;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Convert this object into a string representation. </summary>
		///
		/// <remarks>	Adrian, 17/09/2015. </remarks>
		///
		/// <returns>	A string that represents this object. </returns>
		///-------------------------------------------------------------------------------------------------

		string ToString() const
		{
			return radiance.ToString() + "^" + m_position.toString() + "^" + to_string(m_rayLength) + "^" + hitPointNormal.toString();
		}


		void FromString(string p_stringInput)
		{
			string data[4];
			Common::Tokenizer::ParseString(p_stringInput, "^", data);

			m_acquiredFrom = 1;

			radiance.FromString(data[0]);
			m_position.FromString(data[1]);
			m_rayLength = stof(data[2]);
			hitPointNormal.FromString(data[3]);
		}

		void SerializePoint(unsigned char p_Array[], int p_startingPosition)
		{
			int incPos = p_startingPosition;

			radiance.SerializePoint(p_Array, incPos);
			incPos += 12;
			m_position.SerializePoint(p_Array, incPos);
			incPos += 12;
			ConvertionClass::ConvertFloat(m_rayLength, incPos, p_Array);
			incPos += 4;
			hitPointNormal.SerializePoint(p_Array, incPos);
		}

		void SerializePoint(vector<unsigned char>& p_Array)
		{
			radiance.SerializePoint(p_Array);
			m_position.SerializePoint(p_Array);
			ConvertionClass::ConvertFloat(m_rayLength, p_Array);
			hitPointNormal.SerializePoint(p_Array);
		}

		void FromSerialize(char* p_charArray)
		{
			vector<unsigned char> sampleDetails(RADIANCE_POINT_SIZE);
			for (int index = 0; index < RADIANCE_POINT_SIZE; index++)
			{
				sampleDetails[index] = (unsigned char)p_charArray[index];
			}

			FromSerialize(sampleDetails);
		}


		void FromSerialize(vector<unsigned char>& p_charArray, int &p_startingPoint)
		{
			radiance.FromSerialize(p_charArray, p_startingPoint);
			m_position.FromSerialize(p_charArray,p_startingPoint);
			ConvertionClass::ByteArrayToFloat(m_rayLength, p_charArray, p_startingPoint, 4);
			hitPointNormal.FromSerialize(p_charArray, p_startingPoint);

			/*if (radiance.r > 1 || radiance.r < 0 || radiance.g > 1 || radiance.g < 0 || radiance.b > 1 || radiance.b < 0)
			{
				cout << "problem here " << endl;
			}*/

			m_acquiredFrom = 1;
		}

		bool FromSerialize(vector<unsigned char>& p_charArray)
		{
			int startingPoint = 0;
			radiance.FromSerialize(p_charArray, startingPoint);
			m_position.FromSerialize(p_charArray, startingPoint);
			ConvertionClass::ByteArrayToFloat(m_rayLength, p_charArray, startingPoint, 4);
			hitPointNormal.FromSerialize(p_charArray, startingPoint);
			m_acquiredFrom = 1;
			if (radiance.r > 1 || radiance.r < 0 || radiance.g > 1|| radiance.g < 0 || radiance.b > 1 || radiance.b < 0)
			{
				return false;
			}

			return true;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CheckIfValidToPoint
		//		Return Value: bool
		//		  Parameters: RaidancePoint*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool CheckIfValidToPoint(RadiancePoint* currentSample)
		{
			return false;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetHarmonicValue
		//		Return Value: VOID
		//		  Parameters: vector<ShadeRec>&
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void GetHarmonicValue(vector<ShadeRec>& p_ShadeRecList)
		{
			double invRaySummation = 0.0f;

			for (int sampleIndex = 0; sampleIndex < p_ShadeRecList.size(); sampleIndex++)
			{
				if (p_ShadeRecList[sampleIndex].hitAnObject)
				{
					double rayVectorLength = (p_ShadeRecList[sampleIndex].hitPoint - m_position).length();

					invRaySummation += (1 / rayVectorLength);
				}
			}

			float rayLength = p_ShadeRecList.size() / invRaySummation;

			m_rayLength = rayLength;
		}

		static float DistanceBetweenTwoSamples(RadiancePoint* p_lhs, RadiancePoint* p_rhs)
		{
			Point3D lhsPos = p_lhs->m_position;
			Point3D rhsPos = p_rhs->m_position;
			return std::sqrtf(std::powf(rhsPos.x- lhsPos.x, 2) + std::powf(rhsPos.y - lhsPos.y, 2) + std::powf(rhsPos.z - lhsPos.z, 2));
		}


		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: == operator overload for comparision with another instance of the same class 
		//		Return Value: VOID
		//		  Parameters: RaidnacePoint
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		inline bool operator == (RadiancePoint* p_radiancePoint)
		{
			return m_position == p_radiancePoint->m_position;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: == operatoor overload for with point3D class
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		inline bool operator == (Point3D p_position)
		{
			return m_position == p_position;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetType
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		int GetType()
		{
			return m_type;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GotFrom
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		int GotFrom()
		{
			return m_acquiredFrom;
		}


		void SetRadiance(RGBColor p_radiance)
		{
			radiance = p_radiance;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: DESTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		~RadiancePoint()
		{}

	private:
		int m_type = 0;
		int m_acquiredFrom = 0;

		/// <summary>	Size of the byte. </summary>
		int m_ByteSize = 55;
	};


	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------

