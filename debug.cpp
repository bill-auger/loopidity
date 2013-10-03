
//#include "debug.h" // included in "loopidity.h"
#include "loopidity.h"


/* Trace class private constants */

const char* Trace::MODEL = DEBUG_TRACE_MODEL ;
const char* Trace::MODEL_ERR = DEBUG_TRACE_MODEL_ERR ;
const char* Trace::VIEW = DEBUG_TRACE_VIEW ;
const char* Trace::VIEW_ERR = DEBUG_TRACE_VIEW_ERR ;

const char* Trace::MODEL_STATE_FMT = DEBUG_TRACE_MODEL_STATE_FORMAT ;
const char* Trace::VIEW_STATE_FMT = DEBUG_TRACE_VIEW_STATE_FORMAT ;
const char* Trace::MODEL_DESC_FMT = DEBUG_TRACE_MODEL_DESC_FORMAT ;
const char* Trace::VIEW_DESC_FMT = DEBUG_TRACE_VIEW_DESC_FORMAT ;
const char* Trace::MODEL_ERR_FMT = DEBUG_TRACE_MODEL_ERROR_FORMAT ;
const char* Trace::VIEW_ERR_FMT = DEBUG_TRACE_VIEW_ERROR_FORMAT ;

const unsigned int Trace::EVENT_LEN = DEBUG_TRACE_EVENT_LEN ;
const unsigned int Trace::STATE_LEN = DEBUG_TRACE_STATE_LEN ;
const unsigned int Trace::DESC_LEN = DEBUG_TRACE_DESC_LEN ;


/* Trace class private variables */

// buffers
char Trace::Event[EVENT_LEN + 1] = {0} ; //memset(Event , ' ' , EVENT_LEN) ;
char Trace::State[STATE_LEN + 1] = {0} ; const char* Trace::StateFormat = 0 ;
char Trace::Desc[DESC_LEN + 1] = {0} ; const char* Trace::DescFormat = 0 ;
unsigned int Trace::EventLen = 0 , Trace::OriginLen = 0 , Trace::StateLen = 0 , Trace::DescLen = 0 ;


/* Trace class public functions */

void Trace::PrintScene(const char evType[] , const char origin[] , bool bool0 , bool bool1 , bool bool2)
{
//printf("Trace::PrintScene() evType='%s'\n" , evType) ;

#if DEBUG_TRACE

	bool isModel = !strcmp(evType , MODEL) ; bool isModelErr = !strcmp(evType , MODEL_ERR) ;
	bool isView = !strcmp(evType , VIEW) ; bool isViewErr = !strcmp(evType , VIEW_ERR) ;
	if (isModel || isModelErr)
		{ StateFormat = MODEL_STATE_FMT ; DescFormat = (isModelErr)? MODEL_ERR_FMT : MODEL_DESC_FMT ; }
	else if (isView || isViewErr)
		{ StateFormat = VIEW_STATE_FMT ; DescFormat = (isViewErr)? VIEW_ERR_FMT : VIEW_DESC_FMT ; }

/*
//printf("Trace::PrintScene() isModel=%d isModelErr=%d isView=%d isViewErr=%d\n" , isModel , isModelErr , isView , isViewErr);
printf("Trace::PrintScene() StateFormat='%s' len =%d\n" , StateFormat , strlen(StateFormat)) ;
printf("Trace::PrintScene() DescFormat='%s' len =%d\n" , DescFormat , strlen(DescFormat)) ;
if (isModel || isModelErr || isView || isViewErr) ;
	else { printf("Trace::PrintScene() evType='%s' unknown\n" , evType) ; return ; }
*/

	EventLen = strlen(evType) ; OriginLen = strlen(origin) ;
	if (EventLen > EVENT_LEN) EventLen = EVENT_LEN ;
	if (OriginLen > EVENT_LEN - EventLen) OriginLen = EVENT_LEN - EventLen ;

	memset(Event , ' ' , EVENT_LEN) ;
	memcpy(Event , evType , EventLen) ; memcpy(Event+EventLen , origin , OriginLen) ;
	snprintf(State , STATE_LEN + 1 , StateFormat , bool0 , bool1 , bool2) ;
	snprintf(Desc , DESC_LEN + 1 , DescFormat , bool0 , bool1 , bool2) ;

	Event[EVENT_LEN] = State[STATE_LEN] = Desc[DESC_LEN] = '\0' ;
	printf("%s %s %s\n" , Event , State , Desc) ;

/*
printf("Event='%s' len=%d\n" , Event , strlen(Event)) ;
printf("State='%s' len=%d\n" , State , strlen(State)) ;
for (int i=0;i<strlen(State);++i) printf("State[%d]=%c\n" , i , State[i]) ;
//	printf("ModelStatus='%s' len=%d\n" , State , strlen(ModelStatus)) ;
//	printf("ViewStatus='%s' len=%d\n" , State , strlen(ViewStatus)) ;
printf("Desc='%s' len=%d\n" , Desc , strlen(Desc)) ;
*/

#endif // #if DEBUG_TRACE
}
