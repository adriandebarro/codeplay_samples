#pragma once 

#include <vector>

#include "../../CoreClasses/Point/Point3D/Point3D.h"

namespace Engine
{
	namespace Networking
	{
		namespace Neighbours
		{
			struct State
			{
				/// <summary>	The peer position. </summary>
				Point3D m_peerPosition;
				float m_peerLocalityWeight;
				float m_localityImportanceWeight;

				/// <summary>	The bandwidth. </summary>
				float m_bandwidth;
				float m_bandwidthWeight;
				float m_bandwidthImportanceWeight;

				/// <summary>	The CPU speed. </summary>
				float m_cpuSpeed;
				float m_cpuSpeedWeight;
				float m_cpuSpeedImportance;

				/// <summary>	The frame rate. </summary>
				float m_frameRate;
				float m_weightFrameRate;
				
				/// <summary>	The viewing direction. </summary>
				Point3D m_viewingDirection;
				float m_viewPointWeight;
				float m_viewPointImportanceWeight;

				/// <summary>	The latency. </summary>
				int m_latency;
				float m_latencyWeight;

				/// <summary>	The bounds position. </summary>
				int m_boundsPosition;
				float m_boundsWeight;


				float m_peerWeight;

			public:

				void UpdateState(int p_contextIndex, std::vector<unsigned char>& p_byteVector, int& p_startingPosition)
				{
					switch (p_contextIndex)
					{
						//position
						case 0:
						{
							m_peerPosition.FromSerialize(p_byteVector, p_startingPosition);
							break;
						}

					
						case 2:
						{
							m_viewingDirection = *(Point3D*)&p_byteVector[p_startingPosition];
							p_startingPosition += sizeof(Point3D);
							break;
						}

						//bounds position
						case 3:
						{
							m_boundsPosition = *(int*)&p_byteVector[p_startingPosition];
							p_startingPosition += 4;
							break;
						}

						case 4:
						{
							m_frameRate = *(float*)&p_byteVector[p_startingPosition];
							p_startingPosition += 4;
							break;
						}

						case 5:
						{
							m_latency = *(int*)& p_byteVector[p_startingPosition];
							p_startingPosition += 4;
							break;
						}

						default:
							break;
					}
				}

				void ComputeWeighting()
				{
					m_peerWeight =  m_localityImportanceWeight * m_peerLocalityWeight + m_viewPointImportanceWeight * m_viewPointWeight;
				}

				void ComputeWeighting(float p_localityWeight, float p_viewPointWeight, float p_boundsWeight, float p_latencyWeight,  float p_frameRate )
				{
					m_peerWeight = p_localityWeight * m_peerLocalityWeight + p_viewPointWeight * m_viewPointWeight + p_boundsWeight * m_boundsWeight + p_latencyWeight * m_latencyWeight + p_frameRate * m_weightFrameRate;
				}

				float GetWeight()
				{
					return m_peerWeight;
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Sets a weighting. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_weighting">	The weighting. </param>
				/// <param name="context">	  	The context. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////

				void SetWeighting(float p_weighting, int p_context)
				{
					switch (p_context)
					{
						case 0:
						{
							m_peerLocalityWeight = p_weighting;
							break;
						}

						case 2:
						{
							m_viewPointWeight = p_weighting;
							break;
						}

						case 3:
						{
							m_boundsWeight = p_weighting;
							break;
						}

						case 4:
						{
							m_weightFrameRate = p_weighting;
							break;
						}

						case 5:
						{
							m_latencyWeight = p_weighting;
							break;
						}
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Constructor. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_localityImportanceWeight"> 	The locality importance weight. </param>
				/// <param name="p_viewPointImportanceWeight">	The view point importance weight. </param>
				/// <param name="p_bandiwidthImportance">	  	The bandiwidth importance. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////

				State()
				{
					m_peerPosition = Point3D(0);
					m_frameRate = -1;
					m_viewingDirection = Point3D(0);
					m_latency = -1;
					m_boundsPosition = -1;

					m_peerLocalityWeight = 0;
					m_weightFrameRate = 0;
					m_viewPointWeight = 0;
					m_latencyWeight = 0;
					m_boundsWeight = 0;

					m_peerWeight = 0;
				}
			};
		}
	}
}