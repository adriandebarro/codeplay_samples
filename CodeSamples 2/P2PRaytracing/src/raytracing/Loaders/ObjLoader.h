#pragma once 
//----------------------------------------------------------------------------------------------------------------
//			Filename:		ObjLoader.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <random>
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <boost\tokenizer.hpp>
#include <time.h>
#include <stdio.h>
#include <map>

#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "..\Material\Material.h"
#include "..\Material\Matte.h"
#include "..\CoreClasses\Logger\Logger.h"
#include "..\CoreClasses\GeometricObjects\Quad.h"
#include "..\Material\Material.h"
#include "..\CoreClasses\GeometricObjects\Triangle.h"
#include "..\Material\Glossy.h"
#include "..\Material\Reflective.h"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using Engine::Primitives::Triangle;
using Engine::TriangleVertex;
using std::vector;

typedef boost::tokenizer<boost::char_separator<char>> tokeniz;

using Engine::Materials::Material;
using Engine::Materials::Matte;
using Engine::Materials::Glossy;
using Engine::Materials::Reflective;

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: ObjLoader
	//		   Description: 
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class ObjLoader
	{
	public:
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: LoadFile
		//		Return Value: vector<GeometricObjects*>
		//		  Parameters: string
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		vector<GeometricObjects*> LoadFile(string path)
		{
			bool numberPaths = path.find(";");

			vector<GeometricObjects*> allObjects;

			//if more than one path
			if (numberPaths)
			{
				vector<string> allPaths;
				//get the paths

				for (int index = 0; index < allPaths.size(); index++)
				{
					ParseObjFile(allPaths[index], allObjects);
				}

				return allObjects;

			}
			else
			{
				ParseObjFile(path, allObjects);
				return allObjects;
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: LoadFile
		//		Return Value: vector<GeometricObjects*>
		//		  Parameters: string path, Point3D centroid, bool Obj uses textures
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		vector<GeometricObjects*> LoadFile(string path, Point3D centroid, bool p_useTextures = false)
		{
			bool numberPaths = path.find(";");
			
			vector<GeometricObjects*> allObjects;

			//if more than one path
			if(numberPaths)
			{
				vector<string> allPaths;

				ParseValues(path, ";", allPaths);

				for (int index = 0; index < allPaths.size(); index++)
				{
					if(allPaths[index] != "")
						ParseObjFile(allPaths[index], allObjects , centroid, p_useTextures);
				}

				return allObjects;
			
			}
			else
			{
				ParseObjFile(path, allObjects, centroid, p_useTextures);
				return allObjects;
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ParseStringSentence
		//		Return Value: VOID
		//		  Parameters: string to parse, string splittingString, vector<string>& output list
		//			  Status: 
		//					  TODO: 
		//----------------------------------------------------------------------------------------------------------------
		inline void ParseStringSentence(std::string toParse, std::string splittingString, std::vector<std::string>& output)
		{
			boost::char_separator<char> sep(splittingString.c_str());
			tokeniz tok(toParse, sep);
			int index = 0;
			for (tokeniz::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
			{
				output.push_back(tokIter.current_token().c_str());
			}
		}

		inline void ParseStringSentence(std::string toParse, std::string splittingString, string* p_output)
		{
			boost::char_separator<char> sep(splittingString.c_str());
			tokeniz tok(toParse, sep);
			int index = 0;
			for (tokeniz::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
			{
				p_output[index] = tokIter.current_token().c_str();
				++index;
			}
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ParseString
		//		Return Value: vector<string>
		//		  Parameters: string toParse, string splittingString
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		vector<std::string> ParseString(std::string toParse, std::string splittingString)
		{
			boost::char_separator<char> sep(splittingString.c_str());
			tokeniz tok(toParse, sep);
			vector<string> output;

			int index = 0;
			for (tokeniz::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
			{
				output.push_back(tokIter.current_token().c_str());
			}

			return output;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ParseValues
		//		Return Value: VOID
		//		  Parameters: string toParse, string splitting string, vector<string>& output
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void ParseValues(std::string toParse, std::string splittingString, vector<string>& output)
		{
			boost::char_separator<char> sep(splittingString.c_str());
			tokeniz tok(toParse, sep);
			int amount = 0;
			for (tokeniz::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
			{
				amount++;
			}

			int index = 0;
			for (tokeniz::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
			{
				if ((index == 0) | (index > 1))
				{
					output.push_back(tokIter.current_token().c_str());
					if (amount == 1)
					{
						output.push_back("");
						output.push_back("");
					}
				}
				else
				{
					int size = output[0].size();
					char atSize = toParse.at(size);
					char atSize1 = toParse.at(size + 1);

					std::string check = std::string() + atSize + atSize1;

					if (std::strcmp(check.c_str(), "//") == 0)
					{
						output.push_back("");
						output.push_back(tokIter.current_token().c_str());
					}
					else if (amount == 2)
					{
						output.push_back(tokIter.current_token().c_str());
						output.push_back("");
					}
					else
					{
						output.push_back(tokIter.current_token().c_str());
					}

				}

				index++;
			}
		}
		//void GetMaterials(string path, map<string, Material*>& allMaterials, bool containsTextures = false)
		//{
		//	bool newfile = true;
		//	int index = 0;
		//	std::string detailsArray[2];
		//	bool firstTime = false;
		//	bool finishedFlag = false;
		//	//string fullPath = "C:\\Users\\Adrian\\Desktop\\" + path;
		//	//string fullPath = "\ObjFiles\\"+path;
		//	std::ifstream newFile(path);
		//	string line;
		//	if (newFile.is_open())
		//	{
		//		try
		//		{
		//			vector<string> tokens; // [KB] This is redefined below
		//			string materialName = "";
		//			RGBColor diffuseColor(0);
		//			//RGBColor specularColor();

		//			// [KB] Debugging stuff
		//			int linesParsed = 0;

		//			while (std::getline(newFile, line))
		//			{
		//				//++linesParsed; std::cout << "MTL Line : [" << linesParsed << " : " << line << "]" << std::endl;

		//				if (line.empty() || line.at(0) == '#')
		//					continue;

		//				//char firstChar = line.at(0);
		//				//vector<string> tokens; [KB] This has been defined above already! Use previous definition and clear (in case Parse doesn't)
		//				tokens.clear();

		//				ParseStringSentence(line, " ", tokens);

		//				if (tokens.size() > 0 && strcmp(tokens[0].c_str(), "newmtl") == 0)
		//				{
		//					if (!firstTime)
		//					{
		//						materialName = tokens[1];
		//						firstTime = true;
		//					}
		//					else
		//					{
		//						//first save then clear and store the new texture name

		//						bool solidColorExists = detailsArray[0].compare("") != 0;
		//						bool textureExists = detailsArray[1].compare("") != 0;

		//						Matte* currentMaterial;

		//						//only color
		//						if (solidColorExists && !textureExists)
		//						{
		//							vector<std::string> subTokens;

		//							ParseStringSentence(detailsArray[0], " ", subTokens);

		//							diffuseColor = RGBColor((float)atof(subTokens[0].c_str()), (float)atof(subTokens[1].c_str()), (float)atof(subTokens[2].c_str()));

		//							Matte* currentMaterial = new Matte(materialName, 0.75, diffuseColor);
		//							allMaterials[materialName] = currentMaterial;

		//						}
		//						else if (!solidColorExists && textureExists)
		//						{

		//							Matte* currentMaterial = new Matte(materialName, 0.75, detailsArray[1]);
		//							allMaterials[materialName] = currentMaterial;
		//						}
		//						else
		//						{
		//							vector<std::string> subTokens;

		//							ParseStringSentence(detailsArray[0], " ", subTokens);

		//							diffuseColor = RGBColor((float)atof(subTokens[0].c_str()), (float)atof(subTokens[1].c_str()), (float)atof(subTokens[2].c_str()));


		//							Matte* currentMaterial = new Matte(materialName, 0.75, detailsArray[1], diffuseColor);
		//							allMaterials[materialName] = currentMaterial;
		//						}

		//						detailsArray[0] = "";
		//						detailsArray[1] = "";

		//						materialName = tokens[1];
		//					}
		//				}
		//				else  if (tokens.size() > 0 && strcmp(tokens[0].c_str(), "Kd") == 0)
		//				{
		//					detailsArray[0] = tokens[1] + " " + tokens[2] + " " + tokens[3];
		//				}
		//				else if (tokens.size() > 0 && strcmp(tokens[0].c_str(), "map_Kd") == 0)
		//				{
		//					/*
		//					format is : map_Kd filename.ppm
		//					*/
		//					//name of file token [1]
		//					stringstream currentPath;

		//					vector<string> pathTokens;
		//					ParseStringSentence(path, "\\", pathTokens);

		//					for (int index = 0; index < pathTokens.size() - 1; index++)
		//					{
		//						/*if (index == 0)
		//						{
		//						currentPath << ".";
		//						}
		//						else
		//						{
		//						currentPath << "\\";
		//						currentPath << pathTokens[index];
		//						}*/

		//						//since passing the whole path to the obj scene
		//						if (index == 0)
		//						{
		//							currentPath << pathTokens[index];
		//						}
		//						else
		//						{
		//							currentPath << "\\";
		//							currentPath << pathTokens[index];
		//						}
		//						/*currentPath << "\\";
		//						currentPath << pathTokens[index];*/

		//					}

		//					currentPath << "\\" << tokens[1];

		//					//fix here tonight
		//					detailsArray[1] = currentPath.str();
		//				}
		//			}

		//			// [KB] Debugging
		//			//std::cout << "MTL Lines read : [" << linesParsed << "]" << std::endl;
		//		}
		//		catch (exception e)
		//		{
		//			cout << "problem at line: " << line << endl;
		//			cout << e.what() << endl;
		//		}

		//		cout << "the size of the material velctor is: " << allMaterials.size() << "\n";

		//		newFile.close();
		//	}
		//	else
		//	{
		//		cout << "mtl file not found" << endl;
		//	}
		//}
		
		void GenerateMaterial(RGBColor p_diffuseComponent, RGBColor p_specularComponent, int p_illumValue, bool p_containsTexture, string p_texturePath ,string p_materialName, map<string, Material*>& p_allMaterials)
		{

			SURFACE_TYPE currentMaterialType = GetMaterialType(p_illumValue);
			Material* currentMaterial = nullptr;

			if (currentMaterialType == SURFACE_TYPE::UNKOWN_MATERIAL)
				currentMaterialType = SURFACE_TYPE::DIFFUSE_MATERIAL;

			switch(currentMaterialType)
			{
			case SURFACE_TYPE::DIFFUSE_MATERIAL:
				{
					if(p_containsTexture)
						currentMaterial = new Matte(p_materialName, 1, p_texturePath);
					else
						currentMaterial = new Matte(p_materialName, 1, p_diffuseComponent);
				}
				break;

			case SURFACE_TYPE::SPECULAR_MATERIAL:
				{
					if (p_containsTexture)
						currentMaterial = new Reflective(p_specularComponent, p_diffuseComponent, p_texturePath);
					else
						currentMaterial = new Reflective(p_specularComponent, p_diffuseComponent);
				}
				break;
			}

			if(currentMaterial != nullptr)
			{
				p_allMaterials[p_materialName] = currentMaterial;
			}
		}
		

		void GetMaterialsForScene(string p_path, map<string, Material*>& p_allMaterials, bool containsTextures = false)
		{
			int index = 0;
			std::ifstream materialFile(p_path);
			string materialName = "";
			RGBColor diffuseConstant(0,0,0);
			RGBColor specularConstant(0,0,0);
			int illumValue = 0;
			string currentLine = "";
			bool containsTexture = false;
			string texturePath = "";

			if(materialFile.is_open())
			{ 
				while(std::getline(materialFile, currentLine))
				{
					string tokens[5];
					const char* currentLineArray = currentLine.c_str();
					if (currentLine.empty() || currentLineArray[0] == '#')
						continue;
					else
					{
						ParseStringSentence(currentLine, " ", tokens);

						MTL_CONSTANT currentLineType = GetMtlLineType(currentLineArray);

						switch(currentLineType)
						{
							case MTL_CONSTANT::NEW_MTL:
								{
									if (illumValue == 0)
									{
										materialName = tokens[1];
									}
									else
									{
										if (illumValue == SURFACE_TYPE::UNKOWN_MATERIAL)
											illumValue = SURFACE_TYPE::DIFFUSE_MATERIAL;

										if(p_allMaterials.find(materialName) == p_allMaterials.end())
											GenerateMaterial(diffuseConstant, specularConstant, illumValue, containsTexture, texturePath, materialName, p_allMaterials);
										
										materialName = tokens[1];
										containsTexture = false;
									}
								}
								break;

							case MTL_CONSTANT::DIFFUSE_CONSTANT:
								{
									diffuseConstant = RGBColor(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
								}
								break;

							case  MTL_CONSTANT::SPECULAR_CONSTANT:
								{
									specularConstant = RGBColor(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
								}
								break;


							case MTL_CONSTANT::ILLUM_VALUE:
								{
									illumValue = GetMaterialType(std::stoi(tokens[1]));
								}
								break;

							case MTL_CONSTANT::DIFFUSE_TEXTURE:
								{
									containsTexture = true;
									vector<string> pathTokens;
									//split the path to the material file
									ParseStringSentence(p_path, "\\", pathTokens);

									stringstream currentTexturePath;

									//build the texture path
									for (int index = 0; index < pathTokens.size() - 1; ++index)
									{
										if (index == 0)
										{
											currentTexturePath << pathTokens[index];
										}
										else
										{
											currentTexturePath << "\\";
											currentTexturePath << pathTokens[index];
										}
									}

									currentTexturePath << "\\" << tokens[1];

									texturePath = currentTexturePath.str();
								}
								break;
						}
					}
				}

				//remember to add the last remaning material
				
			}
			else
			{
				cout << "Material file not found!!" << __LINE__ << " " << __FILE__ <<  endl;
			}

			if(illumValue != 0)
			{
				//diffuse by default
				if (illumValue == SURFACE_TYPE::UNKOWN_MATERIAL)
					illumValue = SURFACE_TYPE::DIFFUSE_MATERIAL;
				GenerateMaterial(diffuseConstant, specularConstant, illumValue, containsTexture, texturePath, materialName, p_allMaterials);
			}

		}


		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetMaterials
		//		Return Value: VOID
		//		  Parameters: path, map<string, Materials*> materials, bool contains texture
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void GetMaterials(string path, map<string, Material*>& allMaterials, bool containsTextures = false)
		{
			bool newfile = true;
			int index = 0;
			std::string detailsArray[3];
			bool firstTime = false;
			bool finishedFlag = false;
			//string fullPath = "C:\\Users\\Adrian\\Desktop\\" + path;
			//string fullPath = "\ObjFiles\\"+path;
			std::ifstream newFile(path);
			string line;
			if (newFile.is_open())
			{
				try
				{

					vector<string> tokens; // [KB] This is redefined below
					string materialName = "";
					
					RGBColor diffuseColor(0);
					RGBColor specularColor(0);
					bool hasDiffuse = false;
					bool hasSpeculare = false;
					int specularExponent = 1;

		          // [KB] Debugging stuff
		          int linesParsed = 0;

					while (std::getline(newFile, line))
					{
			            //++linesParsed; std::cout << "MTL Line : [" << linesParsed << " : " << line << "]" << std::endl;
			
			            if (line.empty() || line.at(0) == '#') 
							continue;

						//char firstChar = line.at(0);
						//vector<string> tokens; [KB] This has been defined above already! Use previous definition and clear (in case Parse doesn't)
						tokens.clear();

						ParseStringSentence(line, " ", tokens);
            
						if (tokens.size() > 0 && strcmp(tokens[0].c_str(), "newmtl") == 0)
						{
							if (!firstTime)
							{
								materialName = tokens[1];
								firstTime = true;
							}
							else
							{
								//first save then clear and store the new texture name

								bool solidColorExists = detailsArray[0].compare("") != 0;
								bool textureExists = detailsArray[1].compare("") != 0;
								bool specularExi = detailsArray[2].compare("") != 0;

								Matte* currentMaterial;

								//glossy material
								if ((hasSpeculare && hasDiffuse))
								/*{
									vector<std::string> diffuseSubTokens;
									vector<std::string> speculatSubToikens;

									ParseStringSentence(detailsArray[0], " ", diffuseSubTokens);
									ParseStringSentence(detailsArray[2], " ", speculatSubToikens);

									diffuseColor = RGBColor((float)atof(diffuseSubTokens[0].c_str()), (float)atof(diffuseSubTokens[1].c_str()), (float)atof(diffuseSubTokens[2].c_str()));
									specularColor = RGBColor((float)atof(speculatSubToikens[0].c_str()), (float)atof(speculatSubToikens[1].c_str()), (float)atof(speculatSubToikens[2].c_str()));
									
									Glossy* currentMaterial = new Glossy(specularColor, diffuseColor, specularExponent);
									allMaterials[materialName] = currentMaterial;

								}
								else if(hasSpeculare && !hasDiffuse)*/
								{
									vector<std::string> diffuseSubTokens;
									vector<std::string> speculatSubToikens;

									ParseStringSentence(detailsArray[0], " ", diffuseSubTokens);
									ParseStringSentence(detailsArray[2], " ", speculatSubToikens);

									diffuseColor = RGBColor((float)atof(diffuseSubTokens[0].c_str()), (float)atof(diffuseSubTokens[1].c_str()), (float)atof(diffuseSubTokens[2].c_str()));
									specularColor = RGBColor((float)atof(speculatSubToikens[0].c_str()), (float)atof(speculatSubToikens[1].c_str()), (float)atof(speculatSubToikens[2].c_str()));

									Reflective* currentMaterial = new Reflective(specularColor, diffuseColor);
									allMaterials[materialName] = currentMaterial;
								}
								else if(!hasSpeculare && hasDiffuse)
								{
									//only color
									if (solidColorExists && !textureExists)
									{
										vector<std::string> subTokens;

										ParseStringSentence(detailsArray[0], " ", subTokens);

										diffuseColor = RGBColor((float)atof(subTokens[0].c_str()), (float)atof(subTokens[1].c_str()), (float)atof(subTokens[2].c_str()));

										Matte* currentMaterial = new Matte(materialName, 0.75, diffuseColor);
										allMaterials[materialName] = currentMaterial;
									}
									else if (!solidColorExists && textureExists)
									{
										Matte* currentMaterial = new Matte(materialName, 0.75, detailsArray[1]);
										allMaterials[materialName] = currentMaterial;
									}
									else
									{
										vector<std::string> subTokens;

										ParseStringSentence(detailsArray[0], " ", subTokens);

										diffuseColor = RGBColor((float)atof(subTokens[0].c_str()), (float)atof(subTokens[1].c_str()), (float)atof(subTokens[2].c_str()));

										Matte* currentMaterial = new Matte(materialName, 0.75, detailsArray[1], diffuseColor);
										allMaterials[materialName] = currentMaterial;
									}
								}
							}

							detailsArray[0] = "";
							detailsArray[1] = "";
							detailsArray[2] = "";

							hasSpeculare = false;
							hasDiffuse = false;

							materialName = tokens[1];
						}
						else  if (tokens.size() > 0 && strcmp(tokens[0].c_str(), "Kd") == 0)
						{
							float average = std::stof(tokens[1]) + std::stof(tokens[2]) + std::stof(tokens[3]);
							if (average != 0)
							{
								average = average / 3;

								hasDiffuse = true;
								
							}	
							detailsArray[0] = tokens[1] + " " + tokens[2] + " " + tokens[3];
						}
						else if(tokens.size() > 0 && strcmp(tokens[0].c_str(), "Ks") == 0)
						{
							float average = std::stof(tokens[1]) + std::stof(tokens[2]) + std::stof(tokens[3]);
							if (average > 0.01)
							{
								average = average / 3;

								if (average > 0.2)
								{
									
									hasSpeculare = true;
								}
							}
							detailsArray[2] = tokens[1] + " " + tokens[2] + " " + tokens[3];
						}
						else if((tokens.size() > 0 && strcmp(tokens[0].c_str(), "Ns") == 0))
						{
							specularExponent = std::stoi(tokens[1]);
						}
						else if (tokens.size() > 0 && strcmp(tokens[0].c_str(), "map_Kd") == 0)
						{
							/*
								format is : map_Kd filename.ppm
								*/
							//name of file token [1]
							stringstream currentPath;

							vector<string> pathTokens;
							ParseStringSentence(path, "\\", pathTokens);

							for (int index = 0; index < pathTokens.size() - 1; index++)
							{
								/*if (index == 0)
								{
									currentPath << ".";
								}
								else
								{
									currentPath << "\\";
									currentPath << pathTokens[index];
								}*/

								//since passing the whole path to the obj scene
								if (index == 0)
								{
									currentPath << pathTokens[index];
								}
								else
								{
									currentPath << "\\";
									currentPath << pathTokens[index];
								}
								/*currentPath << "\\";
								currentPath << pathTokens[index];*/

							}

							currentPath << "\\" << tokens[1];

							//fix here tonight
							detailsArray[1] = currentPath.str();
							hasDiffuse = true;
						}
					}
					

				  // [KB] Debugging
				  //std::cout << "MTL Lines read : [" << linesParsed << "]" << std::endl;
				}
				catch (exception e)
				{
					cout << "problem at line: " << line << endl;
					cout << e.what() << endl;
				}

				cout << "the size of the material velctor is: " << allMaterials.size() << "\n";

				newFile.close();
			}
			else
			{
				cout << "mtl file not found" << endl;
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ParseObjFile
		//		Return Value: VOID
		//		  Parameters: string path, point3D objectcenter, bool usesTextures
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void ParseObjFile(string path, vector<GeometricObjects*>& allTriangles, Point3D objectCenter = Point3D(0), bool usesTextures = false)
		{
     // std::cout << "[INFO] Application booted from : " << boost::filesystem::initial_path().string() << std::endl;

			vector<Point3D> allVertices;
			vector<Point3D> allNormals;
			vector<Point3D> allTextureVertex;
			std::map<string, Material*> allMaterials;
			//updated 
			std::map<string, TriangleVertex> producedVertices;
			string emptyString = "";

			bool materialsAvailable = false;

			bool v = false;
			bool vt = false;
			bool vn = false;

			string mtlObjectName = "";
			char secondChar = '_';

			string line;
			std::ifstream myfile(path);
			if (myfile.is_open())
			{
				try
				{
					while (std::getline(myfile, line))
					{
						vector<string> tokens;
						string currentToken = "";
						ParseStringSentence(line, " ", tokens);

						if (line.compare(emptyString) == 0 || tokens.size() == 0)
						{
							continue;
						}

						char firstChar = line.at(0);
						if (line.length() > 1)
							secondChar = line.at(1);

						if (firstChar == '#')
						{
							continue;
						}

						//if materials mtllib 
						else if (firstChar == 'm' && secondChar == 't')
						{
							//load materials
							string materialPath = tokens[1];

							vector<string> pathTokens;
							ParseStringSentence(path, "\\", pathTokens);

							stringstream textFilePath;
							if (pathTokens.size() > 1)
							{
								for (int index = 0; index < pathTokens.size() - 1; index++)
								{
									if (index == 0)
										textFilePath <<  pathTokens[index];
									else
										textFilePath << "\\" << pathTokens[index];
								}
							}


							textFilePath << "\\" << materialPath;

							clock_t startingTime = clock();
							GetMaterialsForScene(textFilePath.str(), allMaterials, usesTextures);
							clock_t endingTime = clock();

							cout << "time taken fort materials is :" << (endingTime - startingTime)/ CLOCKS_PER_SEC << endl;

							materialsAvailable = true;
						}
						//when it finds usemtl 
						else if (firstChar == 'u' && secondChar == 's')
						{
							mtlObjectName = tokens[1];
						}
						else if (tokens.size() > 0)
						{
							//first token is always what is the type mof the object eg V or F
							string typeOfLine = tokens[0];
							if (firstChar == 'f')
							{
								//tokens = vector<string>(tokens.begin() + 1, tokens.end());

								try
								{
									//triangle
									if (tokens.size() == 4)
									{
										vector<string> output;

										TriangleVertex a;
										TriangleVertex b;
										TriangleVertex c;

										for (int subIndex = 1; subIndex < 4; subIndex++)
										{
											TriangleVertex& pointer = subIndex == 1 ? a : subIndex == 2 ? b : c;
											vector<std::string> output;

											ParseValues(tokens[subIndex], "/", output);

											std::map<string, TriangleVertex>::iterator it = producedVertices.find(tokens[subIndex]);

											if (it != producedVertices.end())
											{
												pointer = producedVertices[tokens[subIndex]];
											}
											else
											{
												Point3D tempVertex = output[0].compare("") == 0 ? NULL : allVertices.at((int)atoi(output[0].c_str()) - 1);
												Point3D temptextureVertex = output[1].compare("") == 0 ? NULL : allTextureVertex.at(atoi(output[1].c_str()) - 1);
												Point3D tempNormal = output[2].compare("") == 0 ? NULL : allNormals.at(atoi(output[2].c_str()) - 1);
												producedVertices[tokens[subIndex]] = TriangleVertex(tempVertex, tempNormal, temptextureVertex);;
												pointer = producedVertices[tokens[subIndex]];
											}
										}

										//cout << mtlObjectName << endl;
										Material *currentMaterial = allMaterials[mtlObjectName];

										if (currentMaterial != nullptr)
										{
											Triangle *currentTriangle = new Triangle(a, b, c, currentMaterial, mtlObjectName);
											allTriangles.push_back(currentTriangle);
										}
										else
										{

											cout << "we may have a problem " << mtlObjectName <<endl;
										}
									}
									else if (tokens.size() == 5)
									{
										vector<string> output;

										TriangleVertex vertexArray[4];

										for (int subIndex = 1; subIndex <= 4; subIndex++)
										{
											TriangleVertex& pointer = vertexArray[subIndex - 1];
											vector<std::string> output;
											ParseValues(tokens[subIndex], "/", output);
											std::map<string, TriangleVertex>::iterator it = producedVertices.find(tokens[subIndex]);

											if (it != producedVertices.end())
											{
												pointer = producedVertices[tokens[subIndex]];
											}
											else
											{
												Point3D tempVertex = output[0].compare("") == 0 ? NULL : allVertices.at((int)atoi(output[0].c_str()) - 1);
												Point3D temptextureVertex = output[1].compare("") == 0 ? NULL : allTextureVertex.at(atoi(output[1].c_str()) - 1);
												Point3D tempNormal = output[2].compare("") == 0 ? NULL : allNormals.at(atoi(output[2].c_str()) - 1);
												producedVertices[tokens[subIndex]] = TriangleVertex(tempVertex, tempNormal, temptextureVertex);;
												pointer = producedVertices[tokens[subIndex]];
											}
										}

										Material *currentMaterial = allMaterials[mtlObjectName];
										if (materialsAvailable)
										{
											Triangle *firstTriangle = new Triangle(vertexArray[0], vertexArray[1], vertexArray[2], currentMaterial, mtlObjectName);
											Triangle *secondTriangle = new Triangle(vertexArray[2], vertexArray[3], vertexArray[0], currentMaterial, mtlObjectName);
											allTriangles.push_back(firstTriangle);
											allTriangles.push_back(secondTriangle);
										}
										else
										{
											Triangle *firstTriangle = new Triangle(vertexArray[0], vertexArray[1], vertexArray[2]);
											Triangle *secondTriangle = new Triangle(vertexArray[2], vertexArray[3], vertexArray[0]);
											allTriangles.push_back(firstTriangle);
											allTriangles.push_back(secondTriangle);
										}
											

										
									}

								}
								catch (std::exception e)
								{
									std::cout << "error " << line << std::endl;
								}
							}
							else if (firstChar == 'v' && secondChar == 'n')
							{
								float xCoor = atof(tokens[1].c_str());
								float yCoor = atof(tokens[2].c_str());
								float zCoor = atof(tokens[3].c_str());

								Point3D currentVector(xCoor, yCoor, zCoor);
								allNormals.push_back(currentVector);
							}
							else if (firstChar == 'v' && secondChar == 't')
							{
								float xCoor = atof(tokens[1].c_str());
								float yCoor = atof(tokens[2].c_str());
								//float zCoor = atof(tokens[3].c_str());

								Point3D currentVector(xCoor, yCoor, 0);
								allTextureVertex.push_back(currentVector);
							}
							else if (firstChar == 'v')
							{
								float xCoor = atof(tokens[1].c_str());
								float yCoor = atof(tokens[2].c_str());
								float zCoor = atof(tokens[3].c_str());

								Point3D currentVector((xCoor + objectCenter.x), (yCoor + objectCenter.y), (zCoor + objectCenter.z));
								allVertices.push_back(currentVector);
							}

						}
						tokens.clear();
					}

					myfile.close();
				}
				catch (exception e)
				{
					cout << "Error when reading line" << endl;
				}
			}
			else
			{
				cout << "Error when opening the file" << endl;
			}

			Engine::Logger::LogTrace("normals --- " + to_string(allNormals.size()));
			Engine::Logger::LogTrace("all vertices --- " + to_string(allVertices.size()));
			Engine::Logger::LogTrace("all materials --- " + to_string(allMaterials.size()));
 			Engine::Logger::LogTrace("all triangles ---" + to_string(allTriangles.size()));


			//return allTriangles;
		}

	private:

		//----------------------------------------------------------------------------
		// Surface type: Constant integer number for each material type
		//----------------------------------------------------------------------------
		enum SURFACE_TYPE
		{
			DIFFUSE_MATERIAL = 1,
			SPECULAR_MATERIAL = 2,
			UNKOWN_MATERIAL = 200
		};

		//----------------------------------------------------------------------------
		// MTL CONSTANT: ENUM representing all possible mtl objects to be encountered
		//----------------------------------------------------------------------------
		enum MTL_CONSTANT
		{
			NEW_MTL = 0,
			SPECULAR_CONSTANT = 2,
			DIFFUSE_CONSTANT = 1,
			ILLUM_VALUE = 3,
			DIFFUSE_TEXTURE = 4,
			NOT_DEFINED = 100
		};

		//----------------------------------------------------------------------------
		// GetMtlLineTypee: Returns an ENUM value for the corresponding line
		//----------------------------------------------------------------------------
		MTL_CONSTANT GetMtlLineType(const char* p_currentLine)
		{
			if(p_currentLine[0] == 'n' && p_currentLine[1] == 'e')
			{
				return MTL_CONSTANT::NEW_MTL;
			}
			else if(p_currentLine[0] == 'K' && p_currentLine[1] == 'd')
			{
				return DIFFUSE_CONSTANT;
			}
			else if(p_currentLine[0] == 'K' && p_currentLine[1] == 's')
			{
				return SPECULAR_CONSTANT;
			}
			else if(p_currentLine[0] == 'i' && p_currentLine[1] == 'l')
			{
				return ILLUM_VALUE;
			}
			else if(p_currentLine[0] == 'm' && p_currentLine[1] == 'a' && p_currentLine[4] == 'K' && p_currentLine[5] == 'd')
			{
				return DIFFUSE_TEXTURE;
			}
			else
			{
				return NOT_DEFINED;
			}
		}



		//----------------------------------------------------------------------------
		// GetMaterialType: Returns a SURFACE_TYPE value for the current 'illum' value
		//----------------------------------------------------------------------------
		SURFACE_TYPE GetMaterialType(int p_illumValue)
		{
			switch(p_illumValue)
			{
			case 1:
				return SURFACE_TYPE::DIFFUSE_MATERIAL;
			case 2:
				return SURFACE_TYPE::SPECULAR_MATERIAL;
			default:
				cout << "Error material type not handled" << " " << __LINE__ << endl;
				return SURFACE_TYPE::UNKOWN_MATERIAL;
			}
		}



		

	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------

