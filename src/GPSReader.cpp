#include <GPSReader.h>
#include <strTools.h>

// **********************************************
// *************** GPSMsgHandler ****************
// **********************************************
//
// This is that base class of a NMEA sentence handler. $type,param,param...*x\n  in text.
// Gets all the calls in order. Then they can stuff the reader's variables with the data
// they read.
//

char	strBuff[40]; // For formatting output strings etc.


GPSMsgHandler::GPSMsgHandler(const char* inType,GPSReader* inReader) {

	typeStr = NULL;
	heapStr(&typeStr,inType);
	reader = inReader;
}


GPSMsgHandler::~GPSMsgHandler(void) { freeStr(&typeStr); }


void GPSMsgHandler::clearValues() {  }


bool GPSMsgHandler::canHandle(const char* inType) {

	if (!strcmp(typeStr,inType)) {	// If this is OUR string..
		clearValues();						// Reset our local variables
		readErr = false;					// Clear any previous errors.
		return true;						// Tell 'em we're good!
	}											//
	return false;							// Sorry, that ain't ours.
}


bool GPSMsgHandler::decodeParam(char* inParam,int paramIndex,bool lastParam) { return false; }


void GPSMsgHandler::stripChecksum(char* inStr) {

	int index;

   if (inStr) {
      index = 0;
      while(inStr[index]!='\0'&& inStr[index]!='*') index++;
      inStr[index] = '\0';
   } 
}



char* GPSMsgHandler::quadToText(quad inQuad) {

	switch(inQuad) {
		case north	: strcpy(strBuff,"North"); break;
		case south	: strcpy(strBuff,"South"); break;
		case east	: strcpy(strBuff,"East"); break;
		case west	: strcpy(strBuff,"West"); break;
	}
	return strBuff;
}


char* GPSMsgHandler::qualityToText(fixQuality inQual) {

	switch(inQual) {
		case fixInvalid	: strcpy(strBuff,"Fix invalid"); break;
		case fixByGPS		: strcpy(strBuff,"Fix by GPS"); break;
		case fixByDGPS		: strcpy(strBuff,"Fix by differential GPS"); break;
	}
	return strBuff;
}


void GPSMsgHandler::showData(void) { }
   
   
   
// **********************************************
// ****************     GPVTG    ****************
// **********************************************


GPVTG::GPVTG(GPSReader* inReader)
	: GPSMsgHandler("GPVTG",inReader)
	{  }
	
	
GPVTG::~GPVTG(void) {  }


void GPVTG::clearValues(void) {

	trueCourse				= 0;
	magCourse				= 0;
	groudSpeedKnots		= 0;
	groundSpeedKilos	= 0;
}


bool GPVTG::decodeParam(char* inParam,int paramIndex,bool lastParam) {

	if (!readErr) {
      switch(paramIndex) {
         case 1   : trueCourse = atof(inParam); 					break;
         case 2   : if (strcmp(inParam,"T")) readErr = true;	break;
         case 3   : magCourse = atof(inParam);						break;
         case 4   : if (strcmp(inParam,"M")) readErr = true;	break;
         case 5   : groudSpeedKnots = atof(inParam);				break;
         case 6   : if (strcmp(inParam,"N")) readErr = true;	break;
         case 7   : groundSpeedKilos = atof(inParam);				break;
         case 8   :
            stripChecksum(inParam);
            if (!strcmp(inParam,"K") && lastParam) {
               reader->trueCourse	=	trueCourse;
               reader->trueCourse	=	magCourse;
               reader->trueCourse	=	groudSpeedKnots;
               reader->trueCourse	=	groundSpeedKilos; 
            }
            showData();
         break;
      }
   }																							
	return !readErr;																		// Let 'em know how we're doin'.	
}

	
#ifdef SHOW_DATA          
void GPVTG::showData(void) {

	Serial.println("-------------------------------");
   Serial.print("True Course :\t");Serial.print(trueCourse);Serial.println("\tDegrees");
   Serial.print("Mag Course  :\t");Serial.print(magCourse);Serial.println("\tDegrees");
   Serial.print("Groud Speed :\t");Serial.print(groudSpeedKnots);Serial.println("\tKnots");
   Serial.print("Groud Speed :\t");Serial.print(groundSpeedKilos);Serial.println("\tKilometers per hour");
   Serial.println("-------------------------------");
}
#endif
   
// **********************************************
// ****************     GPGGA    ****************
// **********************************************



GPGGA::GPGGA(GPSReader* inReader)
	: GPSMsgHandler("GPGGA",inReader){  }
	

	
GPGGA::~GPGGA(void) {  }


void GPGGA::clearValues(void) {

	hours				= 0;
	min				= 0;
	sec				= 0;
	qualVal			= fixInvalid;
	numSatellites	= 0;
	HDOP				= 0;
	altitude			= 0;
	GeoidalHeight	= 0;
	ageOfDGPSData	= 0;
	DGPSStationID	= 0;
}


bool GPGGA::decodeParam(char* inParam,int paramIndex,bool lastParam) {
   
   char	buff[8];
   char*	ptr;
	int	rawQ;
	
   if (!readErr) {
      switch(paramIndex) {
         case 1	:
         	buff[0] = inParam[0];
         	buff[1] = inParam[1];
         	buff[2] = '\n';  
         	hours = atoi(buff);
         	buff[0] = inParam[2];
         	buff[1] = inParam[3];
         	min =  atoi(buff);
				ptr = &(inParam[4]);			// I think
				sec = atof(ptr);					// This one won't work.
			break;
			case 2	: latLon.setLatValue(inParam); break;
			case 3	: latLon.setLatQuad(inParam); break;
			case 4	: latLon.setLonValue(inParam); break;
			case 5	: latLon.setLonQuad(inParam); break;
			case 6	:
				rawQ = atoi(inParam);
				if (rawQ==0) {
					qualVal = fixInvalid;
				} else if (rawQ==1) {
					qualVal = fixByGPS;
				} else if (rawQ==2) {
					qualVal = fixByDGPS;
				} else {
					readErr = true;
				}
			break;
			case 7	: numSatellites = atoi(inParam); break;
			case 8	: HDOP = atof(inParam); break;
			case 9	: altitude = atof(inParam); break;
			case 10	: if (strcmp(inParam,"M"))	readErr = true; break;
			case 11	: GeoidalHeight = atof(inParam); break;
			case 12	: if (strcmp(inParam,"M"))	readErr = true; break;
			case 13	: ageOfDGPSData = atof(inParam); break;
			case 14	: 
				stripChecksum(inParam);
				DGPSStationID = atoi(inParam);
				if (lastParam) {
					reader->hours				= hours;
					reader->min					= min;
					reader->sec					= sec;
					reader->latLon.copyPos(&latLon);
					reader->qualVal			= qualVal;
					reader->numSatellites	= numSatellites;
					reader->HDOP				= HDOP;
					reader->altitude			= altitude;
					reader->GeoidalHeight	= GeoidalHeight;
					reader->ageOfDGPSData	= ageOfDGPSData;
					reader->DGPSStationID	= DGPSStationID;
					showData();
				}
			break;       
      }
   }																		//
	return !readErr;													// Let 'em know how we're doin'.
}


#ifdef SHOW_DATA
void GPGGA::showData(void) {
	
	char degStr[10];


	Serial.println("-------------------------------");
	Serial.print("Time :\t\t\t");Serial.print(hours);Serial.print(":");
	Serial.print(min);Serial.print(":");Serial.println(sec,2);
	sprintf(degStr,"%3u",latLon.getLatDeg());
	Serial.print("Lattitide :\t\t");Serial.print(degStr);Serial.print(" Deg.\t");
	Serial.print(latLon.getLatMin(),2);Serial.print(" Min.\t");
	Serial.println(latLon.getLatQuadStr());
	sprintf(degStr,"%3u",latLon.getLonDeg());
	Serial.print("Longitutde :\t\t");Serial.print(degStr);Serial.print(" Deg.\t");
	Serial.print(latLon.getLonMin(),2);Serial.print(" Min.\t");
	Serial.println(latLon.getLonQuadStr());
	Serial.print("Fix quality :\t\t");Serial.println(qualityToText(qualVal));
	Serial.print("Number of satellites :\t");Serial.println(numSatellites);
	Serial.print("HDOP value :\t\t");Serial.println(HDOP,2);		
	Serial.print("Altitiude : \t\t");Serial.print(altitude,1);Serial.println(" meters");
	Serial.print("Geoidal Height :\t");Serial.print(GeoidalHeight);Serial.println(" meters");
	Serial.print("Age of Diff. fix :\t");Serial.print(ageOfDGPSData,1);Serial.println(" seconds");
	Serial.print("Diff. station ID :\t");Serial.println(DGPSStationID);
	Serial.println("-------------------------------");
}
#endif


// **********************************************
// ****************     GPGSA    ****************
// **********************************************



GPGSA::GPGSA(GPSReader* inReader)
	: GPSMsgHandler("GPGSA",inReader) { }


GPGSA::~GPGSA(void) {  }


void GPGSA::clearValues(void) {

	operationMode	= manual;
	fixType			= noFix;
	for (int i=0;i<11;i++) {
		SVID[i] = 0;
	}
	PDOP				= 0;
	HDOP				= 0;
	VDOP				= 0;
}


bool GPGSA::decodeParam(char* inParam,int paramIndex,bool lastParam) {

	int	rawMode;
	
	if (!readErr) {
		if (paramIndex>=3 && paramIndex <=14) {
			SVID[paramIndex-3] = atoi(inParam);
			return true;										// I hope this is right..
		}
      switch(paramIndex) {
         case 1	:
				if (!strcmp(inParam,"M")) {
					operationMode = manual;
				} else if (!strcmp(inParam,"A")) {
					operationMode = automatic;
				} else {
					readErr = true;
				}
			break;
			case 2	:
				rawMode = atoi(inParam);
				if (rawMode==1) {
					fixType = noFix;
				} else if (rawMode==2) {
					fixType = twoD;
				} else if (rawMode==3) {
					fixType = threeD;
				} else  {
					readErr = true;
				}
			break;
			case 15	: PDOP = atof(inParam); break;
			case 16	: HDOP = atof(inParam); break;
			case 17	:
				stripChecksum(inParam);
				VDOP = atof(inParam);
				if (lastParam) {
					reader->operationMode	= operationMode;
					reader->fixType			= fixType;
					numSatellites	= 0;
					for(int i=0;i<11;i++) {
						reader->SVID[i] = SVID[i];
						if (SVID[i]) {
							numSatellites++;
						}
					}
					reader->numSatellites = numSatellites;
					reader->PDOP	= PDOP;
					reader->HDOP	= HDOP;
					reader->VDOP	= VDOP;
					showData();
				} else {
					readErr = true;
				}
			break;
		}
	}																		//
	return !readErr;													// Let 'em know how we're doin'.
}
				
		
#ifdef SHOW_DATA
void GPGSA::showData(void) {
   	
   Serial.println("-------------------------------");		
	Serial.print("Op Mode :\t");
	if (operationMode==manual) Serial.println("Manual");
	else Serial.println("Automatic");
	Serial.print( "Type of fix :\t");
	if (fixType==noFix) Serial.println("No Fix");
	else if (fixType==twoD) Serial.println("2D fix");
	else Serial.println("3D fix");
	Serial.print("Satellite count :\t");  Serial.println(numSatellites);
	Serial.print("Satellite IDs :\t");
	for(int i=0;i<11;i++) {
		if (SVID[i]) {
			Serial.print(SVID[i]);
		} else {
			Serial.print("..");
		}
		Serial.print("\t");
	}
	Serial.println();
	Serial.print("PDOP :\t\t");Serial.println(PDOP,1);
	Serial.print("HDOP :\t\t");Serial.println(HDOP,1);
	Serial.print("VDOP :\t\t");Serial.println(VDOP,1);
	Serial.println("-------------------------------");
}		
#endif

	
// **********************************************
// ****************     GPGSV    ****************
// **********************************************



// ****************    satData   ****************


/*
PRNNum;
elevation;
azimuth;
SigToNoise;
*/

satData::satData(void)
	: linkListObj() {  }


satData::satData(satData* copyData)
	: linkListObj() {
	
	PRNNum		= copyData->PRNNum;
	elevation	= copyData->elevation;
	azimuth		= copyData->azimuth;
	SigToNoise	= copyData->SigToNoise;
}

		
satData::~satData(void) {  }



// ****************     GPGSV    ****************


//GPS Satellites in view
GPGSV::GPGSV(GPSReader* inReader)
	: GPSMsgHandler("GPGSV",inReader) { setup(newMsgSet); }
	
	
GPGSV::~GPGSV(void) {  }


void GPGSV::setup(setupReasons reason) {

	switch (reason) {
		case newMsgSet		:					// Starting over fresh with message #1.
			numMsgs					= 0;		// Multiple messages in a set. How many to expect.
			msgNum					= 1;		// What message are we waiting for or working on?
			repeatNum				= 0;		// Multiple data sets in a message. What one we working on? In that message.
			numSatllites			= 0;		// Number of data sets to expect as a total of all messages.
			satNum					= 1;		// Which of the total data sets are we working on?
			workingSatList.dumpList();		// Local list of satellite data sets to be handed lover to the reader.
			tempData.PRNNum		= 0;		// Where we store the data set we are currently working on.
			tempData.elevation	= 0;
			tempData.azimuth		= 0;
			tempData.SigToNoise	= 0;
			readErr					= false;
		break;
		case newMsg			:					// Starting a new message but NOT #1.
			msgNum++;							// Bump up the msg number.
			repeatNum				= 0;		// No repeats yet on a new message.
			tempData.PRNNum		= 0;		// Where we store the data set we are currently working on.
			tempData.elevation	= 0;
			tempData.azimuth		= 0;
			tempData.SigToNoise	= 0;
		break;
		case newDataSet	:					// Starting a new data set on the current message.
			repeatNum++;						// Bump up to the next dataSet.
			tempData.PRNNum		= 0;		// Where we store the data set we are currently working on.
			tempData.elevation	= 0;
			tempData.azimuth		= 0;
			tempData.SigToNoise	= 0;
		break;
	}
}


int GPGSV::adjustedIndex(int paramIndex) {

	int offset;
	
	offset = 4 * repeatNum;
	return paramIndex - offset;
}


bool GPGSV::addNode(void) {

	satData*	newNode;

	newNode = new satData(&tempData);
	if (newNode) {
		workingSatList.addToEnd(newNode);
		return true;
	}
	return false;
}


void GPGSV::setComplete(void) {

	satData*	datPtr;
	
	if (!readErr) {
		datPtr = (satData*)workingSatList.getFirst();	// Get a pointer to the list.
		workingSatList.looseList();							// We're taking the list, let it go.
		reader->satInViewList.dumpList();					// Your list is stale, loose it.
		reader->satInViewList.addToEnd(datPtr);			// Take this fresh one.
	}
}


bool GPGSV::decodeParam(char* inParam,int paramIndex,bool lastParam) {
		
	
	switch (paramIndex) {									// Lets see what we have.
		case 1	:												// 
			if (msgNum==1) {									// Are we the first param of first message?
				numMsgs = atoi(inParam);					// First param sets number of messages in total.
				return true;									// Everything seems ok.
			} else if (numMsgs==atoi(inParam)) {		// First parameters match?
				return true;									// We're good. We're done.
			}														//
			readErr = true;									// Niether of these things are true? 
			setup(newMsgSet);									// Setup for the next time around.
			return false;										// Then fail.
		break;													//
		case 2	:												// Second parameter.
			if (atoi(inParam)==msgNum) {					// Second is message number. If it matches msgNum..	
				return true;									// Looks good, we're done here.
			}														//
			readErr = true;									// Didn't match? It's a fail.
			setup(newMsgSet);									// Setup for the next time around.
			return false;										// And we return the fail.
		break;													//
		case 3	:												// Third param.
			if (msgNum==1) {									// Are we the third param of first message?
				numSatllites = atoi(inParam);				// This gives the number of satellites.
				return true;									// W are OK so far.
			} else if (numSatllites==atoi(inParam)) {	// Not first message? Then total number of satellites needs to match.
				return true;									// Still doing ok.
			} else {												// Else we had a mismatch.
				readErr = true;								// That's a fail.
				setup(newMsgSet);								// Setup for the next time around.
				return false;									// We let them know.
			}														//
		break;													//
	}																//
	if (paramIndex>=4) {										// We have entered dataland..
		stripChecksum(inParam);								// If there is a checksum in there, loose it.
		switch(adjustedIndex(paramIndex)) {				// Start looping around an adjusted index.
			case 4	:											//
				tempData.PRNNum = atoi(inParam);			// First satellite data bit.  Seemingly ID/type value?
				return true;
			break;												//
			case 5	:											//
				tempData.elevation = atof(inParam);		// Altitude.
				return true;									// OK so far.
			break;												//
			case 6	:											//
				tempData.azimuth = atof(inParam);		// Azimuth.
				return true;									// Still in the running
			break;												//
			case 7	:											//
				tempData.SigToNoise = atoi(inParam);	// Signal to noise ratio.
				if (!addNode()) {								// If we CAN'T add a data node.
					setup(newMsgSet);							// Setup for the next time around.
					return false;								// Tell 'em it broke.
				}													//
				satNum++;										// Bump up the current number satellite number.
				if (lastParam) {								// If last param, this message is complete.
					if (satNum>numSatllites) {				// If we just encoded the last satellite.
						showData();								// Let debugging people see it before it's gone.
						setComplete();							// Push it all over to the GPS reader.
						setup(newMsgSet);						// Setup for the next time around.
						return true;							// return our complete success!
					} else {										// Else we have more to come.
						setup(newMsg);							// Setup for the next message of this set.
						return true;							// Return our current success, wait for next message in set.
					}												//
				} else {											// Else we are NOT done with this message.
					setup(newDataSet);						// Setup for more params.
					return true;								// Tell 'em were still here for them.
				}													//
			break;												//
		}															//
	}																//
	return false;												// There should be NO path to this line.
}	

		  
#ifdef SHOW_DATA				
void GPGSV::showData(void) {

	int		numItems;
	satData*	dataNode;
	
	numItems = workingSatList.getCount();
	Serial.println("-------------------------------");
	Serial.print("Satelites in view :\t");Serial.println(numItems);
	for (int i=0;i<numItems;i++) {
		dataNode = (satData*)workingSatList.getByIndex(i);
		if (dataNode) {
			Serial.print("Satellite :\t");Serial.println(dataNode->PRNNum);
			Serial.print("Elevation :\t");Serial.println(dataNode->elevation);
			Serial.print("Azmuth :\t");Serial.println(dataNode->azimuth);
			Serial.print("Sig / Noise :\t");Serial.println(dataNode->SigToNoise);
			Serial.println(".....");
		}
	}
	Serial.println("-------------------------------");
}
#endif				
				
				
// **********************************************
// ****************     GPRMC    ****************
// **********************************************


//Recommended minimum specific GPS/Transit data
GPRMC::GPRMC(GPSReader* inReader)
	: GPSMsgHandler("GPRMC",inReader) { }


GPRMC::~GPRMC(void) {  }


void GPRMC::clearValues(void) {

	hours					= 0;
	min					= 0;
	sec					= 0;
	valid					= false;
	groudSpeedKnots	= 0;
	trueCourse			= 0;
	year					= 0;
	month					= 0;
	day					= 0;
	magVar				= 0;
	vEastWest			= west;
	posMode				= notValid;
}


bool GPRMC::decodeParam(char* inParam,int paramIndex,bool lastParam) {

	char	temp[10];
	
	if (readErr) return false;											// Only do this if there have been readErr errors;       		
	switch(paramIndex) {
		case 1	:													// hhmmss.ss
			temp[0] = inParam[0];
			temp[1] = inParam[1];
			temp[2] = '\0';
			hours = atoi(temp);
			temp[0] = inParam[2];
			temp[1] = inParam[3];
			temp[2] = '\0';
			min = atoi(temp);
			strcpy(temp,&(inParam[4]));
			sec = atof(temp);
		break;
		case 2	:
			valid = false;
			if (!strcmp(inParam,"A")) {
				valid = true;
			}
		break;
		case 3	: latLon.setLatValue(inParam);		break;
		case 4	: latLon.setLatQuad(inParam);			break;
		case 5	: latLon.setLonValue(inParam);		break;
		case 6	: latLon.setLonQuad(inParam);			break;
		case 7	: groudSpeedKnots = atof(inParam);	break;
		case 8	: trueCourse = atof(inParam);			break;
		case 9	:													// ddmmyy
			temp[0] = inParam[0];
			temp[1] = inParam[1];
			temp[2] = '\0';
			day = atoi(temp);
			temp[0] = inParam[2];
			temp[1] = inParam[3];
			temp[2] = '\0';
			month = atoi(temp);
			temp[0] = inParam[4];
			temp[1] = inParam[5];
			temp[2] = '\0';
			year = atoi(temp);
		break;
		case 10	: magVar = atof(inParam); break;
		case 11	:
			upCase(inParam);
			vEastWest = west;
			if (!strcmp(inParam,"E")||!strcmp(inParam,"EAST")) {
				vEastWest = east;
			}
		break;
		case 12	:
			stripChecksum(inParam);
			upCase(inParam);
			switch(inParam[0]) {
				case 'A'	: posMode = Autonomous;		break;
				case 'D'	: posMode = Differential;	break;
				case 'E'	: posMode = Estimated;		break;
				case 'M'	: posMode = Manual;			break;
				case 'N'	: posMode = notValid;		break;
			}
			if (lastParam) {
				reader->hours				= hours;
				reader->min					= min;
				reader->sec					= sec;
				reader->valid				= valid;
				reader->groudSpeedKnots	= groudSpeedKnots;
				reader->trueCourse		= trueCourse;
				reader->year				= year;
				reader->month				= month;
				reader->day					= day;
				reader->magVar				= magVar;
				reader->vEastWest			= vEastWest;
				reader->posMode			= posMode;
				showData();
			} else {
				readErr = true;
			}
		break;
	}
	return !readErr;
}  


#ifdef SHOW_DATA
void GPRMC::showData(void) {

	char degStr[10];


	Serial.println("-------------------------------");
	Serial.println("Recommended minimum specific GPS/Transit data");
	Serial.print("Time :\t\t");Serial.print(hours);Serial.print(":");
	Serial.print(min);Serial.print(":");Serial.println(sec,2);
	Serial.print("Reading valid :\t");
	if (valid) {
		Serial.println("True");
	} else {
		Serial.println("False");
	}
	sprintf(degStr,"%3u",latLon.getLatDeg());
	Serial.print("Lattitide :\t");Serial.print(degStr);Serial.print(" Deg.\t");
	Serial.print(latLon.getLatMin(),2);Serial.print(" Min.\t");
	Serial.println(latLon.getLatQuadStr());
	sprintf(degStr,"%3u",latLon.getLonDeg());
	Serial.print("Longitutde :\t");Serial.print(degStr);Serial.print(" Deg.\t");
	Serial.print(latLon.getLonMin(),2);Serial.print(" Min.\t");
	Serial.println(latLon.getLonQuadStr());
	Serial.print("Groud Speed :\t");Serial.print(groudSpeedKnots);Serial.println("\tKnots");
	Serial.print("True Course :\t");Serial.print(trueCourse);Serial.println("\tDegrees");
  	Serial.print("Date :\t\t");Serial.print(month);Serial.print("/");
	Serial.print(day);Serial.print("/");Serial.println(year);
	Serial.print("Mag variation :\t");Serial.print(magVar);Serial.print("\t");
	if (vEastWest==east) {
		Serial.println("East");
	} else {
		Serial.println("West");
	}
	Serial.print("Position mode :\t");
	switch(posMode) {
		case	Autonomous		: Serial.println("Autonomous");		break;
		case	Differential	: Serial.println("Differential");	break;
		case	Estimated		: Serial.println("Estimated");		break;
		case	Manual			: Serial.println("Manual");			break;
		case	notValid			: Serial.println("notValid");			break;
	}
	Serial.println("-------------------------------");
}
#endif  
       
       
            
// **********************************************
// ****************   GPSReader  ****************
// **********************************************


GPSReader* ourGPS;



GPSReader::GPSReader(Stream* inStream,int tokenBuffBytes)
	: numStreamIn(inStream,tokenBuffBytes) {
	
	for (int i=0;i<NUM_HANDLERS;i++) {
		handlers[i] = NULL;
	}
	for (int i=0;i<11;i++) {
		SVID[i] = 0;
	}
	year					= 0;
   month					= 0;
	day					= 0;
            
	hours					= 0;
	min					= 0;
	sec					= 0;
            
	altitude				= 0;
	qualVal				= fixInvalid;
	valid					= false;
	posMode				= notValid;
            
	trueCourse			= 0;
	magCourse			= 0;
	groudSpeedKnots	= 0;
	groundSpeedKilos	= 0;
	magVar				= 0;
	vEastWest			= 'W';
	GeoidalHeight		= 0;
	ageOfDGPSData		= 0;
	DGPSStationID		= 0;
	operationMode		= automatic;
	fixType				= noFix;
	numSatellites		= 0;
	PDOP					= 0;
	HDOP					= 0;
	VDOP					= 0;
	reset();
}


GPSReader::~GPSReader(void) {
	
	for (int i=0;i<NUM_HANDLERS;i++) {
		if (handlers[i]) {
			delete(handlers[i]);
			handlers[i] = NULL;
		}
		satInViewList.dumpList();
	}
}

	
//GPVTG, GPGGA, GPGSA, GPGSV, GPRMC,	
void GPSReader::begin(void) {

	int	i;
	
	numStreamIn::begin();
	i = 0;
	handlers[i++] = new GPVTG(this);
	handlers[i++] = new GPGGA(this);
	handlers[i++] = new GPGSA(this);
	handlers[i++] = new GPGSV(this);
	handlers[i++] = new GPRMC(this);
}


void GPSReader::reset(void) {

	numStreamIn::reset();
	theHandler = -1;
}


bool GPSReader::canHandle(const char* param) {

	theHandler = -1;
	for (int i=0;i<NUM_HANDLERS;i++) {
		if (handlers[i]) {
			if (handlers[i]->canHandle(param)) {
				theHandler = i;
				return true;
			}
		}
	}
	Serial.print("**** Oh ohh! No handler for [");
	Serial.print(param);
	Serial.println("] ****");
	return false;
}



bool GPSReader::addValue(char* param,int paramIndex,bool isLast) {


	if (theHandler>=0) {
		if (handlers[theHandler]) {
			return handlers[theHandler]->decodeParam(param,paramIndex,isLast);
		}
	}
	return false;
}


      		