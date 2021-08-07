/*******************************************************************************
 *
 * File Name: OLC.hpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2021 Baxter AI (baxterai.com)
 * Project: LD Optional Lines Creator (Calculator)
 * Project Version: 1d1a 07-August-2021 (github.com/baxterai/OLC) (automatic source upgrades: generate const / generate OO)
 * First Public Release: 1b9b 12-June-2007 (https://www.philohome.com/isecalc/olc.htm) 
 * Second Public Release: 1c1b 19-Sept-2009 (https://www.baxterai.com/utils/misc/OLC/OLC.html) (part file white space/syntax requirement relaxation, Added UI option to create edge line instead of optional line where the angle between the two intersecting polygons is less than x degrees)
 * Comments: NB - no recursive parsing.
 *
 *******************************************************************************/

#include "LDreferenceClass.hpp"
#include "OLCoperations.hpp"
#include "LDparser.hpp"
#include "SHAREDvector.hpp"
#include "LDreferenceManipulation.hpp"
//#include "SHAREDvars.hpp"

#define OPTIONAL_LINE_DATA_HEADER "0 \n0 Optional Line Data (generated using OLC.exe):\n0 \n"

//#define OLD_COLINEAR_OLS_CHECK

int main(const int argc,const char* *argv);

class OLCClass
{
	private: LDparserClass LDparser;
	private: LDreferenceManipulationClass LDreferenceManipulation;
	private: SHAREDvectorClass SHAREDvector;
	private: OLCoperationsClass OLCoperations;
	private: SHAREDvarsClass SHAREDvars;

	public: bool createPartWithOptionalLines(string partFileOriginalName, const string partFileModifiedName, bool createEdges, float edgeThreshold);

	private: bool partOptionalLinesFileOutput(const string partFileModifiedName, const string partFileOriginalName, LDreference* initialReferenceInPart, bool createEdges, float edgeThreshold);

/***************************** PART 1* **********************************************/
	private: void createReferenceListOfOptionalLines(LDreference* initialReferenceInPart, LDreference* initialReferenceInOptionalLineData, bool createEdges, float edgeThreshold);
		private: LDreference* addOptionalLineReferencesForQuad(LDreference* quadReference, LDreference* currentReferenceInOptionalLineData, LDreference* initialReferenceInPart, LDreference* initialReferenceInOptionalLineData, bool* result, bool createEdges, float edgeThreshold);
		private: LDreference* addOptionalLineReferencesForTri(LDreference* triReference, LDreference* currentReferenceInOptionalLineData, LDreference* initialReferenceInPart, LDreference* initialReferenceInOptionalLineData, bool* result, bool createEdges, float edgeThreshold);
			private: LDreference* findCorrespondingPrimRefEdgeAndAddOptionalLine(const LDreference* polyReference, LDreference* polyReferenceRotated, LDreference* currentReferenceInOptionalLineData, LDreference* initialReferenceInPart, LDreference* initialReferenceInOptionalLineData, bool* result, bool createEdges, float edgeThreshold);
				private: LDreference* fillEdgeOrOptionalLine(LDreference* polyReferenceRotated, LDreference* currentReferenceInPartRotated, LDreference* currentReferenceInOptionalLineData, LDreference* initialReferenceInOptionalLineData, bool* result, const bool createEdges, const float edgeThreshold);
					private: LDreference* fillOptionalLine(LDreference* polyReferenceRotated, LDreference* currentReferenceInPartRotated, LDreference* currentReferenceInOptionalLineData, const LDreference* initialReferenceInOptionalLineData, bool* result);
					private: LDreference* fillEdgeLine(vec* polyReferenceVertexA, vec* polyReferenceVertexB, LDreference* currentReferenceInOptionalLineData, LDreference* initialReferenceInOptionalLineData, bool* result);
					private: double calculateAngleBetweenNormalsOfPolys(LDreference* polyReferenceRotated, LDreference* currentReferenceInPartRotated);
						private: bool search1DrefListFindAnyLikeLineRef(const LDreference* referenceToFind, const LDreference* initialReferenceInList);
							private: bool compareAnyLikeLineReferences(const LDreference* reference1, const LDreference* reference2);
					//these should be shifted elsewhere
						private: void search1DrefListRemoveRef(const LDreference* referenceToFindAndRemove, LDreference* initialReferenceInList, bool* result);
						private: void search1DrefListRemoveAnyLikeOptionalLineRef(const LDreference* referenceToFindAndRemove, LDreference* initialReferenceInList, bool* result);
	private: bool compareAnyLikeOptionalLineReferences(const LDreference* reference1, const LDreference* reference2);
	private: bool compareAnyLikeEdgeLineReferences(const LDreference* reference1, const LDreference* reference2);
	private: bool search1DrefListFindAnyLikeEdgeLineRef(const LDreference* referenceToFind, const LDreference* initialReferenceInList);
};
