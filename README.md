# ![Spotian logo](http://adolphsson.se/media/2995/spotian.png) Spotian

A small Trillian plug-in to show currently played Spotify song in Trillian's status field.

![Preview of spotian in action](http://adolphsson.se/media/3382/spotifyintegration_preview.png)

Trillian main window to the right and Spotify with the (optional) notification overlay to the left.

Note!
=====
This plug-in is NOT working with the latest Spotify client since the Song and Artist information got removed from the window title of the client. Another way to read out this information would be to follow along with Toastify and use ChromeDriver (see http://toastify.codeplex.com/SourceControl/changeset/89968).

Usage
=====

1. Put the dll-file in the plugins folder, usually located at C:\Program Files\Trillian\plugins

2. Start/Restart Trillian so that the plug-in can be discovered.

3. Under Trillian->Prefrences->Plugin, put a check mark next to spotify.dll (it may also be named Spotify Integration) to activate the plug-in.

4. Don't close the preferences window just yet. First make sure that there is a check mark next to Set my status to what I'm listening to in the status tab under Basics.

Now you're all set up! Please note that any personal status message will override the song information.


Kudos to Ingmar Runge, the guy behind Trilly Tunes 2 (http://irsoft.de/web/TrillyTunes2), for helping me understand how to change the status and to Toastify (http://toastify.codeplex.com/) for being open source. This plug-in would never have been possible without them. And a huge thanks to everyone who improve and share to make life easier for others.
