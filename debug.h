
#ifndef _DEBUG_H_
#define _DEBUG_H_


//#include "loopidity_sdl.h"
//class LoopiditySdl ;


#define DBG Debug::Dbg


class Debug
{
	public:

		static void Dbg(const char* msgIn) { strcpy(MsgOut , msgIn) ; SetStatus() ; }
		static void Dbg(const char* msgIn , int i1) { sprintf(MsgOut , msgIn , i1) ; SetStatus() ; }
		static void Dbg(const char* msgIn , int i1 , int i2) { sprintf(MsgOut , msgIn , i1 , i2) ; SetStatus() ; }
		static void Dbg(const char* msgIn , int i1 , int i2 , int i3) { sprintf(MsgOut , msgIn , i1 , i2 , i3) ; SetStatus() ; }
		static void Dbg(const char* msgIn , int i1 , int i2 , int i3 , int i4) { sprintf(MsgOut , msgIn , i1 , i2 , i3 , i4) ; SetStatus() ; }
		static void SetStatus() { LoopiditySdl::SetStatusL(MsgOut) ; }

	private:

		static char MsgOut[256] ;
} ;

char Debug::MsgOut[256] = {0} ;


#endif // _DEBUG_H_
