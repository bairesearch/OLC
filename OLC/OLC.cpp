/*******************************************************************************
 *
 * File Name: OLC.cpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2021 Baxter AI (baxterai.com)
 * Project: LD Optional Lines Creator (Calculator)
 * Project Version: 1d1b 07-August-2021 (github.com/baxterai/OLC) (automatic source upgrades: generate const / generate OO)
 * First Public Release: 1b9b 12-June-2007 (https://www.philohome.com/isecalc/olc.htm) 
 * Second Public Release: 1c1b 19-Sept-2009 (https://www.baxterai.com/utils/misc/OLC/OLC.html) (part file white space/syntax requirement relaxation, Added UI option to create edge line instead of optional line where the angle between the two intersecting polygons is less than x degrees)
 * Comments: NB - no recursive parsing.
 *
 *******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <math.h>

#include "OLC.hpp"
//using namespace std;


int numpolysfound = 0;
int totalNumOptionalLines = 0;
int samespolysfound = 0;
int ordinaryedgesfound = 0;
int alreadyaddedoptionallinesfound = 0;
int edgeAndOptionalLineOnSamePlaneFound = 0;
int alreadyaddededgelinesfound = 0;
int totalNumEdgeLines = 0;

static char errmessage[] = "Usage:  OLC.exe [options]\n\n\twhere options are any of the following\n\n\t-input [string]     : .dat input filename (def: part.dat)\n\n\t-output [string]     : .dat output filename (def: partNew.dat)\n\n\t-addedges [float]   : create edges instead of optional lines\n \twhen poly intersection angle is less than x degrees, Eg 160 (def: disabled) \n\n\t-version          : print version\n\n\tThe program adds optional lines (LDraw line type 5)\n \tbetween polygons of a file.\n\n";

int main(const int argc,const char* *argv)
{
	cout << errmessage << endl;

	string partFileOriginalName = "part.dat"; 
	string partFileModifiedName = "partNew.dat";
	bool createEdges;		//createEdgesInsteadOfOLsAtAGivenAngleThreshold
	float edgeThreshold;	//edgeVersusOLAngleThreshold in degrees

	//set defaults;
	createEdges = false;
	edgeThreshold = 0;

	if(SHAREDvarsClass().argumentExists(argc,argv,"-input"))
	{
		partFileOriginalName=SHAREDvarsClass().getStringArgument(argc,argv,"-input");
	}

	if(SHAREDvarsClass().argumentExists(argc,argv,"-output"))
	{
		partFileModifiedName=SHAREDvarsClass().getStringArgument(argc,argv,"-output");
	}

	if(SHAREDvarsClass().argumentExists(argc,argv,"-addedges"))
	{
		createEdges = true;										
		edgeThreshold=SHAREDvarsClass().getFloatArgument(argc,argv,"-addedges");	
	}
	if(SHAREDvarsClass().argumentExists(argc,argv,"-version"))
	{
		cout << "OLC.exe version: 1c1a (19 Sept 09a)" << endl;
		exit(1);
	}

	edgeThreshold = 180 - edgeThreshold;

	int result = true;

	//for parsing cs.txt
	if(!OLCClass().createPartWithOptionalLines(partFileOriginalName, partFileModifiedName, createEdges, edgeThreshold))
	{
		result = false;
	}

	return result;
}

bool OLCClass::createPartWithOptionalLines(string partFileOriginalName, const string partFileModifiedName, bool createEdges, float edgeThreshold)
{
	int result = true;


	ifstream partFileOriginal(partFileOriginalName.c_str());

	if(!partFileOriginal.rdbuf()->is_open())
	{
		cout << "part.dat does not exist in current directory." << endl;
		result = false;
	}
	else
	{
		partFileOriginal.close();

		//declare initial scene references
		LDreference* initialReferenceInPart = new LDreference();
		LDreference* topLevelReferenceInPart = new LDreference();		//not used in OLC project

		if(!LDparser.parseFile(partFileOriginalName, initialReferenceInPart, topLevelReferenceInPart, FALSE))
		{
			result = false;
		}

		if(!this->partOptionalLinesFileOutput(partFileModifiedName, partFileOriginalName, initialReferenceInPart, createEdges, edgeThreshold))
		{
			result = false;
		}
	}

	cout << "numpolysfound = " << numpolysfound << endl;
	cout << "totalNumOptionalLines = " << totalNumOptionalLines << endl;
	cout << "samespolysfound = " << samespolysfound << endl;
	cout << "ordinaryedgesfound = " << ordinaryedgesfound << endl;
	cout << "alreadyaddedoptionallinesfound = " << alreadyaddedoptionallinesfound << endl;
	cout << "edgeAndOptionalLineOnSamePlaneFound = " << edgeAndOptionalLineOnSamePlaneFound << endl;
	cout << "alreadyaddededgelinesfound = " << alreadyaddededgelinesfound << endl;
	cout << "totalNumEdgeLines = " << totalNumEdgeLines << endl;

	return result;
}

bool OLCClass::partOptionalLinesFileOutput(const string partFileModifiedName, const string partFileOriginalName, LDreference* initialReferenceInPart, bool createEdges, float edgeThreshold)
{
	LDreference* tempcurrentReference = initialReferenceInPart;
	while(tempcurrentReference->next != NULL)		//nothing detected here in 1a3a
	{
		/*
		cout << tempcurrentReference->type 
		<< " " << tempcurrentReference->colour 
		<< " " << tempcurrentReference->vertex1relativePosition.x 
		<< " " << tempcurrentReference->vertex1relativePosition.y 
		<< " " << tempcurrentReference->vertex1relativePosition.z 
		<< " " << tempcurrentReference->vertex2relativePosition.x 
		<< " " << tempcurrentReference->vertex2relativePosition.y 
		<< " " << tempcurrentReference->vertex2relativePosition.z 
		<< " " << tempcurrentReference->vertex3relativePosition.x 
		<< " " << tempcurrentReference->vertex3relativePosition.y 
		<< " " << tempcurrentReference->vertex3relativePosition.z 
		<< " " << tempcurrentReference->vertex4relativePosition.x 
		<< " " << tempcurrentReference->vertex4relativePosition.y 
		<< " " << tempcurrentReference->vertex4relativePosition.z << endl; 
		*/

		/*
		cout << "tempcurrentReference->type = " << tempcurrentReference->type << endl; 
		cout << "tempcurrentReference->colour = " << tempcurrentReference->colour << endl; 
		cout << "tempcurrentReference->vertex1relativePosition.x = " << vertex1relativePosition.x << endl; 
		cout << "tempcurrentReference->vertex1relativePosition.y = " << vertex1relativePosition.y << endl; 
		cout << "tempcurrentReference->vertex1relativePosition.z = " << vertex1relativePosition.z << endl; 
		cout << "tempcurrentReference->vertex2relativePosition.x = " << vertex2relativePosition.x << endl; 
		cout << "tempcurrentReference->vertex2relativePosition.y = " << vertex2relativePosition.y << endl; 
		cout << "tempcurrentReference->vertex2relativePosition.z = " << vertex2relativePosition.z << endl; 
		cout << "tempcurrentReference->vertex3relativePosition.x = " << vertex3relativePosition.x << endl; 
		cout << "tempcurrentReference->vertex3relativePosition.y = " << vertex3relativePosition.y << endl; 
		cout << "tempcurrentReference->vertex3relativePosition.z = " << vertex3relativePosition.z << endl; 
		cout << "tempcurrentReference->vertex4relativePosition.x = " << vertex4relativePosition.x << endl; 
		cout << "tempcurrentReference->vertex4relativePosition.y = " << vertex4relativePosition.y << endl; 
		cout << "tempcurrentReference->vertex4relativePosition.z = " << vertex4relativePosition.z << endl; 
		*/
		tempcurrentReference = tempcurrentReference->next;
	}



	int count = 0;

	char c;	//current character being read in

	ifstream parseFileOriginal(partFileOriginalName.c_str());
	ofstream partFileModified(partFileModifiedName.c_str());

	/*add original part file data*/

  	while((parseFileOriginal).get(c))
	{
		count++;
		(partFileModified).put(c);
	}
	//cout << "count = " << count << endl;
	

	/***************************** PART 1* **********************************************/
	/*determine optional line data (create optional line data reference list)*/
	LDreference* initialReferenceInOptionalLineData = new LDreference();
	initialReferenceInOptionalLineData->type = REFERENCE_TYPE_COMMENT;
	LDreference* firstRealReferenceInOptionalLineData = new LDreference();
	initialReferenceInOptionalLineData->next = firstRealReferenceInOptionalLineData;
	this->createReferenceListOfOptionalLines(initialReferenceInPart, initialReferenceInOptionalLineData, createEdges, edgeThreshold);

	cout << "createReferenceListOfOptionalLines successful" << endl;

	/***************************** PART 2* **********************************************/
	/*add optional line data to modified part file*/
	string optionalLineReferenceString = "";
	LDreference* currentReference = initialReferenceInOptionalLineData;

	(partFileModified).write(OPTIONAL_LINE_DATA_HEADER, strlen(OPTIONAL_LINE_DATA_HEADER));
	while(currentReference->next != NULL)		//nothing detected here in 1a3a
	{
		LDreferenceManipulation.convertReferenceToString(currentReference, &optionalLineReferenceString);

		(partFileModified).write(optionalLineReferenceString.c_str(), optionalLineReferenceString.length());

		currentReference = currentReference->next;
	}

	parseFileOriginal.close();
	partFileModified.close();

	return true;
}


/***************************** PART 1* **********************************************/

void OLCClass::createReferenceListOfOptionalLines(LDreference* initialReferenceInPart, LDreference* initialReferenceInOptionalLineData, bool createEdges, float edgeThreshold)
{
	LDreference* currentReferenceInPart = initialReferenceInPart;
	LDreference* currentReferenceInOptionalLineData = initialReferenceInOptionalLineData->next;

	bool currentFunctionResult = false;

	while(currentReferenceInPart->next != NULL)
	{
		/*
		cout << currentReferenceInPart->type 
		<< " " << currentReferenceInPart->colour 
		<< " " << currentReferenceInPart->vertex1relativePosition.x 
		<< " " << currentReferenceInPart->vertex1relativePosition.y 
		<< " " << currentReferenceInPart->vertex1relativePosition.z 
		<< " " << currentReferenceInPart->vertex2relativePosition.x 
		<< " " << currentReferenceInPart->vertex2relativePosition.y 
		<< " " << currentReferenceInPart->vertex2relativePosition.z 
		<< " " << currentReferenceInPart->vertex3relativePosition.x 
		<< " " << currentReferenceInPart->vertex3relativePosition.y 
		<< " " << currentReferenceInPart->vertex3relativePosition.z 
		<< " " << currentReferenceInPart->vertex4relativePosition.x 
		<< " " << currentReferenceInPart->vertex4relativePosition.y 
		<< " " << currentReferenceInPart->vertex4relativePosition.z << endl; 
		*/


		if(currentReferenceInPart->type == REFERENCE_TYPE_QUAD)
		{
			numpolysfound++;
			currentReferenceInOptionalLineData = this->addOptionalLineReferencesForQuad(currentReferenceInPart, currentReferenceInOptionalLineData, initialReferenceInPart, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);
		}
		else if(currentReferenceInPart->type == REFERENCE_TYPE_TRI)
		{
			numpolysfound++;
			currentReferenceInOptionalLineData = this->addOptionalLineReferencesForTri(currentReferenceInPart, currentReferenceInOptionalLineData, initialReferenceInPart, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);
		}
		else
		{/*currentReferenceInPart->type == REFERENCE_TYPE_LINE/REFERENCE_TYPE_SUBMODEL/REFERENCE_TYPE_OPTIONALLINE*/
		}

		currentReferenceInPart = currentReferenceInPart->next;
	}
}



//unfinished
LDreference* OLCClass::addOptionalLineReferencesForQuad(LDreference* quadReference, LDreference* currentReferenceInOptionalLineData, LDreference* initialReferenceInPart, LDreference* initialReferenceInOptionalLineData, bool* result, bool createEdges, float edgeThreshold)
{
		//method;

		//calculate all optional line references to be added for the given quad...
			//For every optional line reference calculated, check if optional line reference already exists...
				//if the optional line reference already exists, do not add the optional line reference to the optional line data, currentReferenceInOptionalLineData
				//if the optional line reference does not yet exist, add the optional line reference to the optional line data, currentReferenceInOptionalLineData

		LDreference* modCurrentReferenceInOptionalLineData;
		bool currentFunctionResult = false;

		LDreference quadReferenceReordered;
		quadReferenceReordered.type = quadReference->type;
		quadReferenceReordered.colour = quadReference->colour;
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex1relativePosition), &(quadReference->vertex1relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex2relativePosition), &(quadReference->vertex2relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex3relativePosition), &(quadReference->vertex3relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex4relativePosition), &(quadReference->vertex4relativePosition));
		currentReferenceInOptionalLineData = this->findCorrespondingPrimRefEdgeAndAddOptionalLine(quadReference, &quadReferenceReordered, currentReferenceInOptionalLineData, initialReferenceInPart, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex1relativePosition), &(quadReference->vertex2relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex2relativePosition), &(quadReference->vertex3relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex3relativePosition), &(quadReference->vertex4relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex4relativePosition), &(quadReference->vertex1relativePosition));
		currentReferenceInOptionalLineData = this->findCorrespondingPrimRefEdgeAndAddOptionalLine(quadReference, &quadReferenceReordered, currentReferenceInOptionalLineData, initialReferenceInPart, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex1relativePosition), &(quadReference->vertex3relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex2relativePosition), &(quadReference->vertex4relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex3relativePosition), &(quadReference->vertex1relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex4relativePosition), &(quadReference->vertex2relativePosition));
		currentReferenceInOptionalLineData = this->findCorrespondingPrimRefEdgeAndAddOptionalLine(quadReference, &quadReferenceReordered, currentReferenceInOptionalLineData, initialReferenceInPart, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex1relativePosition), &(quadReference->vertex4relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex2relativePosition), &(quadReference->vertex1relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex3relativePosition), &(quadReference->vertex2relativePosition));
		SHAREDvector.copyVectors(&(quadReferenceReordered.vertex4relativePosition), &(quadReference->vertex3relativePosition));
		currentReferenceInOptionalLineData = this->findCorrespondingPrimRefEdgeAndAddOptionalLine(quadReference, &quadReferenceReordered, currentReferenceInOptionalLineData, initialReferenceInPart, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);

		modCurrentReferenceInOptionalLineData = currentReferenceInOptionalLineData;
		*result = currentFunctionResult;
		return modCurrentReferenceInOptionalLineData;
}

//unfinished
LDreference* OLCClass::addOptionalLineReferencesForTri(LDreference* triReference, LDreference* currentReferenceInOptionalLineData, LDreference* initialReferenceInPart, LDreference* initialReferenceInOptionalLineData, bool* result, bool createEdges, float edgeThreshold)
{
		//method;

		//calculate all optional line references to be added for the given quad...
			//For every optional line reference calculated, check if optional line reference already exists...
				//if the optional line reference already exists, do not add the optional line reference to the optional line data, currentReferenceInOptionalLineData
				//if the optional line reference does not yet exist, add the optional line reference to the optional line data, currentReferenceInOptionalLineData

		LDreference* modCurrentReferenceInOptionalLineData;
		bool currentFunctionResult = false;

		LDreference triReferenceReordered;
		triReferenceReordered.type = triReference->type;
		triReferenceReordered.colour = triReference->colour;
		SHAREDvector.copyVectors(&(triReferenceReordered.vertex1relativePosition), &(triReference->vertex1relativePosition));
		SHAREDvector.copyVectors(&(triReferenceReordered.vertex2relativePosition), &(triReference->vertex2relativePosition));
		SHAREDvector.copyVectors(&(triReferenceReordered.vertex3relativePosition), &(triReference->vertex3relativePosition));
		currentReferenceInOptionalLineData = this->findCorrespondingPrimRefEdgeAndAddOptionalLine(triReference, &triReferenceReordered, currentReferenceInOptionalLineData, initialReferenceInPart, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);
		SHAREDvector.copyVectors(&(triReferenceReordered.vertex1relativePosition), &(triReference->vertex2relativePosition));
		SHAREDvector.copyVectors(&(triReferenceReordered.vertex2relativePosition), &(triReference->vertex3relativePosition));
		SHAREDvector.copyVectors(&(triReferenceReordered.vertex3relativePosition), &(triReference->vertex1relativePosition));
		currentReferenceInOptionalLineData = this->findCorrespondingPrimRefEdgeAndAddOptionalLine(triReference, &triReferenceReordered, currentReferenceInOptionalLineData, initialReferenceInPart, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);
		SHAREDvector.copyVectors(&(triReferenceReordered.vertex1relativePosition), &(triReference->vertex3relativePosition));
		SHAREDvector.copyVectors(&(triReferenceReordered.vertex2relativePosition), &(triReference->vertex1relativePosition));
		SHAREDvector.copyVectors(&(triReferenceReordered.vertex3relativePosition), &(triReference->vertex2relativePosition));
		currentReferenceInOptionalLineData = this->findCorrespondingPrimRefEdgeAndAddOptionalLine(triReference, &triReferenceReordered, currentReferenceInOptionalLineData, initialReferenceInPart, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);

		modCurrentReferenceInOptionalLineData = currentReferenceInOptionalLineData;
		*result = currentFunctionResult;
		return modCurrentReferenceInOptionalLineData;
}




//unfinished
	//edgeNumbers description [polyReferenceEdgeNumber; 1 = vertex 1 and 2, 2 = vertex 2 and 3, 3 = vertex 3 and 4, 4 = vertex 4 and 1)
	/*NB this code assumes that any two coordinates that are equal will always be stated in the same precision in the ldraw part file*/
LDreference* OLCClass::findCorrespondingPrimRefEdgeAndAddOptionalLine(const LDreference* polyReference, LDreference* polyReferenceRotated, LDreference* currentReferenceInOptionalLineData, LDreference* initialReferenceInPart, LDreference* initialReferenceInOptionalLineData, bool* result, bool createEdges, float edgeThreshold)
{
	vec* polyReferenceVertexA = &(polyReferenceRotated->vertex1relativePosition);
	vec* polyReferenceVertexB = &(polyReferenceRotated->vertex2relativePosition);

	LDreference* modCurrentReferenceInOptionalLineData;
	bool currentFunctionResult = false;

		//primaryConditions check/////////////////////////////////////////////////////////////
	bool primaryConditions = true;
	LDreference lineReference;
	lineReference.type = REFERENCE_TYPE_LINE;
	lineReference.colour = DAT_FILE_DEFAULT_COLOUR_EDGELINE;
	SHAREDvector.copyVectors(&(lineReference.vertex1relativePosition), polyReferenceVertexA);
	SHAREDvector.copyVectors(&(lineReference.vertex2relativePosition), polyReferenceVertexB);
	if(LDreferenceManipulation.search1DrefListFindRef(&lineReference, initialReferenceInPart))
	{
		//do not add an optional line if an ordindary line already exists for the given edge
		primaryConditions = false;
		ordinaryedgesfound++;
		//cout << "here50" << endl;
	}
	SHAREDvector.copyVectors(&(lineReference.vertex1relativePosition), polyReferenceVertexB);
	SHAREDvector.copyVectors(&(lineReference.vertex2relativePosition), polyReferenceVertexA);
	if(LDreferenceManipulation.search1DrefListFindRef(&lineReference, initialReferenceInPart))
	{
		//do not add an optional line if an ordindary line already exists for the given edge
		primaryConditions = false;
		ordinaryedgesfound++;
		//cout << "here50" << endl;
	}
		////////////////////////////////////////////////////////////////////////////////////////

	if(primaryConditions == true)
	{
		LDreference* currentReferenceInPart = initialReferenceInPart;

		while(currentReferenceInPart->next != NULL)
		{
				//secondaryConditions check/////////////////////////////////////////////////////////////
			bool secondaryConditions = false;

			if((currentReferenceInPart->type == REFERENCE_TYPE_QUAD) || (currentReferenceInPart->type == REFERENCE_TYPE_TRI))
			{
				secondaryConditions = true;
				if(currentReferenceInPart->type == REFERENCE_TYPE_QUAD)
				{
					if(LDreferenceManipulation.compareReferences(currentReferenceInPart, polyReference, REFERENCE_TYPE_QUAD))
					{
						samespolysfound++;
						secondaryConditions = false;
					}
				}
				else if(currentReferenceInPart->type == REFERENCE_TYPE_TRI)
				{
					if(LDreferenceManipulation.compareReferences(currentReferenceInPart, polyReference, REFERENCE_TYPE_TRI))
					{
						samespolysfound++;
						secondaryConditions = false;
					}
				}
			}
				////////////////////////////////////////////////////////////////////////////////////////

			LDreference poly2Rotated;
			poly2Rotated.type = currentReferenceInPart->type;
			poly2Rotated.colour = currentReferenceInPart->colour;
			if(secondaryConditions == true)
			{
				bool vectorComparison1 = false;
				if(currentReferenceInPart->type == REFERENCE_TYPE_QUAD)
				{
					if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex1relativePosition), polyReferenceVertexA)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex2relativePosition), polyReferenceVertexB)))	
					{
						vectorComparison1 = true;
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex4relativePosition), &(currentReferenceInPart->vertex4relativePosition));
					}
					else if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex1relativePosition), polyReferenceVertexB)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex2relativePosition), polyReferenceVertexA)))
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex4relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex4relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						vectorComparison1 = true;
					}
				}
				else if(currentReferenceInPart->type == REFERENCE_TYPE_TRI)
				{
					if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex1relativePosition), polyReferenceVertexA)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex2relativePosition), polyReferenceVertexB)))	
					{
						vectorComparison1 = true;
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex3relativePosition));
					}
					else if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex1relativePosition), polyReferenceVertexB)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex2relativePosition), polyReferenceVertexA)))
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						vectorComparison1 = true;
					}
				}

				if(vectorComparison1 == true)
				{
					//cout << "vectorComparison1 = true;" << endl;
					//determine optional line required;
					currentReferenceInOptionalLineData = this->fillEdgeOrOptionalLine(polyReferenceRotated, &poly2Rotated, currentReferenceInOptionalLineData, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);

				}

				bool vectorComparison2 = false;
				if(currentReferenceInPart->type == REFERENCE_TYPE_QUAD)
				{
					if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex2relativePosition), polyReferenceVertexA)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex3relativePosition), polyReferenceVertexB))) 
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex4relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex4relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						vectorComparison2 = true;
					}
					else if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex2relativePosition), polyReferenceVertexB)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex3relativePosition), polyReferenceVertexA)))
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex4relativePosition), &(currentReferenceInPart->vertex4relativePosition));
						vectorComparison2 = true;
					}
				
				}
				else if(currentReferenceInPart->type == REFERENCE_TYPE_TRI)
				{
					if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex2relativePosition), polyReferenceVertexA)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex3relativePosition), polyReferenceVertexB))) 
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						vectorComparison2 = true;
					}
					else if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex2relativePosition), polyReferenceVertexB)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex3relativePosition), polyReferenceVertexA)))
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						vectorComparison2 = true;
					}
				}
				if(vectorComparison2 == true)
				{
					//cout << "vectorComparison2 = true;" << endl;
					//determine optional line required;
					currentReferenceInOptionalLineData = this->fillEdgeOrOptionalLine(polyReferenceRotated, &poly2Rotated, currentReferenceInOptionalLineData, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);
				}

				bool vectorComparison3 = false;
				if(currentReferenceInPart->type == REFERENCE_TYPE_QUAD)
				{
					if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex3relativePosition), polyReferenceVertexA)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex4relativePosition), polyReferenceVertexB))) 
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex4relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex4relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						vectorComparison3 = true;
					}
					else if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex3relativePosition), polyReferenceVertexB)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex4relativePosition), polyReferenceVertexA)))
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex4relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex4relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						vectorComparison3 = true;
					}
				}
				else if(currentReferenceInPart->type == REFERENCE_TYPE_TRI)
				{
					if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex3relativePosition), polyReferenceVertexA)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex1relativePosition), polyReferenceVertexB)))
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						vectorComparison3 = true;
					}
					else if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex3relativePosition), polyReferenceVertexB)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex1relativePosition), polyReferenceVertexA)))
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						vectorComparison3 = true;
					}
				}
				if(vectorComparison3 == true)
				{
					//cout << "vectorComparison3 = true;" << endl;
					//determine optional line required;
					currentReferenceInOptionalLineData = this->fillEdgeOrOptionalLine(polyReferenceRotated, &poly2Rotated, currentReferenceInOptionalLineData, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);

				}

				bool vectorComparison4 = false;
				if(currentReferenceInPart->type == REFERENCE_TYPE_QUAD)
				{
					if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex4relativePosition), polyReferenceVertexA)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex1relativePosition), polyReferenceVertexB))) 
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex4relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex4relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						vectorComparison4 = true;
					}
					else if((SHAREDvector.compareVectors(&(currentReferenceInPart->vertex4relativePosition), polyReferenceVertexB)) && (SHAREDvector.compareVectors(&(currentReferenceInPart->vertex1relativePosition), polyReferenceVertexA)))
					{
						SHAREDvector.copyVectors(&(poly2Rotated.vertex1relativePosition), &(currentReferenceInPart->vertex1relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex2relativePosition), &(currentReferenceInPart->vertex4relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex3relativePosition), &(currentReferenceInPart->vertex3relativePosition));
						SHAREDvector.copyVectors(&(poly2Rotated.vertex4relativePosition), &(currentReferenceInPart->vertex2relativePosition));
						vectorComparison4 = true;
					}
					if(vectorComparison4 == true)
					{
						//cout << "vectorComparison4 = true;" << endl;
						//determine optional line required;
						currentReferenceInOptionalLineData = this->fillEdgeOrOptionalLine(polyReferenceRotated, &poly2Rotated, currentReferenceInOptionalLineData, initialReferenceInOptionalLineData, &currentFunctionResult, createEdges, edgeThreshold);
					}
				}
			}

			currentReferenceInPart = currentReferenceInPart->next;
		}
	}

	//cout << " numCorrespondingPrimitiveReferenceEdge = " << numCorrespondingPrimitiveReferenceEdge << endl;

	modCurrentReferenceInOptionalLineData = currentReferenceInOptionalLineData;
	*result = currentFunctionResult;
	return modCurrentReferenceInOptionalLineData;

}

LDreference* OLCClass::fillEdgeOrOptionalLine(LDreference* polyReferenceRotated, LDreference* currentReferenceInPartRotated, LDreference* currentReferenceInOptionalLineData, LDreference* initialReferenceInOptionalLineData, bool* result, const bool createEdges, const float edgeThreshold)
{
	bool currentFunctionResult = false;
	vec* polyReferenceVertexA = &(polyReferenceRotated->vertex1relativePosition);
	vec* polyReferenceVertexB = &(polyReferenceRotated->vertex2relativePosition);

	double angleBetweenNormalsOfPolys = this->calculateAngleBetweenNormalsOfPolys(polyReferenceRotated, currentReferenceInPartRotated);
	if(angleBetweenNormalsOfPolys > 179.0)
	{
		//polygons are parallel/colinear - do not add a line		
	}
	else if(angleBetweenNormalsOfPolys < 1.0)
	{
		//polygons are parallel/colinear - do not add a line		
	}
	else if(createEdges && (angleBetweenNormalsOfPolys > edgeThreshold))
	{
		//then create edge
		currentReferenceInOptionalLineData = this->fillEdgeLine(polyReferenceVertexA, polyReferenceVertexB, currentReferenceInOptionalLineData, initialReferenceInOptionalLineData, &currentFunctionResult);
	}
	else
	{
		//create optionaline
		currentReferenceInOptionalLineData = this->fillOptionalLine(polyReferenceRotated, currentReferenceInPartRotated, currentReferenceInOptionalLineData, initialReferenceInOptionalLineData, &currentFunctionResult);
	}

	*result = currentFunctionResult;
	return currentReferenceInOptionalLineData;
}

double OLCClass::calculateAngleBetweenNormalsOfPolys(LDreference* polyReferenceRotated, LDreference* currentReferenceInPartRotated)
{
	vec poly1CrossVector1;
	vec poly1CrossVector2;
	SHAREDvector.subtractVectors(&poly1CrossVector1, &(polyReferenceRotated->vertex1relativePosition), &(polyReferenceRotated->vertex2relativePosition));
	SHAREDvector.subtractVectors(&poly1CrossVector2, &(polyReferenceRotated->vertex1relativePosition), &(polyReferenceRotated->vertex3relativePosition));
	vec poly2CrossVector1;
	vec poly2CrossVector2;
	SHAREDvector.subtractVectors(&poly2CrossVector1, &(currentReferenceInPartRotated->vertex1relativePosition), &(currentReferenceInPartRotated->vertex2relativePosition));
	SHAREDvector.subtractVectors(&poly2CrossVector2, &(currentReferenceInPartRotated->vertex1relativePosition), &(currentReferenceInPartRotated->vertex3relativePosition));
	vec poly1Normal; 
	vec poly2Normal; 
	SHAREDvector.calculateNormal(&poly1CrossVector1, &poly1CrossVector2, &poly1Normal);
	SHAREDvector.calculateNormal(&poly2CrossVector1, &poly2CrossVector2, &poly2Normal);
	vec p1centre;
	p1centre.x = 0.0;
	p1centre.y = 0.0;
	p1centre.z = 0.0;
	double angleBetweenNormalsOfPolys = SHAREDvector.calculateInteriorAngleOfAPolygonVertex(&p1centre, &poly1Normal, &poly2Normal);
	
	//crazy angle check (parallel vectors);
	
	angleBetweenNormalsOfPolys = (angleBetweenNormalsOfPolys/PI* 180.0);

	//cout << "\tangleBetweenNormalsOfPolys = " << angleBetweenNormalsOfPolys << endl;
	/*	
	cout << polyReferenceRotated->type 
		<< " " << polyReferenceRotated->colour 
		<< " " << polyReferenceRotated->vertex1relativePosition.x 
		<< " " << polyReferenceRotated->vertex1relativePosition.y 
		<< " " << polyReferenceRotated->vertex1relativePosition.z 
		<< " " << polyReferenceRotated->vertex2relativePosition.x 
		<< " " << polyReferenceRotated->vertex2relativePosition.y 
		<< " " << polyReferenceRotated->vertex2relativePosition.z 
		<< " " << polyReferenceRotated->vertex3relativePosition.x 
		<< " " << polyReferenceRotated->vertex3relativePosition.y 
		<< " " << polyReferenceRotated->vertex3relativePosition.z 
		<< " " << polyReferenceRotated->vertex4relativePosition.x 
		<< " " << polyReferenceRotated->vertex4relativePosition.y 
		<< " " << polyReferenceRotated->vertex4relativePosition.z << endl; 
		cout << currentReferenceInPartRotated->type 
		<< " " << currentReferenceInPartRotated->colour 
		<< " " << currentReferenceInPartRotated->vertex1relativePosition.x 
		<< " " << currentReferenceInPartRotated->vertex1relativePosition.y 
		<< " " << currentReferenceInPartRotated->vertex1relativePosition.z 
		<< " " << currentReferenceInPartRotated->vertex2relativePosition.x 
		<< " " << currentReferenceInPartRotated->vertex2relativePosition.y 
		<< " " << currentReferenceInPartRotated->vertex2relativePosition.z 
		<< " " << currentReferenceInPartRotated->vertex3relativePosition.x 
		<< " " << currentReferenceInPartRotated->vertex3relativePosition.y 
		<< " " << currentReferenceInPartRotated->vertex3relativePosition.z 
		<< " " << currentReferenceInPartRotated->vertex4relativePosition.x 
		<< " " << currentReferenceInPartRotated->vertex4relativePosition.y 
		<< " " << currentReferenceInPartRotated->vertex4relativePosition.z << endl; 
	*/

	angleBetweenNormalsOfPolys = SHAREDvars.absDouble(angleBetweenNormalsOfPolys);

	angleBetweenNormalsOfPolys = 180.0 - angleBetweenNormalsOfPolys;
	angleBetweenNormalsOfPolys = SHAREDvars.absDouble(angleBetweenNormalsOfPolys);
	/*
	if(angleBetweenNormalsOfPolys < 0.0)
	{
		angleBetweenNormalsOfPolys = 0.0;
	}
	*/
	return angleBetweenNormalsOfPolys;

}


LDreference* OLCClass::fillEdgeLine(vec* polyReferenceVertexA, vec* polyReferenceVertexB, LDreference* currentReferenceInOptionalLineData, LDreference* initialReferenceInOptionalLineData, bool* result)
{
	LDreference* modCurrentReferenceInOptionalLineData;
	bool currentFunctionResult = false;

	LDreference edgeLineReference;
	//determine optional line required;
	edgeLineReference.type = REFERENCE_TYPE_LINE;
	edgeLineReference.colour = DAT_FILE_DEFAULT_COLOUR_EDGELINE;
	SHAREDvector.copyVectors(&(edgeLineReference.vertex1relativePosition), polyReferenceVertexA);
	SHAREDvector.copyVectors(&(edgeLineReference.vertex2relativePosition), polyReferenceVertexB);

		//tertiaryConditions check/////////////////////////////////////////////////////////////
	bool tertiaryConditions = true;

	
	if(this->search1DrefListFindAnyLikeEdgeLineRef(&edgeLineReference, initialReferenceInOptionalLineData))
	{
		//delete optionalLineReference;	//not necessary
		tertiaryConditions = false;
		alreadyaddededgelinesfound++;
	}

	
	bool removeRes = false;
	this->search1DrefListRemoveAnyLikeOptionalLineRef(&edgeLineReference, initialReferenceInOptionalLineData, &removeRes);

	if(removeRes == true)
	{
		//cout << "removed a ref" <<endl;
	}
	
	

	if(tertiaryConditions == true)
	{
		totalNumEdgeLines++;
		//cout << "totalNumEdgeLines = " << totalNumEdgeLines << endl;

		//copy edgeLineReference into currentReferenceInOptionalLineData
		LDreferenceManipulation.copyReferences(currentReferenceInOptionalLineData, &edgeLineReference, REFERENCE_TYPE_LINE);

		//template code to create a new reference In Optional Line Data
		LDreference* nextReference = new LDreference();
		currentReferenceInOptionalLineData -> next = nextReference;
		currentReferenceInOptionalLineData = currentReferenceInOptionalLineData->next;
		currentFunctionResult = true;
	}



	modCurrentReferenceInOptionalLineData = currentReferenceInOptionalLineData;
	*result = currentFunctionResult;
	return modCurrentReferenceInOptionalLineData;
}


LDreference* OLCClass::fillOptionalLine(LDreference* polyReferenceRotated, LDreference* currentReferenceInPartRotated, LDreference* currentReferenceInOptionalLineData, const LDreference* initialReferenceInOptionalLineData, bool* result)
{
	LDreference* modCurrentReferenceInOptionalLineData;
	bool currentFunctionResult = false;

	LDreference optionalLineReference;
	//determine optional line required;
	optionalLineReference.type = REFERENCE_TYPE_OPTIONALLINE;
	optionalLineReference.colour = DAT_FILE_DEFAULT_COLOUR_EDGELINE;
	SHAREDvector.copyVectors(&(optionalLineReference.vertex1relativePosition), &(polyReferenceRotated->vertex1relativePosition));
	SHAREDvector.copyVectors(&(optionalLineReference.vertex2relativePosition), &(polyReferenceRotated->vertex2relativePosition));
	//add optional specific vertex 1 here now based on the 3D angles of the two adjacent polygons; polyReferenceRotated and currentReferenceInPart
	//add optional specific vertex 2 here now based on the 3D angles of the two adjacent polygons; polyReferenceRotated and currentReferenceInPart
	
	if(polyReferenceRotated->type == REFERENCE_TYPE_QUAD)
	{
		SHAREDvector.copyVectors(&(optionalLineReference.vertex3relativePosition), &(polyReferenceRotated->vertex3relativePosition));	//or polyReferenceRotated->vertex4relativePosition
	}
	else if(polyReferenceRotated->type == REFERENCE_TYPE_TRI)
	{
		SHAREDvector.copyVectors(&(optionalLineReference.vertex3relativePosition), &(polyReferenceRotated->vertex3relativePosition));
	}

	if(currentReferenceInPartRotated->type == REFERENCE_TYPE_QUAD)
	{
		SHAREDvector.copyVectors(&(optionalLineReference.vertex4relativePosition), &(currentReferenceInPartRotated->vertex3relativePosition));	//or currentReferenceInPart->vertex4relativePosition
	}
	else if(currentReferenceInPartRotated->type == REFERENCE_TYPE_TRI)
	{
		SHAREDvector.copyVectors(&(optionalLineReference.vertex4relativePosition), &(currentReferenceInPartRotated->vertex3relativePosition));
	}

	
		//tertiaryConditions check/////////////////////////////////////////////////////////////
	bool tertiaryConditions = true;

	if(this->search1DrefListFindAnyLikeLineRef(&optionalLineReference, initialReferenceInOptionalLineData))
	{
		//delete optionalLineReference;	//not necessary
		tertiaryConditions = false;
		alreadyaddedoptionallinesfound++;
	}

		//quadaryConditions check/////////////////////////////////////////////////////////////
	bool quadaryConditions = true;
#ifdef OLD_COLINEAR_OLS_CHECK	/*[make sure these checks are in right/optimum place]*/
	/*
	cout << "(optionalLineReference.vertex1relativePosition).x = " << (optionalLineReference.vertex1relativePosition).x << endl;
	cout << "(optionalLineReference.vertex1relativePosition).y = " << (optionalLineReference.vertex1relativePosition).y << endl;
	cout << "(optionalLineReference.vertex2relativePosition).x = " << (optionalLineReference.vertex2relativePosition).x << endl;
	cout << "(optionalLineReference.vertex2relativePosition).y = " << (optionalLineReference.vertex2relativePosition).y << endl;
	*/
	if(OLCoperations.edgeLinesVerticiesAndOLverticsOnSamePlane(&(optionalLineReference.vertex1relativePosition), &(optionalLineReference.vertex2relativePosition), &(optionalLineReference.vertex3relativePosition), &(optionalLineReference.vertex4relativePosition)))
	{
		//delete optionalLineReference;	//not necessary
		quadaryConditions = false;

		bool removeRes = false;
		this->search1DrefListRemoveAnyLikeOptionalLineRef(&optionalLineReference, initialReferenceInOptionalLineData, &removeRes);

		if(removeRes == true)
		{
			//cout << "removed a ref" <<endl;
		}

		edgeAndOptionalLineOnSamePlaneFound++;
	}
#endif

		////////////////////////////////////////////////////////////////////////////////////////

	if((tertiaryConditions == true) && (quadaryConditions == true))
	{
		totalNumOptionalLines++;
		//cout << "totalNumOptionalLines = " << totalNumOptionalLines << endl;

		//copy optionalLineReference into currentReferenceInOptionalLineData
		LDreferenceManipulation.copyReferences(currentReferenceInOptionalLineData, &optionalLineReference, REFERENCE_TYPE_OPTIONALLINE);

		//template code to create a new reference In Optional Line Data
		LDreference* nextReference = new LDreference();
		currentReferenceInOptionalLineData -> next = nextReference;
		currentReferenceInOptionalLineData = currentReferenceInOptionalLineData->next;
		currentFunctionResult = true;
	}

	modCurrentReferenceInOptionalLineData = currentReferenceInOptionalLineData;
	*result = currentFunctionResult;
	return modCurrentReferenceInOptionalLineData;
}





void OLCClass::search1DrefListRemoveRef(const LDreference* referenceToFindAndRemove, LDreference* initialReferenceInList, bool* result)
{

	LDreference* currentReference = initialReferenceInList;
	LDreference* previousReference = NULL;

	while(currentReference->next != NULL)
	{
		bool refFoundInCurrentReference = false;

		if(LDreferenceManipulation.compareReferences(currentReference, referenceToFindAndRemove, referenceToFindAndRemove->type))
		{
			*result = true;
			refFoundInCurrentReference = true;

			if(previousReference != NULL)
			{
				previousReference->next = currentReference->next;
			}
			else
			{
				cout << "illegal - optional line reference list must begin with a fake reference to avoid this" << endl;
				exit(0);
			}
		}

		previousReference = currentReference;
		currentReference = currentReference->next;

		if(refFoundInCurrentReference == true)
		{
			//delete (previousReference);	//remove found unit from reference tree
		}
	}

}

void OLCClass::search1DrefListRemoveAnyLikeOptionalLineRef(const LDreference* referenceToFindAndRemove, LDreference* initialReferenceInList, bool* result)
{

	LDreference* currentReference = initialReferenceInList;
	LDreference* previousReference = NULL;

	while(currentReference->next != NULL)
	{
		bool refFoundInCurrentReference = false;

		if(this->compareAnyLikeOptionalLineReferences(currentReference, referenceToFindAndRemove))
		{
			*result = true;
			refFoundInCurrentReference = true;

			if(previousReference != NULL)
			{
				previousReference->next = currentReference->next;
			}
			else
			{
				cout << "illegal - optional line reference list must begin with a fake reference to avoid this" << endl;
				exit(0);
			}
		}
		else
		{
			previousReference = currentReference;
		}

			
		const LDreference* oldcurrref = currentReference;
		currentReference = currentReference->next;

		if(refFoundInCurrentReference == true)
		{
			//delete (oldcurrref);	//remove found unit from reference tree
		}
	}

}

bool OLCClass::search1DrefListFindAnyLikeLineRef(const LDreference* referenceToFind, const LDreference* initialReferenceInList)
{
	bool foundReference = false;

	const LDreference* currentReference = initialReferenceInList;

	while(currentReference->next != NULL)
	{
		if(this->compareAnyLikeLineReferences(currentReference, referenceToFind))
		{
			foundReference = true;
		}

		currentReference = currentReference->next;
	}

	return foundReference;
}



bool OLCClass::compareAnyLikeLineReferences(const LDreference* reference1, const LDreference* reference2)
{
	bool result = true;

	/*
	if((reference1->type == REFERENCE_TYPE_LINE) || (reference1->type == REFERENCE_TYPE_OPTIONALLINE))
	{
	*/
		if(!((SHAREDvector.compareVectors(&(reference1->vertex1relativePosition),  &(reference2->vertex1relativePosition)) && SHAREDvector.compareVectors(&(reference1->vertex2relativePosition),  &(reference2->vertex2relativePosition))) || (SHAREDvector.compareVectors(&(reference1->vertex1relativePosition),  &(reference2->vertex2relativePosition)) && SHAREDvector.compareVectors(&(reference1->vertex2relativePosition),  &(reference2->vertex1relativePosition)))))
		{
			result = false;
		}
	/*
	}
	else
	{
		result = false;
	}
	*/

	return result;
}



/*
bool search1DrefListFindAnyLikeOptionalLineRef(LDreference* referenceToFind, LDreference* initialReferenceInList)
{
	bool foundReference = false;

	LDreference* currentReference = initialReferenceInList;

	while(currentReference->next != NULL)
	{
		if(compareAnyLikeOptionalLineReferences(currentReference, referenceToFind))
		{
			foundReference = true;
		}

		currentReference = currentReference->next;
	}

	return foundReference;
}

*/

bool OLCClass::search1DrefListFindAnyLikeEdgeLineRef(const LDreference* referenceToFind, const LDreference* initialReferenceInList)
{
	bool foundReference = false;

	const LDreference* currentReference = initialReferenceInList;

	while(currentReference->next != NULL)
	{
		if(this->compareAnyLikeEdgeLineReferences(currentReference, referenceToFind))
		{
			foundReference = true;
		}

		currentReference = currentReference->next;
	}

	return foundReference;
}



bool OLCClass::compareAnyLikeOptionalLineReferences(const LDreference* reference1, const LDreference* reference2)
{
	bool result = true;

	if(reference1->type == REFERENCE_TYPE_OPTIONALLINE)
	{
		if(!((SHAREDvector.compareVectors(&(reference1->vertex1relativePosition),  &(reference2->vertex1relativePosition)) && SHAREDvector.compareVectors(&(reference1->vertex2relativePosition),  &(reference2->vertex2relativePosition))) || (SHAREDvector.compareVectors(&(reference1->vertex1relativePosition),  &(reference2->vertex2relativePosition)) && SHAREDvector.compareVectors(&(reference1->vertex2relativePosition),  &(reference2->vertex1relativePosition)))))
		{
			result = false;
		}
	}
	else
	{
		result = false;
	}

	return result;
}


bool OLCClass::compareAnyLikeEdgeLineReferences(const LDreference* reference1, const LDreference* reference2)
{
	bool result = true;

	if(reference1->type == REFERENCE_TYPE_LINE)
	{
		if(!((SHAREDvector.compareVectors(&(reference1->vertex1relativePosition),  &(reference2->vertex1relativePosition)) && SHAREDvector.compareVectors(&(reference1->vertex2relativePosition),  &(reference2->vertex2relativePosition))) || (SHAREDvector.compareVectors(&(reference1->vertex1relativePosition),  &(reference2->vertex2relativePosition)) && SHAREDvector.compareVectors(&(reference1->vertex2relativePosition),  &(reference2->vertex1relativePosition)))))
		{
			result = false;
		}
	}
	else
	{
		result = false;
	}


	return result;
}







