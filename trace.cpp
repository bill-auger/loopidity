
#include "trace.h"


/*
const char* Trace::MODEL = DEBUG_TRACE_MODEL ;
const char* Trace::MODEL_ERR = DEBUG_TRACE_MODEL_ERROR ;
const char* Trace::VIEW = DEBUG_TRACE_VIEW ;
const char* Trace::VIEW_ERR = DEBUG_TRACE_VIEW_ERROR ;

const char* Trace::MODEL_STATE_FMT = DEBUG_TRACE_MODEL_STATE_FORMAT ;
const char* Trace::VIEW_STATE_FMT = DEBUG_TRACE_VIEW_STATE_FORMAT ;
const char* Trace::MODEL_DESC_FMT = DEBUG_TRACE_MODEL_DESC_FORMAT ;
const char* Trace::VIEW_DESC_FMT = DEBUG_TRACE_VIEW_DESC_FORMAT ;
const char* Trace::MODEL_ERR_FMT = DEBUG_TRACE_MODEL_ERROR_FORMAT ;
const char* Trace::VIEW_ERR_FMT = DEBUG_TRACE_VIEW_ERROR_FORMAT ;
*/
const unsigned int Trace::EVENT_LEN = DEBUG_TRACE_EVENT_LEN ;
const unsigned int Trace::STATE_LEN = DEBUG_TRACE_STATE_LEN ;
const unsigned int Trace::DESC_LEN = DEBUG_TRACE_DESC_LEN ;

// buffers
char Trace::Event[EVENT_LEN + 1] = {0} ; //memset(Event , ' ' , EVENT_LEN) ;
char Trace::State[STATE_LEN + 1] = {0} ;
char Trace::Desc[DESC_LEN + 1] = {0} ;

unsigned int Trace::EventLen = 0 ;
unsigned int Trace::SenderLen = 0 ;
unsigned int Trace::StateLen = 0 ;
unsigned int Trace::StateFormatLen = 0 ;
unsigned int Trace::DescLen = 0 ;
unsigned int Trace::DescFormatLen = 0 ;


void Trace::TraceScene(const char* event , const char* sender ,
								const char* stateFormat , const char* descFormat ,
								bool bool0 , bool bool1 , bool bool2 , bool isEq)
{
#if DEBUG_TRACE
#if DEBUG_TRACE_IN && DEBUG_TRACE_CLASS
printf("TRACE: Trace::TraceScene(): '%s' bool0=%d bool1=%d bool2=%d\n" , sender , bool0 , bool1 , bool2) ;
#endif // #if TRACE_IN && DEBUG_TRACE_CLASS
/*
	bool isModel = !strcmp(evType , MODEL) ; bool isModelErr = !strcmp(evType , MODEL_ERR) ;
	bool isView = !strcmp(evType , VIEW) ; bool isViewErr = !strcmp(evType , VIEW_ERR) ;
	if (isModel || isModelErr)
		{ StateFormat = MODEL_STATE_FMT ; DescFormat = (isModelErr)? MODEL_ERR_FMT : MODEL_DESC_FMT ; }
	else if (isView || isViewErr)
		{ StateFormat = VIEW_STATE_FMT ; DescFormat = (isViewErr)? VIEW_ERR_FMT : VIEW_DESC_FMT ; }

	if (isModel || isModelErr)
		{ StateFormat = MODEL_STATE_FMT ; DescFormat = (isModelErr)? MODEL_ERR_FMT : MODEL_DESC_FMT ; }
	else if (isView || isViewErr)
		{ StateFormat = VIEW_STATE_FMT ; DescFormat = (isViewErr)? VIEW_ERR_FMT : VIEW_DESC_FMT ; }
*/
/*
#if DBGIN
//printf("Trace::PrintScene() isModel=%d isModelErr=%d isView=%d isViewErr=%d\n" , isModel , isModelErr , isView , isViewErr);
printf("Trace::PrintScene() StateFormat='%s' len =%d\n" , StateFormat , strlen(StateFormat)) ;
printf("Trace::PrintScene() DescFormat='%s' len =%d\n" , DescFormat , strlen(DescFormat)) ;
	if (isModel || isModelErr || isView || isViewErr) ;
else { printf("Trace::PrintScene() evType='%s' unknown\n" , evType) ; return ; }
#endif // #if DBGIN
*/
/*
	EventLen = strlen(Sender) ; if (EventLen > EVENT_LEN) EventLen = EVENT_LEN ;
	memset(Event , ' ' , EVENT_LEN) ;
	memcpy(Event , Sender , EventLen) ; memcpy(Event+EventLen , Sender , SenderLen) ;
*/

memset(Event , ' ' , EVENT_LEN) ; memset(State , ' ' , STATE_LEN) ; memset(Desc , ' ' , DESC_LEN) ;

EventLen = strlen(event) ; if (EventLen > EVENT_LEN) EventLen = EVENT_LEN ;
memcpy(Event , event , EventLen) ;
SenderLen = strlen(sender) ; if (EventLen + SenderLen > EVENT_LEN) SenderLen = EVENT_LEN - EventLen ;
memcpy(Event+EventLen , sender , SenderLen) ;

//		StateFormatLen = strlen(stateFormat) ; if (StateFormatLen > STATE_LEN) StateFormatLen = STATE_LEN ;
char state[STATE_LEN + 1] ; memset(state , ' ' , STATE_LEN) ; // state[STATE_LEN] = '\0' ;
snprintf(state , STATE_LEN + 1 , stateFormat , bool0 , bool1 , bool2) ;
StateLen = strlen(state) ; if (StateLen > STATE_LEN) StateLen = STATE_LEN ;
memcpy(State , state , StateLen) ;

//		DescFormatLen = strlen(descFormat) ; if (DescFormatLen > DESC_LEN) DescFormatLen = DESC_LEN ;
char desc[DESC_LEN + 1] ; memset(desc , ' ' , DESC_LEN) ; desc[DESC_LEN] = '\0' ;
snprintf(desc , DESC_LEN , descFormat , bool0 , bool1 , bool2) ;
DescLen = strlen(desc) ; if (DescLen > DESC_LEN) DescLen = DESC_LEN ;
memcpy(Desc , desc , DescLen + 1) ;

/*
// printf("$2.%02d" ,  3) ;	// '$2.03		"0" padded
// printf("2 %2d" ,  3) ;		// '$2. 3'	" " padded
// printf("%6.3f", 2.8) ;		// ' 2.800'	" " padded w/ precision

//		memcpy(Event+EventLen+ClassLen , senderFunction , FunctionLen) ;
	snprintf(State , STATE_LEN + 1 , stateFormat , bool0 , bool1 , bool2) ;
	snprintf(Desc , DESC_LEN + 1 , descFormat , bool0 , bool1 , bool2) ;
*/
	Event[EVENT_LEN] = State[STATE_LEN] = Desc[DESC_LEN] = '\0' ;
	printf("%s %s %s\n" , Event , State , Desc) ;

#if DEBUG_TRACE_OUT && DEBUG_TRACE_CLASS
printf("TRACE: Trace::TraceScene(): Event(%d)='%s'\n" , strlen(Event) , Event) ; // for (int i=0 ; i < strlen(Event) ; ++i) printf("Event[%d]=%c\n" , i , Event[i]) ;
printf("TRACE: Trace::TraceScene(): State(%d)='%s'\n" , strlen(State) , State) ; // for (int i=0 ; i < strlen(State) ; ++i) printf("State[%d]=%c\n" , i , State[i]) ;
printf("TRACE: Trace::TraceScene(): Desc(%d) ='%s'\n" , strlen(Desc) , Desc) ; // for (int i=0 ; i < strlen(Desc) ; ++i) printf("Desc[%d]=%c\n" , i , Desc[i]) ;
#endif // #if TRACE_OUT && DEBUG_TRACE_CLASS
#endif // #if DEBUG_TRACE
} // Trace::TraceScene()
