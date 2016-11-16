#ifndef _PL_ENVIRONMENT_H
#define _PL_ENVIRONMENT_H

#define ID_FILE_NEW 9001
#define ID_FILE_OPEN 9002
#define ID_FILE_SAVE 9003
#define ID_FILE_SEPARATOR 9004
#define ID_FILE_EXIT 9005
#define ID_STUFF_COMPILE 9006
#define ID_STUFF_RUN 9007
#define ID_FILE_CLOSE 9008

#define IDC_MAIN_EDIT 101
#define IDC_MAIN_NOTIFS 102

void createTextArea();
void createNotifs();
void clearScreen();
void newFile();
void fileOpen();
void fileSave();
int openFile();
int saveFile();
void compile();
void run();

#endif

