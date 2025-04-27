#include "GPxxx.h"
#include <strTools.h>
#include <debug.h>

// **********************************************
// **************** GPSInHandler ****************
// **********************************************


GPSInHandler::GPSInHandler(const char* inIDStr)
   : linkListObj(),
   numStreamIn() {
      
   IDStr = NULL;
   heapStr(&IDStr,inIDStr);
   ourBoss = NULL;
}


GPSInHandler::~GPSInHandler(void) { freeStr(&IDStr); }
   

bool GPSInHandler::handleStr(char* inID,GPSInStr* inGPSInStream) {

   Serial.println(inID);
   if (!strcmp(inID,IDStr)) {
      copyStream(inGPSInStream);
      ourBoss = inGPSInStream;
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

void GPSInStr::readVar(int index) {

   if (index==0) {
      checkHandlers(mTokenBuff);
   }
}



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

void GPVTG::readVar(int index) {
   
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

          
void GPVTG::showData(void) {

   Serial.print("True Course :\t");Serial.print(trueCourse);Serial.println("\tDegrees");
   Serial.print("Mag Course  :\t");Serial.print(magCourse);Serial.println("\tDegrees");
   Serial.print("Groud Speed :\t");Serial.print(groudSpeedKnots);Serial.println("\tKnots");
   Serial.print("Groud Speed :\t");Serial.print(groundSpeedKilosPH);Serial.println("\tKilometers per hour"); 
}
