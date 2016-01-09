/*
 * util.c: Utility routines.
 */

#include "spotify.h"
#include "resource.h"

#include "jsmn.h"

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

	if (strcmp(event, "http_fileComplete") != 0)
		return 0;
	if (result->buffer_size < 1 || result->buffer == NULL)
		return 0;
#if 0
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

#endif

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






void gen_random(char *s, const int len) {
	static const char alpha[] = "abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alpha[rand() % (sizeof(alpha) - 1)];
	}

	s[len] = 0;
}

int localport = -1;
char *oauth_token = 0;
char *csrf_token = 0;
void getSpotifyUrl(char *s, int port, char *path)
{
	char subdomain[10];

	gen_random(subdomain, 10);
	/*sprintf(s, "https://%s.spotilocal.com:%d%s", subdomain, port, path);*/
	sprintf(s, "https://localhost:%d%s", port, path);
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

jsmntok_t * json_tokenise(char *js)
{
	jsmn_parser parser;
	jsmn_init(&parser);

	unsigned int n = 256;
	jsmntok_t *tokens = malloc(sizeof(jsmntok_t) * n);
	/*log_null(tokens);*/
	if (tokens == 0)
		MessageBox(0, "Tokens is null", 0, 0, 0);

	int ret = jsmn_parse(&parser, js, /*strlen(js),*/ tokens, n);

	while (ret == JSMN_ERROR_NOMEM)
	{
		n = n * 2 + 1;
		tokens = realloc(tokens, sizeof(jsmntok_t) * n);
		/*log_null(tokens);*/
		if (tokens == 0)
			MessageBox(0, "Tokens is null", 0, 0, 0);
		ret = jsmn_parse(&parser, js, /*strlen(js),*/ tokens, n);
	}

	if (ret == JSMN_ERROR_INVAL)
		/*log_die("jsmn_parse: invalid JSON string");*/
		MessageBox(0, "jsmn_parse: invalid JSON string", 0, 0, 0);
	if (ret == JSMN_ERROR_PART)
		/*log_die("jsmn_parse: truncated JSON string");*/
		MessageBox(0, "jsmn_parse: truncated JSON string", 0, 0, 0);

	return tokens;
}

int parseroot(char* js, char* key, char** value)
{
	jsmntok_t *tokens = 0;
	int i, j;
	jsmn_parser parser;

	tokens = json_tokenise(js);

	for (i = 0, j = 1; j > 0; i++, j--)
	{
		if (tokens[i].start == -1 || tokens[i].end == -1)
		{
			MessageBox(0, "Should never reach uninitialized tokens", 0, 0, 0);
			break;
		}

		if (tokens[i].type == JSMN_ARRAY || tokens[i].type == JSMN_OBJECT)
			j += tokens[i].size;

		if (jsoneq(js, &tokens[i], key) == 0) {
			i++;
			/* We may use strndup() to fetch string value */
			if (*value) free(*value);
			*value = (char*)malloc(tokens[i].end - tokens[i].start + 1);
			sprintf(*value, "%.*s", tokens[i].end - tokens[i].start, js + tokens[i].start);
			/*oauth_token = strndup(js + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);*/

			if (tokens)	free(tokens);
			return 0;
		}
	}

	if (tokens)	free(tokens);
	return 1;
}

int httpOauthTokenCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	char* js = 0;
	struct http_result_t* result = (struct http_result_t*)data;
	
	if (strcmp(event, "http_fileComplete") != 0)
		return 0;
	if (result->buffer_size < 1 || result->buffer == NULL)
		return 0;

	js = _strdup(result->buffer);
	parseroot(js, "t", &oauth_token);

	if (js)	free(js);
}

void getOauthToken()
{
	struct http_request_t* req = (struct http_request_t*)malloc(sizeof(struct http_request_t));

	trillianInitialize((*req));
	
	req->url = "https://open.spotify.com/token";
	req->callback = httpOauthTokenCallback;
	plugin_send(MYGUID, "httpFileRequest", req);
}

int indexOf(char *s, char ch)
{
	char *e;

	e = strchr(s, ch);
	return (int)(e - s);
}

int httpCsrfCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	char* js = 0;
	struct http_result_t* result = (struct http_result_t*)data;

	if (strcmp(event, "http_fileComplete") != 0)
		return 0;
	if (result->buffer_size < 1 || result->buffer == NULL)
		return 0;

	if (localport < 0)
	{
		char subbuff[5];
		memcpy(subbuff, &(result->url)[18], 4);
		subbuff[4] = '\0';
		localport = atoi(subbuff);
	}
	
	js = _strdup(result->buffer);
	parseroot(js, "token", &csrf_token);

	if (js)	free(js);
}

void getCsrfToken()
{
	struct http_request_t* req = 0;

	if (localport < 0)
	{
		/* Let's find our local port by brute force */
		for (int i = 4370; i <= 4380; i++)
		{
			if (localport > 0)
				break;

			req = (struct http_request_t*)malloc(sizeof(struct http_request_t));
			trillianInitialize((*req));
			req->url = (char*)malloc(sizeof(char) * 64);

			getSpotifyUrl(req->url, i, "/simplecsrf/token.json");
			/*MessageBox(0, req->url, 0, 0);*/
			req->callback = httpCsrfCallback;
			req->http_header = "Origin: https://open.spotify.com";
			plugin_send(MYGUID, "httpFileRequest", req);

			/*Sleep(5000);*/
		}
	}
	else
	{
		req = (struct http_request_t*)malloc(sizeof(struct http_request_t));
		trillianInitialize((*req));
		req->url = (char*)malloc(sizeof(char) * 64);

		getSpotifyUrl(req->url, localport, "/simplecsrf/token.json");
		req->callback = httpCsrfCallback;
		/*MessageBox(0, req->http_header, 0, 0);*/
		req->http_header = "Origin: https://open.spotify.com";
		plugin_send(MYGUID, "httpFileRequest", req);
	}
}

int json_token_streq(char *js, jsmntok_t *t, char *s)
{
	return (strncmp(js + t->start, s, t->end - t->start) == 0
		&& strlen(s) == (size_t)(t->end - t->start));
}

int parseSongInfo(char* js, char** song, char** album, char** artist, short *isPlaying)
{
	typedef enum {
		START,
		ROOT,
		PLAYING,
		START_TRACK,
		TRACK,
		START_RESOURCE,
		TRACK_RESOURCE,
		ARTIST_RESOURCE,
		ALBUM_RESOURCE,
		NAME,
		STOP,
		SKIP
	} parse_state;
	jsmntok_t *tokens = 0;

	// state is the current state of the parser
	parse_state state = START;

	// stack is the state we return to when reaching the end of an object
	parse_state stack = START;

	// Counters to keep track of how far through parsing we are
	size_t start_tokens = 0;
	size_t skip_tokens = 0;
	size_t track = 0;
	size_t track_tokens = 0;
	size_t root_tokens = 0;
	char* ch = 0;
	char** resource = 0;
	jsmntok_t *t = 0;
	size_t i = 0, j = 0;

	/* The Spotify status API response is in this format:
	*
	*{
	*  ...,
	*  "playing": true,
	*  ...,
	*  "track": {
	*    "track_resource": {
	*      "name": "TRACK NAME",
	*      ...
	*    },
	*    "artist_resource": {
	*      "name": "ARTIST NAME",
	*      ...
	*    },
	*    "album_resource": {
	*      "name": "ALBUM NAME",
	*      ...
	*    },
	*    ...
	*  }
	*}
	*
	*/

	tokens = json_tokenise(js);

	for (i = 0, j = 1; j > 0; i++, j--)
	{
		t = &tokens[i];

		// Should never reach uninitialized tokens
		/*log_assert(t->start != -1 && t->end != -1);*/
		if (t->start == -1 || t->end == -1)
		{
			MessageBox(0, "Should never reach uninitialized tokens", 0, 0, 0);
			break;
		}

		if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
			j += t->size;

		switch (state)
		{
		case START:
			if (t->type != JSMN_OBJECT)
				MessageBox(0, "Root element must be an object.", 0, 0, 0);
			state = ROOT;
			root_tokens = t->size;

			if (root_tokens == 0)
				state = STOP;

			if (root_tokens % 2 != 0)
				MessageBox(0, "Object must have even number of children.", 0, 0, 0);
			break;
		case ROOT:
			root_tokens--;

			if (root_tokens % 2 == 1)
			{
				if (t->type != JSMN_STRING)
					MessageBox(0, "Object keys must be strings.", 0, 0, 0);

				if (ch) free(ch);
				ch = (char*)malloc(t->end - t->start + 1);
				sprintf(ch, "%.*s", t->end - t->start, js + t->start, root_tokens);
				/*MessageBox(0, ch, 0, 0, 0);*/

				if (strcmp(ch, "playing") == 0)
				{
					stack = state;
					state = PLAYING;
				}
				else if (strcmp(ch, "track") == 0)
				{
					stack = state;
					state = START_TRACK;
				}
			}
			else if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
			{
				state = SKIP;
				stack = state;
				skip_tokens = t->size;

				/* BAH! Screw everything after context! */
				if (ch)	free(ch);
				if (t)	free(t);
				return;
			}

			if (root_tokens == 0)
				state = STOP;
			break;
		case PLAYING:
			if (ch) free(ch);
			ch = (char*)malloc(t->end - t->start + 1);
			sprintf(ch, "%.*s", t->end - t->start, js + t->start);

			*isPlaying = (strcmp(ch, "false") == 0) ? 0 : 1;
			state = ROOT;
			root_tokens--;
			break;
		case START_TRACK:
			if (t->type != JSMN_OBJECT)
				MessageBox(0, "Track element must be an object.", 0, 0, 0);
			state = TRACK;
			track = t->size;

			if (track == 0)
				state = ROOT;

			if (track % 2 != 0)
				MessageBox(0, "Object must have even number of children.", "TRACK", 0, 0);
			break;
		case TRACK:
			track--;

			if (track % 2 == 1)
			{
				if (t->type != JSMN_STRING)
					MessageBox(0, "Object keys must be strings.", "TRACK", 0, 0);

				if (ch) free(ch);
				ch = (char*)malloc(t->end - t->start + 1);
				sprintf(ch, "%.*s", t->end - t->start, js + t->start);

				if (json_token_streq(js, t, "track_resource"))
				{
					stack = TRACK_RESOURCE;
					state = START_RESOURCE;
				}
				else if (json_token_streq(js, t, "artist_resource"))
				{
					stack = ARTIST_RESOURCE;
					state = START_RESOURCE;
				}
				else if (json_token_streq(js, t, "album_resource"))
				{
					stack = ALBUM_RESOURCE;
					state = START_RESOURCE;
				}
			}
			else if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
			{
				if (ch) free(ch);
				ch = (char*)malloc(t->end - t->start + 1);
				sprintf(ch, "OBJ: %.*s %d", t->end - t->start, js + t->start, root_tokens);
				MessageBox(0, ch, "TRACK", 0, 0);
				stack = state;
				state = SKIP;
				skip_tokens = t->size;
			}

			if (track == 0)
			{
				state = ROOT;
				root_tokens--;
			}
			break;
		case START_RESOURCE:
			if (t->type != JSMN_OBJECT)
				MessageBox(0, "Resource element must be an object.", 0, 0, 0);

			state = stack;
			stack = TRACK;

			track_tokens = t->size;

			if (track_tokens == 0)
				state = TRACK;

			if (track_tokens % 2 != 0)
				MessageBox(0, "Object must have even number of children.", "TRACK_TOKENS", 0, 0);
			break;
		case TRACK_RESOURCE:
		case ARTIST_RESOURCE:
		case ALBUM_RESOURCE:
			track_tokens--;

			if (track_tokens % 2 == 1)
			{
				if (t->type != JSMN_STRING)
					MessageBox(0, "Object keys must be strings.", "TRACK_TOKENS", 0, 0);

				if (json_token_streq(js, t, "name"))
				{
					if (state == TRACK_RESOURCE)
						resource = song;
					else if (state == ARTIST_RESOURCE)
						resource = artist;
					else
						resource = album;

					stack = state;
					state = NAME;
				}
			}
			else if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
			{
				stack = state;
				state = SKIP;
				skip_tokens = t->size;
			}

			if (track_tokens == 0)
			{
				if (state == SKIP)
				{
					stack = TRACK;
					track--;
				}
				else
					state = TRACK;
			}
			break;
		case NAME:
			track_tokens--;

			*resource = (char*)malloc(t->end - t->start + 1);
			sprintf(*resource, "%.*s", t->end - t->start, js + t->start);
			resource = 0;

			state = stack;
			break;
		case STOP:
			// Just consume the tokens
			break;
		case SKIP:
			skip_tokens--;

			if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
				skip_tokens += t->size;

			if (skip_tokens == 0)
				state = stack;

			break;
		}
	}
}

char *str_replace(char *orig, char *rep, char *with) {
	char *result; // the return string
	char *ins;    // the next insert point
	char *tmp;    // varies
	int len_rep;  // length of rep
	int len_with; // length of with
	int len_front; // distance between rep and end of last rep
	int count;    // number of replacements

	if (!orig)
		return NULL;
	if (!rep)
		rep = "";
	len_rep = strlen(rep);
	if (!with)
		with = "";
	len_with = strlen(with);

	ins = orig;
	for (count = 0; tmp = strstr(ins, rep); ++count) {
		ins = tmp + len_rep;
	}

	// first time through the loop, all the variable are set correctly
	// from here on,
	//    tmp points to the end of the result string
	//    ins points to the next occurrence of rep in orig
	//    orig points to the remainder of orig after "end of rep"
	tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

	if (!result)
		return NULL;

	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep; // move to next "end of rep"
	}
	strcpy(tmp, orig);
	return result;
}

char* chOldSong = 0;
int httpSpotifyCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	char* js = 0;

	char* chNewSong = 0;
	char* chArtist = 0;
	char* chAlbum = 0;
	char* chSong = 0;
	char* chSongAlert = 0;
	short isPlaying = 0;
	

	struct alert_t              alert;
	struct event_t              statEvent;
	struct event_variables_t    meh[3];
	struct http_result_t* result = 0;

	if (strcmp(event, "http_fileComplete") != 0)
		return 0;
	result = (struct http_result_t*)data;
	if (result->buffer_size < 1 || result->buffer == NULL)
		return 0;

	js = _strdup(result->buffer);

	parseSongInfo(js, &chSong, &chAlbum, &chArtist, &isPlaying);	
	
	trillianInitialize(alert);
	trillianInitialize(statEvent);

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

	if (chNewSong) free(chNewSong);
	chNewSong = (char*)malloc(strlen(chSong) + 4 + strlen(chAlbum) + 4 + strlen(chArtist) + 1);
	sprintf(chNewSong, "%s<br>%s<br>%s", chArtist, chSong, chAlbum);
	
	if (isPlaying)
	{
		if (chOldSong == 0 || strcmp(chOldSong, chNewSong) != 0)
		{
			alert.type = "music";
			alert.callback = systrayAlertCallback;

			chSongAlert = (char*)malloc(strlen(chNewSong) + 1);
			chSongAlert[strlen(chNewSong)] = '\0';
			strcpy(chSongAlert, chNewSong);
			alert.text = chSongAlert;
			/*alert.data = chSongAlert;*/
			if (chNewSong[0] != '\0')
			{
				if (m_config.enable_notifications)
					plugin_send(MYGUID, "systraySetAlert", &alert);
			}

			statEvent.variables[0].variable_data = chSong;
			statEvent.variables[0].variable_size = strlen(chSong);
			statEvent.variables[1].variable_data = chArtist;
			statEvent.variables[1].variable_size = strlen(chArtist);
			statEvent.variables[2].variable_data = chAlbum;
			statEvent.variables[2].variable_size = strlen(chAlbum);

			plugin_send(MYGUID, "eventsGenerate", &statEvent);

			if (chOldSong) free(chOldSong);
			chOldSong = (char*)malloc(strlen(chSong) + 4 + strlen(chAlbum) + 4 + strlen(chArtist) + 1);
			sprintf(chOldSong, "%s<br>%s<br>%s", chArtist, chSong, chAlbum);
		}
	}
	else
	{
		statEvent.variables[0].variable_data = "";
		statEvent.variables[0].variable_size = 0;
		statEvent.variables[1].variable_data = "";
		statEvent.variables[1].variable_size = 0;
		statEvent.variables[2].variable_data = "";
		statEvent.variables[2].variable_size = 0;

		plugin_send(MYGUID, "eventsGenerate", &statEvent);
	}
	
	
	/*
if (chOldSong)	free(chOldSong);
chOldSong = (char*)malloc(val[i].len - 1);
strcpy(chOldSong, chNewSong);*/
	
	if (chNewSong)		free(chNewSong);
	if (chArtist)		free(chArtist);
	if (chAlbum)		free(chAlbum);
	if (chSong)			free(chSong);
	if (chSongAlert)	free(chSongAlert);
	if (js)				free(js);
	/* Pointer to parameter, don't free this */
	/*if (result)			free(result);*/
}

void CheckSpotify(int eventID)
{
	int length;
	int* hWndSpotify = 0;
	int i;
	char* encodedArtist = 0;
	char* encodedTrack = 0;

	char* oauthToken = 0;
	char* csrfToken = 0;

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

	hr.url = (char*)malloc(sizeof(char) * 64);
	getOauthToken();
	getCsrfToken();

	while(!m_exit)
	{
		if (localport > 0 && oauth_token && csrf_token)
		{
			getSpotifyUrl(hr.url, localport, "/remote/status.json");
			hr.http_header = "Origin: https://open.spotify.com";
			hr.callback = httpSpotifyCallback;
			hr.content_type = 
			sprintf(hr.url, "%s?oauth=%s&csrf=%s&returnafter=0&returnon=login%%2Clogout%%2Cplay%%2Cpause%%2Cerror%%2Cap", hr.url, oauth_token, csrf_token);
			plugin_send(MYGUID, "httpFileRequest", &hr);
		}

		Sleep(5000);
	}

	//plugin_send(MYGUID, "eventsStatusUnregister", &statEvent);

	Sleep(500);
}
