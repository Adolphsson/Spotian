/*
 * spotify.h: Header for spotify plugin.
 */

#ifndef _SPOTIFY_H
#define _SPOTIFY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <WinBase.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <commctrl.h>

/* Always update your plugin.h to be current.
 */

#include "plugin.h"

/* Every plugin has it's own GUID.
 */

#define MYGUID							"C9E5EC2A-CC95-11DF-95EC-91F5DED72085"
#define MYNAME							"Spotian"
#define COMPANY							"Christoffer Adolphsson"
#define DESCRIPTION						"A small Trillian plug-in to show currently played Spotify song in Trillian's status field. Works with both Astra and regular Trillian."

#define MENU_TYPE_SPOTIFY				0

#define IDM_SPOTIFY						4000

struct config_t
{
  BOOL enable_notifications;
  BOOL check_for_updates;
  BOOL ignore_ads;
  BOOL use_track_key;
  BOOL use_url_key;
};

typedef int (*ttkCallback)(int windowID, char *subwindow, char *event, void *data, void *userData); 
typedef int (*plugin_function_main)(char *event, void *data); 
typedef int (*plugin_function_send)(char *guid, char *event, void *data);

#if _DEBUG
int					main();
#endif

int					spotify_main();

void				util_tooltip();
BOOL				util_writeConfig(struct config_t* config);
BOOL				util_readConfig(struct config_t* config);

BOOL				prefResult(struct config_t* config);
BOOL				prefInit(struct config_t* config);
int					getCenter();

int					listCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
int					aliasCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
int					connectCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
int					contactCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
int					nowPlayingCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
int					broadCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
int					systrayAlertCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
int					httpCallback(int windowID, char *subwindow, char *event, void *data, void *userData);

void				CheckSpotify(int eventID);

HWND				myCreateDialog(HINSTANCE hInstance, LPCTSTR templateStr, HWND parent, DLGPROC dlg);
void				myDestroyWindow(HWND hWnd);

extern HINSTANCE	hInstance;
HWND				mMainWnd;
void __declspec( dllexport )unload();
plugin_function_send		plugin_send;

extern HWND			prefs_window;

extern char			*home_directory;
extern int			parentID;
extern int			sectionID;
int m_exit;
struct config_t m_config;
char* version;
char* shortver;
char* updatever;
int		majver;
int		minver;
int		patch;
int		build;

#endif

