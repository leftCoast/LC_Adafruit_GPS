#include <GPSHandlers.h>


// **********************************************
// ****************     GP???    ****************
// **********************************************



// **********************************************
// ****************     GPVTG    ****************
// **********************************************

   
GPVTG::GPVTG(void)
   : GPSInHandler("GPVTG") {
      
    trueCourse          = 0;
    magCourse           = 0;
    groudSpeedKnots     = 0;
    groundSpeedKilosPH  = 0;
}

   
GPVTG::~GPVTG(void) {  }

void GPVTG::readVar(int index,bool lastField) {
   
   if (!readErr) {
      switch(index) {
         case 1   : tC = atof(mTokenBuff); break;
         case 2   : if (strcmp(mTokenBuff,"T")) readErr = true; break;
         case 3   : mC = atof(mTokenBuff); break;
         case 4   : if (strcmp(mTokenBuff,"M")) readErr = true; break;
         case 5   : gSKn = atof(mTokenBuff); break;
         case 6   : if (strcmp(mTokenBuff,"N")) readErr = true; break;
         case 7   : gSKilo = atof(mTokenBuff); break;
         case 8   :
            stripChecksum(mTokenBuff);
            if (!strcmp(mTokenBuff,"K")) {
               trueCourse          = tC;
               magCourse           = mC;
               groudSpeedKnots     = gSKn;
               groundSpeedKilosPH  = gSKilo; 
            }
            showData();
         break;
      }
   }
}


#ifdef SHOW_DATA          
void GPVTG::showData(void) {

	Serial.println("-------------------------------");
   Serial.print("True Course :\t");Serial.print(trueCourse);Serial.println("\tDegrees");
   Serial.print("Mag Course  :\t");Serial.print(magCourse);Serial.println("\tDegrees");
   Serial.print("Groud Speed :\t");Serial.print(groudSpeedKnots);Serial.println("\tKnots");
   Serial.print("Groud Speed :\t");Serial.print(groundSpeedKilosPH);Serial.println("\tKilometers per hour");
   Serial.println("-------------------------------");
}
#endif


// **********************************************
// ****************     GPGGA    ****************
// **********************************************


GPGGA::GPGGA(void)
	: GPSInHandler("GPGGA") {
	
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

	
GPGGA::~GPGGA(void) {  }


void GPGGA::readVar(int index,bool lastField) {
   
   char	buff[8];
   char*	ptr;
	int	rawQ;
	
   if (!readErr) {
      switch(index) {
         case 1	:
         	buff[0] = mTokenBuff[0];
         	buff[1] = mTokenBuff[1];
         	buff[2] = '\n';  
         	h = atoi(buff);
         	buff[0] = mTokenBuff[2];
         	buff[1] = mTokenBuff[3];
         	m =  atoi(buff);
				ptr = &(mTokenBuff[4]);
				s = atof(ptr);	
			break;
			case 2	: pos.setLatValue(mTokenBuff); break;
			case 3	: pos.setLatQuad(mTokenBuff); break;
			case 4	: pos.setLonValue(mTokenBuff); break;
			case 5	: pos.setLonQuad(mTokenBuff); break;
			case 6	:
				rawQ = atoi(mTokenBuff);
				if (rawQ==0) {
					qual = fixInvalid;
				} else if (rawQ==1) {
					qual = fixByGPS;
				} else if (rawQ==2) {
					qual = fixByDGPS;
				} else {
					readErr = true;
				}
			break;
			case 7	: numSat = atoi(mTokenBuff); break;
			case 8	: Acc = atof(mTokenBuff); break;
			case 9	: alt = atof(mTokenBuff); break;
			case 10	: if (strcmp(mTokenBuff,"M"))	readErr = true; break;
			case 11	: GHeight = atof(mTokenBuff); break;
			case 12	: if (strcmp(mTokenBuff,"M"))	readErr = true; break;
			case 13	: ageOfData = atof(mTokenBuff); break;
			case 14	: 
				stripChecksum(mTokenBuff);
				stationID = atoi(mTokenBuff);
				hours				= h;
				min				= m;
				sec				= s;
				latLon.copyPos(&pos);
				qualVal			= qual;
				numSatellites	= numSat;
				HDOP				= Acc;
				altitude			= alt;
				GeoidalHeight	= GHeight;
				ageOfDGPSData	= ageOfData;
				DGPSStationID	= stationID;
				showData();
			break;       
      }
   }
}


#ifdef SHOW_DATA
void GPGGA::showData(void) {
	
	char degStr[10];


	Serial.println("-------------------------------");
	Serial.print("Time :\t\t\t");Serial.print(hours);Serial.print(":");
	Serial.print(min);Serial.print(":");Serial.println(sec,2);
	sprintf(degStr,"%3u",latLon.getLatDeg());
	Serial.print("Lattitide :\t\t");Serial.print(degStr);Serial.print(" Deg.\t");Serial.print(latLon.getLatMin(),2);Serial.print(" Min.\t");Serial.println(latLon.getLatQuadStr());
	sprintf(degStr,"%3u",latLon.getLonDeg());
	Serial.print("Longitutde :\t\t");Serial.print(degStr);Serial.print(" Deg.\t");Serial.print(latLon.getLonMin(),2);Serial.print(" Min.\t");Serial.println(latLon.getLonQuadStr());
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



GPGSA::GPGSA(void)
	: GPSInHandler("GPGSA") {
	
	operationMode = manual;
	fixType			= noFix;
	for (int i=0;i<11;i++) {
		SVID[i] = 0;
	}
	PDOP				= 0;
	HDOP				= 0;
	VDOP				= 0;
}


GPGSA::~GPGSA(void) {  }


void GPGSA::readVar(int index,bool lastField) {

	int	rawMode;
	int	count;
	
	if (!readErr) {
		if (index>=3 && index <=14) {
			ID[index-3] = atoi(mTokenBuff);
			return;
		}
      switch(index) {
         case 1	:
				if (!strcmp(mTokenBuff,"M")) {
					M1 = manual;
				} else if (!strcmp(mTokenBuff,"A")) {
					M1 = automatic;
				} else {
					readErr = true;
				}
			break;
			case 2	:
				rawMode = atoi(mTokenBuff);
				if (rawMode==1) {
					M2 = noFix;
				} else if (rawMode==2) {
					M2 = twoD;
				} else if (rawMode==3) {
					M2 = threeD;
				} else  {
					readErr = true;
				}
			break;
			case 15	: P = atof(mTokenBuff); break;
			case 16	: H = atof(mTokenBuff); break;
			case 17	:
				stripChecksum(mTokenBuff);
				V = atof(mTokenBuff);
				operationMode	= M1;
				fixType			= M2;
				count = 0;
				for(int i=0;i<11;i++) {
					SVID[i] = ID[i];
					if (SVID[i]) {
						count++;
					}
				}
				numSatellites = count;
				PDOP	= P;
				HDOP	= H;
				VDOP	= V;
				showData();
			break;
		}
	}
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
azmuth;
SigToNoise;
*/

satData::satData(void)
	: linkListObj() {  }
	
satData::~satData(void) {  }



// ****************     GPGSV    ****************


//GPS Satellites in view
GPGSV::GPGSV(void)
	: GPSInHandler("GPGSV") {
	
	msgNum	= 0;
	loopNum	= 0;
	satNum	= 0;
}
	
	
GPGSV::~GPGSV(void) {  }
	
	
bool GPGSV::handleStr(char* inID,GPSInStr* inGPSInStream) {

	if (GPSInHandler::handleStr(inID,inGPSInStream)) {
		msgNum++;
		loopNum = 0;
		return true;
	}
	return false;
}


int GPGSV::recalcIndex(int index) { return index - (loopNum * 4); }

	
void GPGSV::readVar(int index,bool lastField) {

	satData* dataNode;
	int		dataItem;
	
	if (readErr) return;													// Only do this if there have been no errors;
	if (msgNum==1) {														// If we're looking for the initial message..
		if (index==1 ) {													// Assume this is the first field of the initial message.
			numMessages = atoi(mTokenBuff);							// First field of initial message, sets num messages.
		} else if (index==2) {											// Second message gives..
			if (atoi(mTokenBuff)!=1) {									// If it's not one..
				Serial.println(1);
				readErr = true;											// toss an error.
			}																	//
		} else if (index==3) {											// Looking at initial third field.
			numSatellites = atoi(mTokenBuff);						// Here's how many we have coming.
		}																		//
	} else {																	// Else NOT initial message..
		if (index==1) {													// Subsequent first field.
			if (atoi(mTokenBuff)!=numMessages) {					// If we get different number of messages..
				readErr = true;											// toss an error.
			}																	//
		} else if (index==2) {											// Second message gives..
			if (atoi(mTokenBuff)!=msgNum) {							// If we get different message num that we expect..
				readErr = true;											// toss an error.
			}																	//
		}  else if (index==3) {											// Looking at third field..
			if (atoi(mTokenBuff)!=numSatellites) {					// If num satellites doesn't match..
				readErr = true;											// toss an error.
			}
		}
	}																		//
	dataItem = recalcIndex(index);								// IF the index is >3 we need to make sure its in the range of 4..7, over and over.
	if (dataItem==4) {												// Fourth of any message. (First of each satellite)
		tempData.PRNNum = atoi(mTokenBuff);						// We stuff in this bit 'O data.					
	} else if (dataItem==5) { 										//
		tempData.elevation = atoi(mTokenBuff);					// We stuff in this bit 'O data.
	} else if (dataItem==6) { 										//
		tempData.azmuth = atoi(mTokenBuff);						// We stuff in this bit 'O data.
	} else if (dataItem==7) { 										//   
		tempData.SigToNoise = atoi(mTokenBuff);				// We stuff in this bit 'O data.
		dataNode = new satData();									// Create a new node..
		if (dataNode) {												// If we got one..
			
			dataNode->PRNNum = tempData.PRNNum;					// Fill in the node's data..
			dataNode->elevation = tempData.elevation;			// Each one..
			dataNode->azmuth = tempData.azmuth;					// Little boring..
			dataNode->SigToNoise = tempData.SigToNoise;		// Save it.
			workingSatList.addToEnd(dataNode);					// Link the filled node into the working list.
			satNum++;													// This one's done. Bump up sat num.
			loopNum++;													// We bump up the loop num as well.
		} else {															// No node? This is bad.
			readErr = true;											// Error out.
		}																	//
	}																		//
	if (readErr) {														// We error out?
		msgNum = 0;														// Reset message count.
		satNum = 0;														// Reset satellite count.
		loopNum = 0;													// Reset loop count.
		workingSatList.dumpList();									// Recycle sat data we collected.
	} else if (lastField) {											// Done with this message..
		if (satNum==numSatellites) {								// If we got 'em all. No error!
			msgNum = 0;													// Reset message count.
			satNum = 0;													// Reset satellite count.
			loopNum = 0;												// Reset loop count.
			satDataList.dumpList();									// Recycle old sat data.
			dataNode = (satData*)workingSatList.getFirst();	// Grab the list from the working linkList.
			workingSatList.looseList();							// Let my people go!
			satDataList.addToEnd(dataNode);						// New list is passed over.
			showData();
		} else {															// Else, this message is done, but we expect another..
			loopNum = 0;												// Reset loop count.
		}																	//
	}
}
		
		  
#ifdef SHOW_DATA				
void GPGSV::showData(void) {

	int		numItems;
	satData*	dataNode;
	
	numItems = satDataList.getCount();
	Serial.println("-------------------------------");
	Serial.print("Satelites in view :\t");Serial.println(numItems);
	for (int i=0;i<numItems;i++) {
		dataNode = (satData*)satDataList.getByIndex(i);
		if (dataNode) {
			Serial.print("Satellite :\t");Serial.println(dataNode->PRNNum);
			Serial.print("Elevation :\t");Serial.println(dataNode->elevation);
			Serial.print("Azmuth :\t");Serial.println(dataNode->azmuth);
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
GPRMC::GPRMC(void)
	: GPSInHandler("GPRMC") { 

	hours			= 0;
	min			= 0;
	sec			= 0;
	valid			= false;
	groundSpeed	= 0;
	trueCourse	= 0;
	year			= 2025;
	month			= 5;
	day			= 2;
	magVar		= 0;
	vEastWest	= west;
}


GPRMC::~GPRMC(void) {  }


void GPRMC::readVar(int index,bool lastField) {

	char	temp[10];
	
	if (readErr) return;											// Only do this if there have been no errors;       		
	switch(index) {
		case 1	:													// hhmmss.ss
			temp[0] = mTokenBuff[0];
			temp[1] = mTokenBuff[1];
			temp[2] = '\0';
			h = atoi(temp);
			temp[0] = mTokenBuff[2];
			temp[1] = mTokenBuff[3];
			temp[2] = '\0';
			mn = atoi(temp);
			strcpy(temp,&(mTokenBuff[4]));
			s = atof(temp);
		break;
		case 2	:
			val = false;
			if (!strcmp(mTokenBuff,"A")) {
				val = true;
			}
		break;
		case 3	: pos.setLatValue(mTokenBuff); break;
		case 4	: pos.setLatQuad(mTokenBuff); break;
		case 5	: pos.setLonValue(mTokenBuff); break;
		case 6	: pos.setLonQuad(mTokenBuff); break;
		case 7	: knots = atof(mTokenBuff); break;
		case 8	: tCourse = atof(mTokenBuff); break;
		case 9	:													// ddmmyy
			temp[0] = mTokenBuff[0];
			temp[1] = mTokenBuff[1];
			temp[2] = '\0';
			d = atoi(temp);
			temp[0] = mTokenBuff[2];
			temp[1] = mTokenBuff[3];
			temp[2] = '\0';
			mo = atoi(temp);
			temp[0] = mTokenBuff[4];
			temp[1] = mTokenBuff[5];
			temp[2] = '\0';
			y = atoi(temp);
		break;
		case 10	: mVar = atof(mTokenBuff); break;
		case 11	:
			upCase(mTokenBuff);
			EW = west;
			if (!strcmp(mTokenBuff,"E")||!strcmp(mTokenBuff,"EAST")) {
				EW = east;
			}
			hours			= h;
			min			= mn;
			sec			= s;
			valid			= val;
			groundSpeed	= knots;
			trueCourse	= tCourse;
			year			= y;
			month			= mo;
			day			= d;
			magVar		= mVar;
			vEastWest	= EW; 
			showData(); 
		break;
	}
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
	Serial.print("Lattitide :\t");Serial.print(degStr);Serial.print(" Deg.\t");Serial.print(latLon.getLatMin(),2);Serial.print(" Min.\t");Serial.println(latLon.getLatQuadStr());
	sprintf(degStr,"%3u",latLon.getLonDeg());
	Serial.print("Longitutde :\t");Serial.print(degStr);Serial.print(" Deg.\t");Serial.print(latLon.getLonMin(),2);Serial.print(" Min.\t");Serial.println(latLon.getLonQuadStr());
	Serial.print("Groud Speed :\t");Serial.print(groundSpeed);Serial.println("\tKnots");
	Serial.print("True Course :\t");Serial.print(trueCourse);Serial.println("\tDegrees");
  	Serial.print("Date :\t\t");Serial.print(month);Serial.print("/");
	Serial.print(day);Serial.print("/");Serial.println(year);
	Serial.print("Mag variation :\t");Serial.print(magVar);Serial.print("\t");
	if (vEastWest==east) {
		Serial.println("East");
	} else {
		Serial.println("West");
	}
	Serial.println("-------------------------------");
}
#endif


// **********************************************
// ****************     GP???    ****************
// **********************************************



