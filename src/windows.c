/*
 * windows.c: Windows routines
 */

#include "spotify.h"
#include "resource.h"

HWND		prefs_window = NULL;
HWND		advanced_prefs_window = NULL;

/******************************************************************************
 p r e f D i a l o g ()
******************************************************************************/

int prefDialog(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {

		case WM_INITDIALOG:
			break;

		case WM_COMMAND:

			switch ((int)LOWORD(wparam)) {

				default:
					break;

			}

			break;
	}

	return 0;
}


/******************************************************************************
 p r e f R e s u l t ()
******************************************************************************/

BOOL prefResult(struct config_t* config)
{
	if(prefs_window != NULL)
	{
		HWND hwnd = GetDlgItem(prefs_window, IDC_NOTIFS_CHECK);
		config->enable_notifications = (BOOL)SendMessage(hwnd, BM_GETCHECK, 0, 0);

		hwnd = GetDlgItem(prefs_window, IDC_UPDATE_CHECK);
		config->check_for_updates = (BOOL)SendMessage(hwnd, BM_GETCHECK, 0, 0);

		return TRUE;
	}

	return FALSE;
}

BOOL prefInit(struct config_t* config)
{
	if(prefs_window != NULL)
	{
		HWND hwnd = GetDlgItem(prefs_window, IDC_NOTIFS_CHECK);
		SendMessage(hwnd, BM_SETCHECK, config->enable_notifications ? BST_CHECKED : BST_UNCHECKED, 0);
		
		hwnd = GetDlgItem(prefs_window, IDC_UPDATE_CHECK);
		SendMessage(hwnd, BM_SETCHECK, config->check_for_updates ? BST_CHECKED : BST_UNCHECKED, 0);

		return TRUE;
	}

	return FALSE;
}

/******************************************************************************
 a d v a n c e d P r e f R e s u l t ()
******************************************************************************/

BOOL advancedPrefResult(struct config_t* config)
{
	if(advanced_prefs_window != NULL)
	{
		HWND hwnd = GetDlgItem(advanced_prefs_window, IDC_NOTIFS_CHECK);
		config->enable_notifications = (BOOL)SendMessage(hwnd, BM_GETCHECK, 0, 0);

		hwnd = GetDlgItem(advanced_prefs_window, IDC_UPDATE_CHECK);
		config->check_for_updates = (BOOL)SendMessage(hwnd, BM_GETCHECK, 0, 0);

		return TRUE;
	}

	return FALSE;
}

BOOL advancedPrefInit(struct config_t* config)
{
	if(advanced_prefs_window != NULL)
	{
		HWND hwnd = GetDlgItem(advanced_prefs_window, IDC_NOTIFS_CHECK);
		SendMessage(hwnd, BM_SETCHECK, config->enable_notifications ? BST_CHECKED : BST_UNCHECKED, 0);
		
		hwnd = GetDlgItem(advanced_prefs_window, IDC_UPDATE_CHECK);
		SendMessage(hwnd, BM_SETCHECK, config->check_for_updates ? BST_CHECKED : BST_UNCHECKED, 0);

		return TRUE;
	}

	return FALSE;
}

/******************************************************************************
 m y C r e a t e D i a l o g ()
******************************************************************************/

HWND myCreateDialog(HINSTANCE hInstance, LPCTSTR templateStr, HWND parent, DLGPROC dlg)
{
	struct dialog_entry_t		det;

	trillianInitialize(det);
						
	det.hwnd = CreateDialog(hInstance, templateStr, parent, dlg);
	
	plugin_send(MYGUID, "dialogAdd", &det);   

	return det.hwnd;
}
		
/******************************************************************************
 m y D e s t r o y W i n d o w ()
******************************************************************************/

void myDestroyWindow(HWND hWnd)
{
	struct dialog_entry_t		det;
	
	trillianInitialize(det);
	det.hwnd = hWnd;

	plugin_send(MYGUID, "dialogRemove", &det);

	DestroyWindow(hWnd);
}
