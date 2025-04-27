#ifndef GPxxx_h
#define GPxxx_h


#include <numStream.h>
#include <idlers.h>

class GPSInStr;

class GPSInHandler : public linkListObj,
                     public numStreamIn {

   public:
            GPSInHandler(const char* inIDStr);
   virtual  ~GPSInHandler(void);

   virtual  bool  handleStr(char* inID,GPSInStr* inGPSInStream);
            void  stripChecksum(char* inStr);
   virtual  void  showData(void);
   
            char*       IDStr;
            GPSInStr*   ourBoss;
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
   virtual  void  readVar(int index);

            linkList    handlers;
            GPSInHandler* currentHandler;
};



class GPVTG :  public GPSInHandler {

   public:
            GPVTG(void);
   virtual  ~GPVTG(void);

   virtual  void readVar(int index);
   virtual  void  showData(void);

            float tC;
            float mC;         
            float gSKn;
            float gSKilo;
            
            float trueCourse;
            float magCourse;
            float groudSpeedKnots;
            float groundSpeedKilosPH; 
};



#endif
