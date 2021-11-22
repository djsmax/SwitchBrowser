// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

void launchBrowser() {
	// first check if app doesn't run as an applet,
	// because if it doesn't webWifiShow() invocation results in hbmenu lock up
	
	AppletType at = appletGetAppletType();
	if (at != AppletType_Application && at != AppletType_SystemApplication) {
		printf("Running as an applet, browser won't work that way (as of now at least)!\n");
		return;
	}
	
	
	Result rc=0;
	WebWifiConfig config;
	Uuid uuid = {};
    
    // Set the initial URL that the applet will navigate too.
    webWifiCreate(&config, NULL, "https://google.com/", uuid, 0);
	
	// enable features
	// although webkit has support for video playback,
	// update 3.0+ blocked it (possibly because of fear of exploits)
	// seemingly no way to turn it back on other than to downgrade
	webConfigSetPageCache( (WebCommonConfig*) &config, true);
	webConfigSetWebAudio( (WebCommonConfig*) &config, true);


    printf("Running webWifiShow...\n");
    rc = webWifiShow(&config, NULL);
    printf("webWifiShow(): 0x%x\n", rc);
	printf("Press Y to launch it again\n");
    printf("Press + to exit.\n");
}


// Main program entrypoint
int main(int argc, char* argv[])
{
    consoleInit(NULL);

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);


    printf("SwitchBrowser\n");
	launchBrowser();

    // Main loop
    while(appletMainLoop())
    {
        // Scan the gamepad. This should be done once for each frame
        padUpdate(&pad);

        // padGetButtonsDown returns the set of buttons that have been
        // newly pressed in this frame compared to the previous one
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus)
            break; // break in order to return to hbmenu
		
		if (kDown & HidNpadButton_Y)
			launchBrowser();

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }

    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}
