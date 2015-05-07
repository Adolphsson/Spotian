/*
 * util.c: Utility routines.
 */

#include "spotify.h"
#include "resource.h"

#define SPOTIFY_NONE				0
#define SPOTIFY_SHOW_TOAST			1
#define SPOTIFY_SHOW_SPOTIFY		2
#define SPOTIFY_COLPY_TRACK_INFO	3
#define SPOTIFY_PLAY_PAUSE			917504
#define SPOTIFY_MUTE				524288
#define SPOTIFY_VOLUME_DOWN			589824
#define SPOTIFY_VOLUME_UP			655360
#define SPOTIFY_STOP				851968
#define SPOTIFY_PREVIOUS_TRACK		786432
#define SPOTIFY_NEXT_TRACK			720896
#define CONFIGFILE					"spotian.xml"

/******************************************************************************
 l i s t C a l l b a c k ()
******************************************************************************/

int listCallback(int windowID, char *subwindow, char *event, void *data, void *userData) 
{
	if (!strcmp(event, "list_leftDoubleClick")) {

		/* Open a browser with our user data.
		 */

		ShellExecute(mMainWnd, "open", (char *)userData, 0, 0, SW_SHOW);
		
	}	

	return 0;
}

/******************************************************************************
 t o o l t i p C a l l b a c k ()
******************************************************************************/

int tooltipCallback(int windowID, char *subwindow, char *event, void *data, void *userData) 
{
	if (!strcmp(event, "alert_linkClick")) {
		if (!strcmp(data, "web")) {

			/* User data contains whatever we want.  NOTE that We don't free it here, though!
			 */

		}
	} else if (!strcmp(event, "alert_destroy")) {

		/* Clean up user data
		 */

		if (userData != NULL)
			free(userData);
	}

	return 0;
}

/******************************************************************************
 a l i a s C a l l b a c k ()
******************************************************************************/

int aliasCallback(int windowID, char *subwindow, char *event, void *data, void *userData) 
{
	return 1;
}

/******************************************************************************
 c o n n e c t C a l l b a c k ()
******************************************************************************/

int connectCallback(int windowID, char *subwindow, char *event, void *data, void *userData) 
{
	return 0;
}

/******************************************************************************
 c o n t a c t C a l l b a c k ()
******************************************************************************/

int contactCallback(int windowID, char *subwindow, char *event, void *data, void *userData) 
{
	return 0;
}

int systrayAlertCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	HWND hWndSpotify;
	// For now, we don't care about any of the other alert notifications, but
	// in more advanced plugins, you'll usually want to at least use the
	// alert_linkClick notification.
	/* You mean like in this über advanced plugin? ;) */
	if (!_stricmp(event, "alert_linkClick"))
	{
		if(hWndSpotify = FindWindow("SpotifyMainWindow", 0))
		{
			ShowWindow(hWndSpotify, 1);
			SetForegroundWindow(hWndSpotify);
			SetFocus(hWndSpotify);
		}
   }
   
   return 0;
}

int nowPlayingCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	return 0;
}

/******************************************************************************
 b r o a d C a l l b a c k ()
******************************************************************************/

int broadCallback(int windowID, char *subwindow, char *event, void *data, void *userData) 
{
	return 0;
}

/******************************************************************************
 u t i l _ t o o l t i p ()
******************************************************************************/

void util_tooltip(char *type, char *message, char *data)
{
	struct alert_t tooltip;
	
	/* 
	 * Does a systray alert.  The type comes back to us later, and also controls the icon.  Skins
	 * can define icons of 'type' and Trillian will use them.
	 */

	trillianInitialize(tooltip);
	
	tooltip.text = message;
	tooltip.type = type;
	tooltip.link = 1;
	tooltip.callback = tooltipCallback;
	tooltip.data = data;

	plugin_send(MYGUID, "systraySetAlert", &tooltip); 

	return;
}

/******************************************************************************
 u t i l _ w r i t e C o n f i g ()
******************************************************************************/

BOOL util_writeConfig(struct config_t* config)
{
	FILE *file;
	char* buf = (char*)malloc(_MAX_PATH);
	m_config = *config;
	_snprintf_s(buf, _MAX_PATH, _MAX_PATH - 1, "%s%s", home_directory, CONFIGFILE);
	file = fopen(buf,"w+");
	if(file == NULL)
		return FALSE;

	fprintf(file, "%s", "<spotian>\n");
	fprintf(file, "\t<notify>%s</notify>\n", (m_config.enable_notifications ? "yes" : "no"));
	fprintf(file, "\t<updateCheck>%s</updateCheck>\n", (m_config.check_for_updates ? "yes" : "no"));
	fprintf(file, "\t<ignoreAds>%s</ignoreAds>\n", (m_config.ignore_ads ? "yes" : "no"));
	fprintf(file, "\t<useTrackKey>%s</useTrackKey>\n", (m_config.use_track_key ? "yes" : "no"));
	fprintf(file, "\t<useUrlKey>%s</useUrlKey>\n", (m_config.use_url_key ? "yes" : "no"));
	fprintf(file, "%s", "</spotian>\n");

	fclose(file);

#if 0
	FILE *outfile = NULL;

	outfile = fopen(totr_config.GetConfigFilename(), "w");

	if (!outfile)
		return FALSE;

	fprintf(outfile, "<trillianotr>\r\n");
	fprintf(outfile, "  <enabled>%s</enabled>\r\n", (enabled ? "yes" : "no"));
	fprintf(outfile, "  <autonegotiate>%s</autonegotiate>\r\n", (autonegotiate ? "yes" : "no"));
	fprintf(outfile, "  <required>%s</required>\r\n", (required ? "yes" : "no"));
	fprintf(outfile, "  <autoclose>%s</autoclose>\r\n", (autoclose ? "yes" : "no"));
	fprintf(outfile, "	<savehistory>%s</savehistory>\r\n",(savehistory ? "yes" : "no"));  // Added by Thisita 10/20/2011
	fprintf(outfile, "	<historyDir>%s</historyDir>\r\n",historyDir); // Added by Thisita 10/20/11
	fprintf(outfile, "</trillianotr>\r\n");
    fclose(outfile);
#endif

	return TRUE;
}

/* Converts an integer value to its hex character*/
char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(char *str) {
  char *pstr = str, *buf = (char*)malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
      *pbuf++ = *pstr;
    else if (*pstr == ' ') 
      *pbuf++ = '+';
    else 
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

void ParseWebAPI(struct xml_tree_t *xml, struct event_t *statEvent)
{
	struct xml_tag_t* tag = NULL;
	BOOL showTrack = TRUE;
	if(xml)
		tag = xml->root_tag;

	/* Decend to our root tag */
	if(tag)
		tag = tag->children;
	else
		tag = NULL;

	/* Confirm this is our XML schema */
	if(tag && !strcmp(tag->type, "tag") && !strcmp(tag->text, "tracks"))
	{
		tag = tag->children;
	}

	/* Ok, now search! */
	while(tag)
	{
		if(!strcmp(tag->type, "tag"))
		{
			if(!strcmp(tag->text, "opensearch:totalResults") && m_config.ignore_ads)
			{
				if(tag->children)
				{
					if(!strcmp(tag->children->text, "0")) 
						showTrack = FALSE;
				}
			}
			else if(!strcmp(tag->text, "track") && m_config.use_url_key)
			{
				/* Get the href */
			}
		}

		tag = tag->next_tag;
	}

	if(showTrack)
	{
		plugin_send(MYGUID, "eventsGenerate", statEvent);
	}
}

int webApiCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	struct xml_tree_t xml;
	struct xml_tag_t* tag;
	struct xml_attribute_t* attribute;
	char* token;
	char* newVersion;
	char* projectUrl;
	char* productId;
	struct http_result_t* result = 0;
	struct event_t* songNotification = (struct event_t*)userData;
	result = (struct http_result_t*)data;

	trillianInitialize(xml);
	
	xml.data = _strdup(result->buffer);

	if (!plugin_send(MYGUID, "xmlGenerateTree", &xml)) {
		ParseWebAPI(&xml, songNotification);
	}
	else
	{
		/* Silent fail */
	}

	plugin_send(MYGUID, "xmlFreeTree", &xml);
}


void ParseSettings(struct xml_tree_t *xml)
{
	struct xml_tag_t* tag = NULL;
	if(xml)
		tag = xml->root_tag;

	/* Decend to our root tag */
	if(tag)
		tag = tag->children;
	else
		tag = NULL;

	/* Confirm this is our XML schema */
	if(tag && !strcmp(tag->type, "tag") && !strcmp(tag->text, "spotian"))
	{
		tag = tag->children;
	}

	/* Ok, now search! */
	while(tag)
	{
		if(!strcmp(tag->type, "tag"))
		{
			if(!strcmp(tag->text, "notify"))
			{
				if(tag->children)
				{
					if(!strcmp(tag->children->text, "yes")) 
						m_config.enable_notifications = 1;
					else
						m_config.enable_notifications = 0;
				}
			}
			else if(!strcmp(tag->text, "updateCheck"))
			{
				if(tag->children)
				{
					if(!strcmp(tag->children->text, "yes")) 
						m_config.check_for_updates = 1;
					else
						m_config.check_for_updates = 0;
				}
			}
			else if(!strcmp(tag->text, "ignoreAds"))
			{
				if(tag->children)
				{
					if(!strcmp(tag->children->text, "yes")) 
						m_config.ignore_ads = 1;
					else
						m_config.ignore_ads = 0;
				}
			}
			else if(!strcmp(tag->text, "useUrlKey"))
			{
				if(tag->children)
				{
					if(!strcmp(tag->children->text, "yes")) 
						m_config.use_url_key = 1;
					else
						m_config.use_url_key = 0;
				}
			}
			else if(!strcmp(tag->text, "useTrackKey"))
			{
				if(tag->children)
				{
					if(!strcmp(tag->children->text, "yes")) 
						m_config.use_track_key = 1;
					else
						m_config.use_track_key = 0;
				}
			}
		}

		tag = tag->next_tag;
	}
}

/******************************************************************************
 u t i l _ r e a d C o n f i g ()
******************************************************************************/

BOOL util_readConfig(struct config_t* c)
{
	struct xml_tree_t xml; // Rewritten by Thisita 10/23/11
	char* buf;

	trillianInitialize(xml);

	buf = (char*)malloc(_MAX_PATH);
	_snprintf_s(buf, _MAX_PATH, _MAX_PATH - 1, "%s%s", home_directory, CONFIGFILE);
	xml.filename = _strdup(buf);
	free(buf);

	if (!plugin_send(MYGUID, "xmlGenerateTree", &xml)) {
		ParseSettings(&xml);
	}
	else
	{
		/* Default settings */
		m_config.enable_notifications = TRUE;
		m_config.check_for_updates = TRUE;
		m_config.ignore_ads = TRUE;
		m_config.use_track_key = FALSE;
		m_config.use_url_key = FALSE;
	}

	*c = m_config;
	plugin_send(MYGUID, "xmlFreeTree", &xml);
	return TRUE;
}

int updateCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	struct browser_location_t browser;
		
	if (!_stricmp(event, "alert_linkClick"))
	{
		trillianInitialize(browser);

		browser.location = (char*)userData;
		browser.new_window = TRUE;
		browser.check_security = 1;
		plugin_send(MYGUID, "browserOpen", &browser);
	}

	return 0;
}

void showUpdateAlert(char* ver, char* url)
{
	struct alert_t alert;
	trillianInitialize(alert);
	alert.text = (char*)malloc(sizeof(char)*128);
	sprintf(alert.text, "New version available!<br>Update Spotian %s to <b>%s</b>.", shortver, ver);
	alert.type = "plugin_update";
	alert.callback = updateCallback;
	alert.data = _strdup(url);
	plugin_send(MYGUID, "systraySetAlert", &alert);
}

int httpCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	struct xml_tree_t xml;
	struct xml_tag_t* tag;
	struct xml_attribute_t* attribute;
	char* token;
	char* newVersion;
	char* projectUrl;
	char* productId;
	struct http_result_t* result = (struct http_result_t*)data;

	trillianInitialize(xml);
	
	xml.data = _strdup(result->buffer);

	if (!plugin_send(MYGUID, "xmlGenerateTree", &xml)) {
		tag = xml.root_tag;
		/* Decend to our root tag */
		if(tag)
			tag = tag->children;
		else
			tag = NULL;

		/* traverse to spotians product id */
		while(tag)
		{
			if(!strcmp(tag->type, "tag"))
			{
				if(!strcmp(tag->text, "xsa"))
					tag = tag->children;
				if(!strcmp(tag->text, "product"))
				{
					attribute = tag->attributes;
					while(attribute)
					{
						if(!strcmp(attribute->name, "id"))
						{
							productId = attribute->value;
							break;
						}
						attribute = attribute->next_attribute;
					}
					if(!strcmp(productId, "1302"))
					{
						tag = tag->children;
						break;
					}
				}
			}
			tag = tag->next_tag;
		}

		newVersion = NULL;

		/* Ok, now search! */
		while(tag)
		{
			if(!strcmp(tag->type, "tag"))
			{
				if(!strcmp(tag->text, "version"))
				{
					if(tag->children)
					{
						if(((token = strtok(_strdup(tag->children->text), ".")) && atoi(token) > majver) ||
						   ((token = strtok(NULL, "."))      && atoi(token) > minver) ||
						   ((token = strtok(NULL, "."))      && atoi(token) > patch))
							newVersion = _strdup(tag->children->text);
					}
				}
				else if(!strcmp(tag->text, "info-url"))
				{
					if(tag->children)
					{
						projectUrl = _strdup(tag->children->text);
					}
				}
			}

			tag = tag->next_tag;
		}

		if(newVersion)
			showUpdateAlert(newVersion, projectUrl);
	}
	else
	{
		/* Silent fail */
	}

	plugin_send(MYGUID, "xmlFreeTree", &xml);
}

int my_strlen_utf8_c(char *s)
{
	int i = 0, j = 1;
	while (s[i])
	{
		if ((int)((unsigned char)s[i]) >= 128)
			j++;
		j++;
		i++;
	}
	return j;
}

void EncodeUTF8(const char** utf8)
{
	int i;
	char* result = (char*)malloc(my_strlen_utf8_c(*utf8));
	result[0] = '\0';

	for (i = 0; i < strlen(*utf8); i++)
	{
		if((int)((unsigned char)(*utf8)[i]) >= 192)
		{
			sprintf(result, "%s%c", result, 0xC3);
			sprintf(result, "%s%c", result, (int)((unsigned char)(*utf8)[i]) - 0x40);
		}
		else if ((int)((unsigned char)(*utf8)[i]) >= 128)
		{
			sprintf(result, "%s%c", result, 0xC2);
			sprintf(result, "%s%c", result, (int)((unsigned char)(*utf8)[i]));
		}
		else
			sprintf(result, "%s%c", result, (*utf8)[i]); // Copy untranslated
	}

	free(*utf8);
	*utf8 = result;
}

void CheckSpotify(int eventID)
{
	int length;
	char* chNewSong = 0;
	char* chOldSong = 0;
	char* chArtist = 0;
	char* chAlbum = 0;
	char* chSong = 0;
	char* chSongAlert = 0;
	int* hWndSpotify = 0;
	int i;
	char* encodedArtist = 0;
	char* encodedTrack = 0;

	struct awaymessage_entry_t  msg;
	struct alert_t              alert;
	struct event_t              statEvent;
	struct event_variables_t    meh[3];
	struct http_request_t       hr;
	
	trillianInitialize(msg);
	trillianInitialize(alert);
	trillianInitialize(statEvent);
	trillianInitialize(hr);

	statEvent.type = "Events: Song Change";
	statEvent.discrete = 1;
	statEvent.variables = meh;

	trillianInitialize(meh[0]);
	meh[0].variable_type = "string";
	meh[0].next_evt = &meh[1];
	meh[0].variable_name = "song";
	meh[0].variable_data = "";
	meh[0].variable_size = 0;

	trillianInitialize(meh[1]);
	meh[1].variable_type = "string";
	meh[1].next_evt = &meh[2];
	meh[1].variable_name = "artist";
	meh[1].variable_data = "";
	meh[1].variable_size = 0;

	trillianInitialize(meh[2]);
	meh[2].variable_type = "string";
	meh[2].next_evt = 0;
	meh[2].variable_name = "album";
	meh[2].variable_data = "";
	meh[2].variable_size = 0;

	alert.type = "music";
	alert.callback = systrayAlertCallback;

	while(!m_exit)
	{
		if(hWndSpotify = FindWindow("SpotifyMainWindow", 0))
		{
			length = GetWindowTextLength(hWndSpotify);
			if(chNewSong)	free(chNewSong);
			chNewSong = (char*)malloc(length+1);
			GetWindowText(hWndSpotify, chNewSong, length+1);
			chNewSong[length] = '\0';
			if(length < 10)
				chNewSong[0] = '\0';
			else
				strcpy(chNewSong, chNewSong+10);

			/* Don't update if player is paused */
			//if(chNewSong[0] != '\0')
			//{
				if(chOldSong && strcmp(chOldSong, chNewSong) != 0)
				{
					chSongAlert = (char*)malloc(chNewSong);
					chSongAlert[0] = '\0';
					strcpy(chSongAlert, chNewSong);
					for(i = 0; i < strlen(chSongAlert); i++)
						if(chSongAlert[i] == -106)
							chSongAlert[i] = '-';
					EncodeUTF8(&chSongAlert);
					alert.text = chSongAlert;
					alert.data = chSongAlert;
					if(chNewSong[0] != '\0')
					{
						if(m_config.enable_notifications)
							plugin_send(MYGUID, "systraySetAlert", &alert);

						if(chSong)		free(chSong);
						if(chAlbum)		free(chAlbum);
						if(chArtist)	free(chArtist);

						for(i = 0; i < strlen(chNewSong); i++)
							if(chNewSong[i] == -106)
								break;

						if(i == strlen(chNewSong))
						{
							// No delimiter found
							chArtist = (char*)malloc(1);
							chArtist[0] = '\0';
							chSong = (char*)malloc(1);
							chSong[0] = '\0';
						}
						else
						{
							chArtist = (char*)malloc(i);
							chSong   = (char*)malloc(strlen(chNewSong) - i - 1);

							strncpy(chArtist, chNewSong, i-1);
							chArtist[i-1] = '\0';

							strcpy(chSong, chNewSong + i + 2);

							for(i = 0; i < strlen(chArtist); i++)
								if(chArtist[i] == -106)
									chArtist[i] = '-';
							for(i = 0; i < strlen(chSong); i++)
								if(chSong[i] == -106)
									chSong[i] = '-';
							EncodeUTF8(&chArtist);
							EncodeUTF8(&chSong);
						}
					}
					else
					{
						chArtist = (char*)malloc(1);
						chArtist[0] = '\0';
						chSong = (char*)malloc(1);
						chSong[0] = '\0';
					}

					chAlbum = (char*)malloc(1);
					chAlbum[0] = '\0';

					statEvent.variables[0].variable_data = chSong;
					statEvent.variables[0].variable_size = strlen(chSong);
					statEvent.variables[1].variable_data = chArtist;
					statEvent.variables[1].variable_size = strlen(chArtist);
					statEvent.variables[2].variable_data = chAlbum;
					statEvent.variables[2].variable_size = strlen(chAlbum);

					if(m_config.use_url_key || m_config.ignore_ads)
					{
						/* We need to ask the web API */
						hr.url = (char*)malloc(sizeof(char)*1024);
						encodedArtist = url_encode(chArtist);
						encodedTrack = url_encode(chSong);
						sprintf(hr.url, "http://ws.spotify.com/search/1/track?q=artist%%3A%%22%s%%22track%%3A%%22%s%%22", encodedArtist, encodedTrack);
						hr.callback = webApiCallback;
						hr.data = &statEvent;
						plugin_send(MYGUID, "httpFileRequest", &hr);
						free(encodedArtist);
						free(encodedTrack);
					}
					else
					{
						plugin_send(MYGUID, "eventsGenerate", &statEvent);
					}
				}

				if(chOldSong)	free(chOldSong);
				chOldSong = (char*)malloc(length+1);
				strcpy(chOldSong, chNewSong);
			//}
		}
		Sleep(500);
	}

	if(chNewSong)		free(chNewSong);
	if(chOldSong)		free(chOldSong);
	if(chSong)	free(chSong);
	if(chAlbum)	free(chAlbum);
	if(chArtist)	free(chArtist);

	//plugin_send(MYGUID, "eventsStatusUnregister", &statEvent);

	Sleep(500);
}
