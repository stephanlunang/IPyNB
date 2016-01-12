#include <cvirte.h>		/* Needed if linking in external compiler; harmless otherwise */
#include <userint.h>
#include "whsptest.h"

#include "alexmisc.h"

static int panelHandle;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)	/* Needed if linking in external compiler; harmless otherwise */
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "whsptest.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	RunUserInterface ();
	return 0;
}





int CVICALLBACK strip (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char String[256];
	
	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlVal ( panel, PANEL_INPUTSTRING, String);
			
			
			strip_white_ends_from_string( String ) ;         
			
			
			SetCtrlVal ( panel, PANEL_OUTPUTSTRING, String );       


			break;
		
	}
	return 0;
}




int CVICALLBACK quit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface (0);
			break;
		
	}
	return 0;
}
