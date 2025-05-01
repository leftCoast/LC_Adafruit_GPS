#ifndef GPxxx_h
#define GPxxx_h


#include <numStream.h>
#include <idlers.h>
#include <debug.h>

extern char	strBuff[];

enum quad {
	north,
	south,
	east,
	west
};


enum fixQuality {
	fixInvalid,
	fixByGPS,
	fixByDGPS
};


enum mode {
	manual,
	automatic
};


enum modeII {
	noFix,
	twoD,
	threeD
};


class GPSInStr;

class GPSInHandler : public linkListObj,
                     public numStreamIn {

   public:
            GPSInHandler(const char* inIDStr);
   virtual  ~GPSInHandler(void);

   virtual  bool  handleStr(char* inID,GPSInStr* inGPSInStream);
            void  stripChecksum(char* inStr);
            char*	quadToText(quad inQuad);
            char*	qualityToText(fixQuality inQual);
   virtual  void  showData(void);
   
            char*       IDStr;
            bool        readErr;
};


class GPSInStr :  public numStreamIn,
                  public idler {

   public:
            GPSInStr(Stream* inPort=DEF_IN_PORT,int tokenBuffBytes=DEF_TOKEN_BYTES);
   virtual  ~GPSInStr(void);

            void  addHandler(GPSInHandler* inHandler);
            void  checkHandlers(char* inStr);
   virtual  void  idle(void);
   virtual  void  readVar(int index,bool lastField);

            linkList			handlers;
            GPSInHandler*	currentHandler;
};


#endif
