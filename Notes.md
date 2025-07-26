# sub_159F460 - creates the game window

## Thoughts/How I think it works

This is just simple win32 window creation iirc

# sub_E11B30, sub_D51370, and sub_D53CC0 - handles the config.xml 

## Thoughts/How I think it works

sub_D53CC0 has a variable called v53, I have ZERO idea what it is/how it works. It also has a variable called v54 which calls the sub_D51370 function. I'm ASSUMING this is a function that finds/grabs files for the engine, not completely sure though. After it gets the file the v55 variable calls the sub_E11B30 function with v53 and v54 as the arguments. The sub_E11B30 function is what actually parses the config.xml file and calls other functions based on the contents of the xml. sub_11FFE90 is a function that grabs the contents from the config.xml, for instance: 
```cpp
sub_11FFE90("Screen/Width", 800, 0);
```
Actually scrap what I said before about sub_11FFE90 grabbing the contents. I think it's actually supposed to be setting the default config for the stuff BUT Rayman Legends opens up in fullscreen 1980x1080 by default. But modifying the config.xml to change the width and height does nothing due to that being saved in ```HKEY_CURRENT_USER\Software\Ubisoft\Rayman Legends\Settings\``` for some reason.

# sub_1F6540 - Handles SteamAPI Initialization

# sub_1F6B90 - Handles UPlay Initialization

# sub_1F64C0 - Handles Mutex 

## Thoughts/How it works

From my understanding, in this case it's so multiple instances of the game cannot be opened.

# sub_1F3CC0 - Initializer?...

## Thoughts/How it works

Okay so this HAS a lot of things, it handles the launch arguments, calls sub_1F1D50 which calls sub_A3CDD0 which calls sub_A3CC20, handles stuff related to the Network Services, handles the Bundle_[DEVICE].ipk (I think)


### Launch argument example

```bash
"Rayman Legends" player_nodamage=1 # You'd take no damage!
```

# sub_A3CC20 - Seems to be a platform/device handler - Gets called by sub_A3CDD0

## Thoughts/How it works

I don't really have much to say but its funny how they left the Wii stuff in Legends lol

But it seems like it just registers the device there and MAYBE sets a specific configuration for said platform?... No idea tbh

# sub_1F8020 - A resource or thread cleanup(?)

# sub_1FABA0 - Reads the contents of the .ipk files

## Thoughts/How it works

I THINK it works by like uh loading stuff via a certain key, like for instance, we have this:

```cpp
sub_1FABA0("bundle", 6);
```

"bundle" could just be telling the engine to look at a file that starts with that and the 6 argument/variable could be somethin like....uh something that gets the device ur on?... No idea honestly.

# sub_1F6A10 - Graphics implementations based on different platforms

## Thoughts/How it works (this time by RayCarrot!)

I know that in the original UbiArt projects they have a setup they called "adapters" where they would have different implementations based on platform. This is done with the rendering and such as well, where there's a DX adapter and then separate ones for different platforms.
The PC versions even allow you to pass in a command when launching the exe for which to use, but only directx is supported
It's checked in ITF::ApplicationFramework::initSystemAndGraphics