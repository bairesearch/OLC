/*******************************************************************************
 *
 * File Name: OLCoperations.cpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2021 Baxter AI (baxterai.com)
 * Project: LD Optional Lines Creator (Calculator)
 * Project Version: 1d1a 07-August-2021 (github.com/baxterai/OLC) (automatic source upgrades: generate const / generate OO)
 * First Public Release: 1b9b 12-June-2007 (https://www.philohome.com/isecalc/olc.htm) 
 * Second Public Release: 1c1b 19-Sept-2009 (https://www.baxterai.com/utils/misc/OLC/OLC.html) (part file white space/syntax requirement relaxation, Added UI option to create edge line instead of optional line where the angle between the two intersecting polygons is less than x degrees)
 * Comments: NB - no recursive parsing.
 *
 *******************************************************************************/

#include "OLCoperations.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>





bool OLCoperationsClass::edgeLinesVerticiesAndOLverticsOnSamePlane(vec* edgeLineVertex1, const vec* edgeLineVertex2, vec* optionalLineVertex1, const vec* optionalLineVertex2)
{
	bool result;

	double xyXInt, zxXInt, xyYInt, yzYInt, yzZInt, zxZInt;

	bool xyinterceptionFound, xyinterceptionPointFound, yzinterceptionFound, yzinterceptionPointFound, zxinterceptionFound, zxinterceptionPointFound;

	xyinterceptionFound = false;
	xyinterceptionPointFound = false;
	yzinterceptionFound = false;
	yzinterceptionPointFound = false;
	zxinterceptionFound = false;
	zxinterceptionPointFound = false;

	/*
	if(totalNumOptionalLines == 12)
	{
		cout << "xy plane" << endl;
	}
	*/
	SHAREDvector.find2DintersectionPoint(edgeLineVertex1->x, edgeLineVertex1->y, optionalLineVertex1->x, optionalLineVertex1->y, edgeLineVertex2->x, edgeLineVertex2->y, optionalLineVertex2->x, optionalLineVertex2->y, &xyXInt, &xyYInt, &xyinterceptionFound, &xyinterceptionPointFound);
	//xy plane

	/*
	if(totalNumOptionalLines == 12)
	{
		cout << "yz plane" << endl;
	}
	*/
	SHAREDvector.find2DintersectionPoint(edgeLineVertex1->y, edgeLineVertex1->z, optionalLineVertex1->y, optionalLineVertex1->z, edgeLineVertex2->y, edgeLineVertex2->z, optionalLineVertex2->y, optionalLineVertex2->z, &yzYInt, &yzZInt, &yzinterceptionFound, &yzinterceptionPointFound);
	//yz plane

	/*
	if(totalNumOptionalLines == 12)
	{
		cout << "zx plane" << endl;
	}
	*/
	SHAREDvector.find2DintersectionPoint(edgeLineVertex1->z, edgeLineVertex1->x, optionalLineVertex1->z, optionalLineVertex1->x, edgeLineVertex2->z, edgeLineVertex2->x, optionalLineVertex2->z, optionalLineVertex2->x, &zxZInt, &zxXInt, &zxinterceptionFound, &zxinterceptionPointFound);
	//zx plane

	/*
	if(totalNumOptionalLines == 12)
	{
		cout << "\nDEBUG: "<< endl;
		cout << "edgeLineVertex1->x = " << edgeLineVertex1->x << endl;
		cout << "edgeLineVertex1->y = " << edgeLineVertex1->y << endl;
		cout << "edgeLineVertex1->z = " << edgeLineVertex1->z << endl;
		cout << "edgeLineVertex2->x = " << edgeLineVertex2->x << endl;
		cout << "edgeLineVertex2->y = " << edgeLineVertex2->y << endl;
		cout << "edgeLineVertex2->z = " << edgeLineVertex2->z << endl;
		cout << "optionalLineVertex1->x = " << optionalLineVertex1->x << endl;
		cout << "optionalLineVertex1->y = " << optionalLineVertex1->y << endl;
		cout << "optionalLineVertex1->z = " << optionalLineVertex1->z << endl;
		cout << "optionalLineVertex2->x = " << optionalLineVertex2->x << endl;
		cout << "optionalLineVertex2->y = " << optionalLineVertex2->y << endl;
		cout << "optionalLineVertex2->z = " << optionalLineVertex2->z << endl;
		cout << "xyinterceptionFound = " << xyinterceptionFound << endl;
		cout << "xyinterceptionPointFound = " << xyinterceptionPointFound << endl;
		cout << "yzinterceptionFound = " << yzinterceptionFound << endl;
		cout << "yzinterceptionPointFound = " << yzinterceptionPointFound << endl;
		cout << "zxinterceptionFound = " << zxinterceptionFound << endl;
		cout << "zxinterceptionPointFound = " << zxinterceptionPointFound << endl;
		cout << "xyXInt = " << xyXInt << endl;
		cout << "zxXInt = " << zxXInt << endl;
		cout << "xyYInt = " << xyYInt << endl;
		cout << "yzYInt = " << yzYInt << endl;
		cout << "yzZInt = " << yzZInt << endl;
		cout << "zxZInt = " << zxZInt << endl;
	}
	*/

	if(xyinterceptionFound && yzinterceptionFound && zxinterceptionFound)
	{
		result = true;

		if(xyinterceptionPointFound && zxinterceptionPointFound)
		{
			if(!SHAREDvars.compareDoubles(xyXInt, zxXInt))
			{//xy plane
				result = false;
			}
			else
			{
				//cout << "DEBUG: X intersection on xy plane = X intersection on zx plane detected" << endl;
			}
		}
		else
		{
			//cout << "DEBUG: X intersection on xy plane = X intersection on zx plane detected" << endl;
		}

		if(xyinterceptionPointFound && yzinterceptionPointFound)
		{
			if(!SHAREDvars.compareDoubles(xyYInt, yzYInt))
			{//yz plane
				result = false;
			}
			else
			{
				//cout << "DEBUG: Y intersection on xy plane = Y intersection on yz plane detected" << endl;
			}
		}
		else
		{
			//cout << "DEBUG: Y intersection on xy plane = Y intersection on yz plane detected" << endl;
		}

		if(yzinterceptionPointFound && zxinterceptionPointFound)
		{
			if(!SHAREDvars.compareDoubles(yzZInt, zxZInt))
			{//zx plane
				result = false;
			}
			else
			{
				//cout << "DEBUG: Z intersection on yz plane = Z intersection on zx plane detected" << endl;
			}
		}
		else
		{
			//cout << "DEBUG: Z intersection on yz plane = Z intersection on zx plane detected" << endl;
		}
	}
	else
	{
		result = false;
	}


	if(result == true)
	{
		/*
		if(totalNumOptionalLines == 12)
		{
			cout << "\nIntersection on all planes detected\n" << endl;
		}
		*/
	}

	//cout << "" << endl;

	return result;
}
