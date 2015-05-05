#ifndef _MIRROR_DISPLAY_H
#define _MIRROR_DISPLAY_H

#include "rfb.h"


#ifdef  __cplusplus
extern "C"{
#endif 

//°²×°¾µÏñÏÔÊ¾Æ÷
bool_t install_vncserver_mirrordisplay(rfbScreenInfoPtr rfbserver);

void  uninstall_vncserver_mirrrordisplay(rfbScreenInfoPtr rfbserver);


#ifdef __cplusplus
}
#endif


#endif
