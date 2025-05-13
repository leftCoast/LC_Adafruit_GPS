#ifndef GPSHandlers_h
#define GPSHandlers_h

#include <GPxxx.h>


class GPSInHandler;

/* A handy template.

// **********************************************
// ****************     GP???    ****************
// **********************************************


class GP??? :  public GPSInHandler {

   public:
            GP???(void);
   virtual  ~GP???(void);

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
class GPVTG :  public GPSInHandler {

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
class GPGGA :  public GPSInHandler {

   public:
            GPGGA(void);
   virtual  ~GPGGA(void);

#ifdef SHOW_DATA
   virtual  void  showData(void);
#endif

            int 			hours;
            int 			min;
            float 		sec;
            gPosition	latLon;
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
            gPosition	pos;
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
class GPGSA :  public GPSInHandler {

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
class GPGSV :  public GPSInHandler {

   public:
            GPGSV(void);
   virtual  ~GPGSV(void);
	
	virtual  bool  handleStr(char* inID,GPSInStr* inGPSInStream);

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
class GPRMC :  public GPSInHandler {

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
            gPosition	latLon;
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
				gPosition	pos;
				float			knots;
				float			tCourse;
				int			y;
				int			mo;
				int			d;
				float			mVar;
				quad			EW;      
};


            



#endif