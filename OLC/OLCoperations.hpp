/*******************************************************************************
 *
 * File Name: OLCoperations.hpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2021 Baxter AI (baxterai.com)
 * Project: LD Optional Lines Creator (Calculator)
 * Project Version: 1d1a 07-August-2021 (github.com/baxterai/OLC) (automatic source upgrades: generate const / generate OO)
 * First Public Release: 1b9b 12-June-2007 (https://www.philohome.com/isecalc/olc.htm) 
 * Second Public Release: 1c1b 19-Sept-2009 (https://www.baxterai.com/utils/misc/OLC/OLC.html) (part file white space/syntax requirement relaxation, Added UI option to create edge line instead of optional line where the angle between the two intersecting polygons is less than x degrees)
 * Comments: NB - no recursive parsing.
 *
 *******************************************************************************/

#include "SHAREDvars.hpp"
#include "SHAREDglobalDefs.hpp"
#include "SHAREDvector.hpp"

class OLCoperationsClass
{
	private: SHAREDvarsClass SHAREDvars;
	private: SHAREDvectorClass SHAREDvector;
	public: bool edgeLinesVerticiesAndOLverticsOnSamePlane(vec* edgeLineVertex1, const vec* edgeLineVertex2, vec* optionalLineVertex1, const vec* optionalLineVertex2);
};


