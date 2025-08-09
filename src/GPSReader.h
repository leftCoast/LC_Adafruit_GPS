#ifndef GPSReader_h
#define GPSReader_h


#include <numStream.h>
#include <globalPos.h>

#define NUM_HANDLERS	5	// GPVTG, GPGGA, GPGSA, GPGSV, GPRMC

enum fixQuality {
	fixInvalid,
	fixByGPS,
	fixByDGPS
};


enum mode {
	manual,
	automatic
};

// From novatel documents.
enum posModes {
	Autonomous,
	Differential,
	Estimated,		// Dead reckoning.
	Manual,
	notValid
};

enum modeII {
	noFix,
	twoD,
	threeD
};


class GPSReader;


// **********************************************
// *************** GPSMsgHandler ****************
// **********************************************

// Base functionality of the incoming GPS handlers.
class GPSMsgHandler {
	
   public:
            GPSMsgHandler(const char* inType,GPSReader* inReader);
   virtual  ~GPSMsgHandler(void);

   virtual  void	clearValues(void);
   virtual  bool	canHandle(const char* inType);
   virtual	bool	decodeParam(char* inParam,int paramIndex,bool lastParam);
            void  stripChecksum(char* inStr);
            char*	quadToText(quad inQuad);
            char*	qualityToText(fixQuality inQual);
   virtual  void  showData(void);
   
            char*			typeStr;
            GPSReader*	reader;
            bool			readErr;
};



// **********************************************
// ****************     GPVTG    ****************
// **********************************************


// Track Made Good and Ground Speed.
class GPVTG :  public GPSMsgHandler {

   public:
            GPVTG(GPSReader* inReader);
   virtual  ~GPVTG(void);
   
   virtual  void	clearValues(void);
	virtual	bool	decodeParam(char* inParam,int paramIndex,bool lastParam);
	
#ifdef SHOW_DATA
   virtual  void  showData(void);
#endif
            
            float 	trueCourse;
            float 	magCourse;
            float 	groudSpeedKnots;
            float 	groundSpeedKilos;
            posModes	posMode;
};			



// **********************************************
// ****************     GPGGA    ****************
// **********************************************

	
// Global Positioning System Fix Data
class GPGGA :  public GPSMsgHandler {

   public:
            GPGGA(GPSReader* inReader);
   virtual  ~GPGGA(void);
	
	virtual  void	clearValues(void);
	virtual	bool	decodeParam(char* inParam,int paramIndex,bool lastParam);
	
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
};



// **********************************************
// ****************     GPGSA    ****************
// **********************************************


//GPS DOP and active satellites
class GPGSA :  public GPSMsgHandler {

   public:
            GPGSA(GPSReader* inReader);
   virtual  ~GPGSA(void);

	virtual  void	clearValues(void);
	virtual	bool	decodeParam(char* inParam,int paramIndex,bool lastParam);
	
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
};



// **********************************************
// ****************     GPGSV    ****************
// **********************************************


class satData :	public linkListObj {

	public:
				satData(void);
				satData(satData* copyData);
	virtual	~satData(void);

				int	PRNNum;
				float	elevation;
				float	azimuth;
				int	SigToNoise;
};


enum setupReasons {

	newMsgSet,
	newMsg,
	newDataSet
};


//Data from the GPS Satellites in view.
class GPGSV :  public GPSMsgHandler {

   public:
            GPGSV(GPSReader* inReader);
   virtual  ~GPGSV(void);
	
				void	setup(setupReasons reason);
				int	adjustedIndex(int paramIndex);
				bool	addNode(void);
				void	setComplete(void);
	virtual	bool	decodeParam(char* inParam,int paramIndex,bool lastParam);
	
#ifdef SHOW_DATA
   virtual  void  showData(void);
#endif
				int		numMsgs;				// Multiple messages in a set. How many to expect.
				int		msgNum;				// What message are we waiting for or working on?
				int		repeatNum;			// Multiple data sets in a message. What one we working on? In that message.
				int		numSatllites;		// Number of data sets to expect as a total of all messages.
				int		satNum;				// Which of the total data sets are we working on?
				linkList	workingSatList;	// Local list of satellite data sets to be handed lover to the reader.
				satData	tempData;			// Where we store the data set we are currently working on.
				
};



// **********************************************
// ****************     GPRMC    ****************
// **********************************************


// Recommended minimum specific GPS/Transit data
class GPRMC :  public GPSMsgHandler {

   public:
            GPRMC(GPSReader* inReader);
   virtual  ~GPRMC(void);
   
   virtual  void	clearValues(void);
	virtual	bool	decodeParam(char* inParam,int paramIndex,bool lastParam);
	
#ifdef SHOW_DATA
   virtual  void  showData(void);
#endif

				int			hours;
            int			min;
            float			sec;
            bool			valid;
            globalPos	latLon;
            float			groudSpeedKnots;
            float			trueCourse;
            int			year;
            int			month;
            int			day;
            float			magVar;
            quad			vEastWest; 
            posModes		posMode;    
};


// **********************************************
// ****************   GPSReader  ****************
// **********************************************


class GPSReader :	public numStreamIn {

	public:
				GPSReader(Stream* inStream=DEF_IN_PORT,int tokenBuffBytes=DEF_TOKEN_BYTES);
	virtual	~GPSReader(void);
	
				void	begin(void);
	virtual	void 	reset(void);
	virtual	bool	canHandle(const char* param);
	virtual  bool	addValue(char* param,int paramIndex,bool isLast);
	
				GPSMsgHandler*	handlers[NUM_HANDLERS];
				int				theHandler;
				
				int				year;
            int				month;
            int				day;
            
            int 				hours;
            int 				min;
            float 			sec;
            
            globalPos		latLon;
            float				altitude;
            fixQuality		qualVal;
            bool				valid;
            posModes			posMode; 
            
            float				trueCourse;
            float				magCourse;
            float				groudSpeedKnots;
            float				groundSpeedKilos;
            
            uint16_t			magVar;
            char				vEastWest;
            float				GeoidalHeight;
            float				ageOfDGPSData;
            int				DGPSStationID;
            mode				operationMode;
				modeII			fixType;
				int				numSatellites;
				int				SVID[11];
				float				PDOP;
				float				HDOP;
				float				VDOP;
				
				linkList			satInViewList;
};


// Pointer to our global GPS input source.
extern GPSReader* ourGPS;

/*

Should be called out in your global code space, above setup(), like..

ourGPS = new GPSReader;	
GPSReader->begin();

For default using DEF_INSTREAM and DEF_TOKEN_BYTES as serial buffer extender. (see top of
numStream.h file.)


Or you can have a different port with default buffer..

ourGPS = new GPSReader(Serial2);		
GPSReader->begin();

Or set both..

ourGPS = new GPSReader(Serial1,200);
GPSReader->begin();

*/



				            
#endif            

