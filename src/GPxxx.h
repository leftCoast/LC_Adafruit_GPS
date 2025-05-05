#ifndef GPxxx_h
#define GPxxx_h


#include <numStream.h>
#include <idlers.h>
#include <debug.h>

//#define SHOW_DATA


extern char	strBuff[];

class GPSInStr;


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


extern bool	checkLatDeg(int degrees);
extern bool	checkLonDeg(int degrees);
extern bool	checkMin(float minutes);



// **********************************************
// ****************  gPosition  *****************
// **********************************************

#define	G_POS_BUFF_BYTES	20

class gPosition {

	public:
				gPosition(void);
	virtual	~gPosition(void);
	
				bool	valid(void);
				
				void	copyPos(gPosition* aLatLon);
				
				void	setLatValue(const char* inLatStr);
				void	setLatQuad(const char* inQuad);
				void	setLonValue(const char* inLonStr);
				void	setLonQuad(const char* inQuad);
				
				void	setPosValues(const char* latStr,const char* lonStr);
				void	setQuads(const char*  inLatQuad,const char*  inLonQuad);
				
				void	setLatValue(int inLatDeg, float inLatMin);
				void	setLatQuad(quad inLatQuad);
				void	setLonValue(int inLonDeg, float inLonMin);
				void	setLonQuad(quad inLonQuad);
				void	setQuads(quad inLatQuad,quad inLonQuad);
				
				void	setPosition(int inLatDeg, float inLatMin, quad inLatQuad, int inLonDeg, float inLonMin, quad inLonQuad);
				
				float	trueBearingTo(gPosition* inDest);
				float	distanceTo(gPosition* inDest);
				
				char*	getLatStr(void);
				char*	getLatQuadStr(void);
				char*	getLonStr(void);
				char*	getLonQuadStr(void);
				
				int	getLatDeg(void);
				float	getLatMin(void);
				quad	getLatQuad(void);
				int	getLonDeg(void);
				float	getLonMin(void);
				quad	getLonQuad(void);
				
	protected:
				int	latDeg;
				float	latMin;
				quad	latQuad;
				int	lonDeg;
				float	lonMin;
				quad	lonQuad;
				char	outStr[G_POS_BUFF_BYTES];
};



// **********************************************
// **************** GPSInHandler ****************
// **********************************************


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
