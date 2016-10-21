//-----------------------------------------------------------------------------
//GDD≤‚ ‘:
//
//
//------------------------------------------------------
extern void GDD_Demo_DrawText(void);
extern void GDD_Demo_Listbox(void);
extern void GDD_Demo_MoveWindow(void);
extern void GDD_Demo_Timer(void);
extern void GDD_Demo_ZigBee(void);
extern void Hello_Word(void);
extern void GDD_Demo_Progressbar(void);
void gdd_test(void)
{
      while(1)
      {
    	Hello_Word();
    	GDD_Demo_DrawText();
        GDD_Demo_Listbox();
        GDD_Demo_MoveWindow();
        GDD_Demo_Timer();
        GDD_Demo_ZigBee();
        GDD_Demo_Progressbar();
      }

}

