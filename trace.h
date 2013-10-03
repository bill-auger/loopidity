
#ifndef _TRACE_H_
#define _TRACE_H_
/*
#define LOOPIDITY_CLASS 1
#define LOOPIDITYSDL_CLASS 2
#define SCENE_CLASS 3
#define SCENESDL_CLASS 4
*/
/*
#define DEBUG_TRACE_TABx 35
//#define DEBUG_TRACE_TAB4 49
#define DEBUG_TRACE_TABxx 4
#define DEBUG_TRACE_TAB3 4
//#define DEBUG_TRACE_TAB4 57
*/
#define DEBUG_TRACE_MODEL "MODEL: "
#define DEBUG_TRACE_MODEL_ERROR "ERROR: "
#define DEBUG_TRACE_VIEW "VIEW:  "
#define DEBUG_TRACE_VIEW_ERROR "ERROR: "
#define DEBUG_TRACE_CONTROLLER DEBUG_TRACE_MODEL
#define DEBUG_TRACE_CONTROLLER_ERROR DEBUG_TRACE_MODEL_ERROR

#define DEBUG_TRACE_MODEL_STATE_FORMAT "%d%d%d   "
#define DEBUG_TRACE_VIEW_STATE_FORMAT "   %d%d%d"

#define DEBUG_TRACE_MODEL_DESC_FORMAT "isRecording=%d shouldSaveLoop=%d doesPulseExist=%d"
#define DEBUG_TRACE_VIEW_DESC_FORMAT "nLoops=%d nHistogramImgs=%d nLoopImgs=%d"
#define DEBUG_TRACE_MODEL_ERROR_FORMAT "MODEL: Scene::mvc inconsistent"
#define DEBUG_TRACE_VIEW_ERROR_FORMAT "VIEW: SceneSdl::mvc inconsistent"

#define DEBUG_TRACE_EVENT_LEN 36
#define DEBUG_TRACE_STATE_LEN 6
#define DEBUG_TRACE_DESC_LEN 128


class Trace
{
	public:

		static void TraceScene(const char* event , const char* sender ,
										const char* stateFormat , const char* descFormat ,
										bool bool0 , bool bool1 , bool bool2 , bool isEq) ;

	private:

		// constants
/*
		static const char *MODEL , *MODEL_ERR , *VIEW , *VIEW_ERR ;
		static const char *MODEL_STATE_FMT , *VIEW_STATE_FMT ;
		static const char *MODEL_DESC_FMT , *VIEW_DESC_FMT ;
		static const char *MODEL_ERR_FMT , *VIEW_ERR_FMT ;
*/
/*
		static const unsigned int EVENT_LEN , STATE_FMT_LEN , STATE_LEN ;
		static const unsigned int MODEL_DESC_FMT_LEN , VIEW_DESC_FMT_LEN , DESC_FMT_LEN , DESC_LEN ;
		static const unsigned int MODEL_ERR_FMT_LEN , VIEW_ERR_FMT_LEN ;
*/
/*
		static const char *MODEL , *MODEL_ERR , *VIEW , *VIEW_ERR ;
		static const char *MODEL_STATE_FMT , *VIEW_STATE_FMT ;
		static const char *MODEL_DESC_FMT , *VIEW_DESC_FMT ;
		static const char *MODEL_ERR_FMT , *VIEW_ERR_FMT ;
*/
		static const unsigned int EVENT_LEN , STATE_LEN , DESC_LEN ;

		// buffers
/*
		static char Event[DEBUG_TRACE_EVENT_LEN + 1] , State[DEBUG_TRACE_STATE_LEN + 1] , Desc[DEBUG_TRACE_DESC_LEN + 1] ;
		static const char* StateFormat ;
		static const char* DescFormat ;
		static unsigned int EventLen , OriginLen , StateFmtLen , StateLen , DescFmtLen , DescLen ;
*/
// buffers
		static char Event[DEBUG_TRACE_EVENT_LEN + 1] ;
		static char State[DEBUG_TRACE_STATE_LEN + 1] ;
		static char Desc[DEBUG_TRACE_DESC_LEN + 1] ;
		static const char *EventType , *SenderClass , *StateFormat , *DescFormat ;
		static unsigned int EventLen , SenderLen , StateLen , StateFormatLen , DescFormatLen , DescLen ;
} ;


#endif // #if _TRACE_H_
