#include "GPxxx.h"
#include <strTools.h>


bool	checkLatDeg(int degrees) { return (degrees>=0 && degrees<90); }

bool	checkLonDeg(int degrees) { return (degrees>=0 && degrees<180); }

bool	checkMin(float minutes) { return (minutes>=0 && minutes<60); }



// **********************************************
// ****************  gPosition  *****************
// **********************************************


gPosition::gPosition(void) {

	latDeg	= 48;
	latMin	= 29.41;
	latQuad	= north;
	lonDeg	= 122;
	lonMin	= 39.72;
	lonQuad	= west;
}


gPosition::~gPosition(void) {  }


// Make sure everything is in spec.x	
bool gPosition::valid(void) {

	if (!checkLatDeg(latDeg))	return false;
	if (!checkMin(latMin))		return false;
	if (latQuad==north||latQuad==south) return false;
	if (!checkLonDeg(lonDeg))	return false;
	if (!checkMin(lonMin))		return false;
	if (lonQuad==east||lonQuad==west) return false;
	return true;
}
	

void gPosition::copyPos(gPosition* aLatLon) {

	if (aLatLon) {
		latDeg	= aLatLon->latDeg;
		latMin	= aLatLon->latMin;
		latQuad	= aLatLon->latQuad;
		lonDeg	= aLatLon->lonDeg;
		lonMin	= aLatLon->lonMin;
		lonQuad	= aLatLon->lonQuad;
	}
}

	
void gPosition::setLatValue(const char* inLatStr) {

	int		dotIndex;
	char*		latStr;                                      
	char		minStr[G_POS_BUFF_BYTES];
	
	latStr = NULL;																// ALWAYS initialize at NULL for this.
	if (heapStr(&latStr,inLatStr)) {										// If we can alocate a copy.
		dotIndex = 0;															// Starting at zero.
		while(latStr[dotIndex]!='.'&&latStr[dotIndex]!='\0') {	// Looking for the dot..
			dotIndex++;															// Cruise!
		}																			//
		if (latStr[dotIndex]=='.') {										// If we are pointing at the dot.
			if (dotIndex>2) {													// If we can back up a couple.                                                                                                                                                                                                                                                                                                     
				strcpy(minStr,&(latStr[dotIndex-2]));					// From here to end is the minutes.
				latStr[dotIndex-2] = '\0';									// Break the string where we started.
				latDeg = atoi(latStr);										// From start to new end is degrees.
				latMin = atof(minStr);										// minute string is already saved.
			}																		//
		}																			//
		freeStr(&latStr);														// Recycle the local string.
	}
}


void gPosition::setLatQuad(const char* inQuad) {

	char*		quadStr;
	
	quadStr = NULL;																// ALWAYS initialize at NULL for this.
	if (heapStr(&quadStr,inQuad)) {											// If we can alocate a copy.
		upCase(quadStr);															// Make it all uppercase.
		if (!strcmp("N",inQuad)||!strcmp("NORTH",inQuad)) {			// We'll take either of these.
			latQuad = north;														// Call it north.
		}else if (!strcmp("S",inQuad)||!strcmp("SOUTH",inQuad)) {	// Else we'll take either of these.
			latQuad = south;														// Calling them south.
		}																				//
		freeStr(&quadStr);														// Recycle the local string.
	}																					//
}
	
		 
void gPosition::setLonValue(const char* inLonStr) {

	int		dotIndex;
	char*		lonStr;                                      
	char		minStr[G_POS_BUFF_BYTES];
	
	lonStr = NULL;																// ALWAYS initialize at NULL for this.
	if (heapStr(&lonStr,inLonStr)) {										// If we can alocate a copy.
		dotIndex = 0;															// Starting at zero.
		while(lonStr[dotIndex]!='.'&&lonStr[dotIndex]!='\0') {	// Looking for the dot..
			dotIndex++;															// Cruise!
		}																			//
		if (lonStr[dotIndex]=='.') {										// If we are pointing at the dot.
			if (dotIndex>2) {													// If we can back up a couple.                                                                                                                                                                                                                                                                                                     
				strcpy(minStr,&(lonStr[dotIndex-2]));					// From here to end is the minutes.
				lonStr[dotIndex-2] = '\0';									// Break the string where we started.
				lonDeg = atoi(lonStr);										// From start to new end is degrees.
				lonMin = atof(minStr);										// minute string is already saved.
			}																		//
		}																			//
		freeStr(&lonStr);														// Recycle the local string.
	}
}



void gPosition::setLonQuad(const char* inQuad) {

	char*		quadStr;
	
	quadStr = NULL;															// ALWAYS initialize at NULL for this.
	if (heapStr(&quadStr,inQuad)) {										// If we can alocate a copy.
		upCase(quadStr);														// Make it all uppercase.
		if (!strcmp("E",inQuad)||!strcmp("EAST",inQuad)) {			// We'll take either of these.
			lonQuad = east;													// Call it north.
		}else if (!strcmp("W",inQuad)||!strcmp("WEST",inQuad)) {	// Else we'll take either of these.
			lonQuad = west;													// Calling them south.
		}																			//
		freeStr(&quadStr);													// Recycle the local string.
	}																				//
}


void gPosition::setPosValues(const char* latStr,const char* lonStr) {

	setLatValue(latStr);
	setLonValue(lonStr);
}

	
void gPosition::setQuads(const char*  inLatQuad,const char*  inLonQuad) {

	setLatQuad(inLatQuad);
	setLonQuad(inLonQuad);
}


void gPosition::setLatValue(int inLatDeg, float inLatMin) {

	if (checkLatDeg(inLatDeg)&&checkMin(inLatMin)) {
		latDeg = inLatDeg;
		latMin = inLatMin;
	}
}


void gPosition::setLatQuad(quad inLatQuad) {

	if (inLatQuad==north||inLatQuad==south) {
		latQuad = inLatQuad;
	}
}


void gPosition::setLonValue(int inLonDeg, float inLonMin) {

	if (checkLonDeg(inLonDeg)&&checkMin(inLonMin)) {
		lonDeg = inLonDeg;
		lonMin = inLonMin;
	}
}


void gPosition::setLonQuad(quad inLonQuad) {

	if (inLonQuad==east||inLonQuad==west) {
		lonQuad = inLonQuad;
	}
}


void gPosition::setQuads(quad inLatQuad,quad inLonQuad) {

	setLatQuad(inLatQuad);
	setLonQuad(inLonQuad);
}


void gPosition::setPosition(int inLatDeg, float inLatMin, quad inLatQuad, int inLonDeg, float inLonMin, quad inLonQuad) {

	setLatValue(inLatDeg,inLatMin);
	setLonValue(inLonDeg,inLonMin);
	setQuads(inLatQuad,inLonQuad);
}


float gPosition::trueBearingTo(gPosition* inDest) { 

	Serial.println("I have no idea how to do that.");
	return 0;
}


float gPosition::distanceTo(gPosition* inDest) {

	Serial.println("You're joking right?");
	return 0;
}


// These value to string methods end up with all this monkey motion because typical
// Arduinos don't do float to string conversions in the normal sprintf() ways. So I
// convert everything to ints and paste the string bits all together "by hand".

// This assumes the values are "good" IE in range. Otherwise you'll get wacky answers.
char* gPosition::getLatStr(void) {

	char	tempStr[G_POS_BUFF_BYTES];
	int	lMin;
	float	lMinDec;
	int	lMinOut;
	
	sprintf(tempStr,"%03u",latDeg);
	strcpy(outStr,tempStr);
	lMin = trunc(latMin);
	sprintf(tempStr,"%02u",lMin);
	strcat(outStr,tempStr);
	strcat(outStr,".");
	lMinDec = latMin - lMin;
	lMinOut = round(lMinDec * 100);
	sprintf(tempStr,"%02u",lMinOut);
	strcat(outStr,tempStr);
	return outStr;
}

	
char* gPosition::getLatQuadStr(void) {

	if (latQuad==north) {
		strcpy(outStr,"N");
	} else {
		strcpy(outStr,"S");
	}
	return outStr;
}


char* gPosition::getLonStr(void) {

	char	tempStr[G_POS_BUFF_BYTES];
	int	lMin;
	float	lMinDec;
	int	lMinOut;
	
	sprintf(tempStr,"%03u",lonDeg);
	strcpy(outStr,tempStr);
	lMin = trunc(lonMin);
	sprintf(tempStr,"%02u",lMin);
	strcat(outStr,tempStr);
	strcat(outStr,".");
	lMinDec = lonMin - lMin;
	lMinOut = round(lMinDec * 100);
	sprintf(tempStr,"%02u",lMinOut);
	strcat(outStr,tempStr);
	return outStr;
}


char* gPosition::getLonQuadStr(void) {
	
	if (lonQuad==east) {
		strcpy(outStr,"E");
	} else {
		strcpy(outStr,"W");
	}
	return outStr;
}
	

int gPosition::getLatDeg(void)	{ return latDeg; }

float gPosition::getLatMin(void)	{ return latMin; }

quad gPosition::getLatQuad(void)	{ return latQuad; }

int gPosition::getLonDeg(void)	{ return lonDeg; }

float gPosition::getLonMin(void)	{ return lonMin; }

quad gPosition::getLonQuad(void)	{ return lonQuad; }
				
				
				
// **********************************************
// **************** GPSInHandler ****************
// **********************************************

char	strBuff[40];


GPSInHandler::GPSInHandler(const char* inIDStr)
   : linkListObj(),
   numStreamIn() {
      
   IDStr = NULL;
   heapStr(&IDStr,inIDStr);
}


GPSInHandler::~GPSInHandler(void) { freeStr(&IDStr); }
   

bool GPSInHandler::handleStr(char* inID,GPSInStr* inGPSInStream) {

   if (!strcmp(inID,IDStr)) {
      copyStream(inGPSInStream);
      readErr = false;
      return true;
   }
   return false;
}


void GPSInHandler::stripChecksum(char* inStr) {

   int index;

   if (inStr) {
      index = 0;
      while(inStr[index]!='\0'&& inStr[index]!='*') index++;
      inStr[index] = '\0';
   } 
}


char*	GPSInHandler::quadToText(quad inQuad) {

	switch(inQuad) {
		case north	: strcpy(strBuff,"North"); break;
		case south	: strcpy(strBuff,"South"); break;
		case east	: strcpy(strBuff,"East"); break;
		case west	: strcpy(strBuff,"West"); break;
	}
	return strBuff;
}


char*	GPSInHandler::qualityToText(fixQuality inQual) {

	switch(inQual) {
		case fixInvalid	: strcpy(strBuff,"Fix invalid"); break;
		case fixByGPS	: strcpy(strBuff,"Fix by GPS"); break;
		case fixByDGPS	: strcpy(strBuff,"Fix by differential GPS"); break;
	}
	return strBuff;
}


void  GPSInHandler::showData(void) {  }


// **********************************************
// ****************   GPSInStr   ****************
// **********************************************


GPSInStr::GPSInStr(Stream* inPort,int tokenBuffBytes)
   : numStreamIn(inPort,tokenBuffBytes),
   idler() { currentHandler = NULL; }

 
GPSInStr::~GPSInStr(void) {  }

void GPSInStr::addHandler(GPSInHandler* inHandler) {

   if (inHandler) {
      hookup();
      handlers.addToEnd(inHandler);
   }
}


void GPSInStr::checkHandlers(char* inStr) {

   GPSInHandler* trace;
   bool        success;
   
   trace = (GPSInHandler*)handlers.getFirst();
   success = false;
   while(trace && !success) {
      success = trace->handleStr(inStr,this);
      if (success) {
         currentHandler = trace;
         return;
      }
      trace = (GPSInHandler*)trace->getNext();
   }
   if (!success) {
   	Serial.print("No handler for : ");
   	Serial.println(inStr);
   }
}


void GPSInStr::idle(void) {

   if (currentHandler) {
      currentHandler->readStream();
      if (!currentHandler->mSynk) {
         copyStream(currentHandler);
         currentHandler = NULL;
      }
   } else {
      readStream(); 
   }
}

void GPSInStr::readVar(int index,bool lastField) {

   if (index==0) {
      checkHandlers(mTokenBuff);
   }
}