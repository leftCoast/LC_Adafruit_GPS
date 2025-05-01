#include "GPxxx.h"
#include <strTools.h>





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