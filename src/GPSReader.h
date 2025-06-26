#ifndef GPSReader_h
#define GPSReader_h


#include <numStream.h>
#include <idlers.h>
#include <debug.h>
#include <globalPos.h>

#define SHOW_DATA

class GPSMsgHandler;
class GPVTG;
class GPGGA;
class GPGSA;
class GPGSV;
class GPRMC;

extern	GPVTG		trackMadeGood;  
extern	GPGGA		fixData; 
extern	GPGSA		activeSatellites; 
extern	GPGSV		SatellitesInView; 
extern	GPRMC		minTransData;


// **********************************************
// ****************   GPSReader  ****************
// **********************************************

class GPSReader :	public numStreamIn,
                  public idler {

   public:
            GPSReader(Stream* inPort=DEF_IN_PORT,int tokenBuffBytes=DEF_TOKEN_BYTES);
   virtual  ~GPSReader(void);

				void	begin(void);
            void  addHandler(GPSMsgHandler* inHandler);
            void  checkHandlers(char* inStr);
   virtual  void  idle(void);
   virtual  void  readVar(int index,bool lastField);

            linkList			handlers;
            GPSMsgHandler*	currentHandler;
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


// **********************************************
// *************** GPSMsgHandler ****************
// **********************************************


class GPSMsgHandler :	public linkListObj,
								public numStreamIn {
	
   public:
            GPSMsgHandler(const char* inIDStr);
   virtual  ~GPSMsgHandler(void);

   virtual  bool  handleStr(char* inID,GPSReader* inGPSReadeream);
            void  stripChecksum(char* inStr);
            char*	quadToText(quad inQuad);
            char*	qualityToText(fixQuality inQual);
   virtual  void  showData(void);
   
            char*       IDStr;
            bool        readErr;
};


/* A handy template.

// **********************************************
// ****************     GPXXX    ****************
// **********************************************


class GPXXX :  public GPSMsgHandler {

   public:
            GPXXX(void);
   virtual  ~GPXXX(void);

#ifdef SHOW_DATA
   virtual  void  showData(void);
#endif

	protected:
	virtual  void readVar(int index,bool lastField);
             
};

*/


// **********************************************
// ****************     GPVTG    ****************
// **********************************************


// Track Made Good and Ground Speed.
class GPVTG :  public GPSMsgHandler {

   public:
            GPVTG(void);
   virtual  ~GPVTG(void);

#ifdef SHOW_DATA
   virtual  void  showData(void);
#endif
            
            float trueCourse;
            float magCourse;
            float groudSpeedKnots;
            float groundSpeedKilosPH;
            
   protected:
   
	virtual	void	readVar(int index,bool lastField);
    
				float tC;
				float mC;         
				float gSKn;
				float gSKilo;
};



// **********************************************
// ****************     GPGGA    ****************
// **********************************************

	
// Global Positioning System Fix Data
class GPGGA :  public GPSMsgHandler {

   public:
            GPGGA(void);
   virtual  ~GPGGA(void);

#ifdef SHOW_DATA
   virtual  void  showData(void);
#endif

            int 			hours;
            int 			min;
            float 		sec;
            globalPos	latLon;
            fixQuality	qualVal;
            int			numSatellites;
            float			HDOP;					// Relative accuracy of horizontal position.
            float			altitude;
            float			GeoidalHeight;
            float			ageOfDGPSData;
            int			DGPSStationID;
            
	protected:
	virtual  void readVar(int index,bool lastField);
	
				int 			h;
            int 			m;
            float 		s;
            globalPos	pos;
            fixQuality	qual;
            int			numSat;
            float			Acc;
            float			alt;
            float			GHeight;
            float			ageOfData;
            int			stationID;           
};



// **********************************************
// ****************     GPGSA    ****************
// **********************************************


//GPS DOP and active satellites
class GPGSA :  public GPSMsgHandler {

   public:
            GPGSA(void);
   virtual  ~GPGSA(void);

#ifdef SHOW_DATA
   virtual  void  showData(void);
#endif
   
   			mode		operationMode;
				modeII	fixType;
				int		numSatellites;
				int		SVID[11];
				float		PDOP;
				float		HDOP;
				float		VDOP;
				
	protected:
	virtual  void readVar(int index,bool lastField);
	
				mode		M1;
				modeII	M2;
				int		ID[11];
				float		P;
				float		H;
				float		V;
};



// **********************************************
// ****************     GPGSV    ****************
// **********************************************


class satData :	public linkListObj {

	public:
				satData(void);
	virtual	~satData(void);

	int	PRNNum;
	float	elevation;
	float	azmuth;
	int	SigToNoise;
};


//GPS Satellites in view
class GPGSV :  public GPSMsgHandler {

   public:
            GPGSV(void);
   virtual  ~GPGSV(void);
	
	virtual  bool  handleStr(char* inID,GPSReader* inGPSInStream);

#ifdef SHOW_DATA
   virtual  void  showData(void);
#endif
				
				linkList	satDataList;
					
	protected:
				void	adjustList(int numItems);
				int	recalcIndex(int index);
	virtual  void	readVar(int index,bool lastField);
    			
            int		numMessages;
				int		msgNum;
				int		numSatellites;
				int		satNum;
				int		loopNum;
				satData	tempData;
				linkList	workingSatList;
};



// **********************************************
// ****************     GPRMC    ****************
// **********************************************


// Recommended minimum specific GPS/Transit data
class GPRMC :  public GPSMsgHandler {

   public:
            GPRMC(void);
   virtual  ~GPRMC(void);
   
#ifdef SHOW_DATA
   virtual  void  showData(void);
#endif

				int			hours;
            int			min;
            float			sec;
            bool			valid;
            globalPos	latLon;
            float			groundSpeed;
            float			trueCourse;
            int			year;
            int			month;
            int			day;
            float			magVar;
            quad			vEastWest;
            
            
	protected:
	virtual  void readVar(int index,bool lastField);
       		
       		int			h;
				int			mn;
				float			s;
				bool			val;
				globalPos	pos;
				float			knots;
				float			tCourse;
				int			y;
				int			mo;
				int			d;
				float			mVar;
				quad			EW;      
};




#endif
