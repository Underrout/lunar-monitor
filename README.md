# Lunar Monitor

Allows for automatic exporting of resources (currently only level files) while using Lunar Magic to edit Super Mario World ROMs.

## Why is this a thing?

Projects using git in combination with the convenient [Lunar Helper](https://github.com/MaddyThorson/LunarHelper) build system will likely experience the issue that git will consider identical .mwl level files to "differ", clogging up commits, causing incorrect merge conflicts and bloating repos. This is partially due to Lunar Helper exporting even unchanged levels every time it performs a Save. 

Unfortunately, this can seemingly not be resolved by comparing newly exported .mwl files with pre-existing ones directly in Lunar Helper during its Save operation, since .mwls can seemingly still actually differ in insubstantial ways (small byte permutations) despite the actual levels not having been changed (I have a Lunar Helper fork [here](https://github.com/Underrout/LunarHelper/tree/mwl_comparison) which attempts to export only .mwls that are different from pre-existing ones, but for me personally there were still a lot of false positives). 

In order to get around this issue I decided to create Lunar Monitor, which is basically a monitoring service that starts as soon as Lunar Magic is opened. Lunar Monitor will continually monitor a user-specified ROM while Lunar Magic is open, if a write to the ROM is detected, the monitor will check what window is currently active. If it's the main Lunar Magic level editor it will instruct Lunar Magic to export the level that was (hopefully) just saved to a user-specified directory. 

Currently there are two limitations to this approach:

- Since importing (Ex)GFX counts as a ROM write and occurs in the main level editor window, the monitor will think the currently open level was just saved and export the level despite it not having actually been saved

- Lunar Magic has a "Save level to ROM as ..." dialog which can be used to effectively copy a level's layout to another level number, unfortunately the monitor cannot currently detect this situation correctly and will export the original level rather than the "new" copied level, to further worsen this situation Lunar Magic does not currently notify the monitor correctly about it now being in this new level and any subsequent saves of this copied level will also result in the original level being exported mistakenly until the user switches to the level "officially" via Alt+PgUp/Alt+PgDown or the "Open level number" dialog

Note that the first limitation should rarely be an issue, especially since Lunar Helper will actually import (Ex)GFX without triggering Lunar Monitor during Build operations. The second situation is more of an issue, unfortunately there is not much I think I can currently do to resolve it without substantial effort. If Lunar Magic ever fixes the LM_NOTIFY_ON_NEW_LEVEL custom toolbar button option or adds new notification options that make this whole process simpler and more reliable I will look into resolving these limitations.

## Why is this a *good* thing?

By exporting only levels that were (very likely) truly changed by the user, Lunar Monitor in combination with [this compatible fork of Lunar Helper](https://github.com/Underrout/LunarHelper/tree/lunar_monitor_compatible), which doesn't export .mwls at all during Save operations, ensures that only truly relevant changes to level data actually get included in commits. This way it will be easier to tell what was actually changed in a commit, merging changes to *different* levels should be completely seamless and repo size should grow more slowly too. 

## Ok, but it's still not optimal

True, there are a lot of other things I would love to be able to export with this project in order to streamline this sort of workflow in a git context, but unfortunately doing so would currently likely require:

- Deep inspection of ROM writes, may require parsers for ROM data or
- Some sort of image recognition to monitor Lunar Magic more closely or
- Monitoring of Lunar Magic windows and retrieval of their data or key logging

I'm not very comfortable with any of these for obvious reasons and will instead wait for future Lunar Magic updates to hopefully add additional facilities for "hook" types of systems.

## Building

Probably just open in Visual Studio (I used 2019) and Build in release mode, I'm not a C++ developer, don't ask me for details please.

If you put a `#define DEBUG` at the top of `lunar_monitor.cpp` you'll get an exe that'll pop up a console window while it's executing, giving you some debug info if you need it.

## Setup

NOTE: If you want to use this with Lunar Helper make sure to use [this branch](https://github.com/Underrout/LunarHelper/tree/lunar_monitor_compatible) which does not export .mwls and ensures there's no conflict between Lunar Helper and Lunar Monitor during Builds.

Place `lunar_monitor.exe`, `usertoolbar.txt` and `lunar_monitor_config.txt` in the same folder as the Lunar Magic executable you want to invoke Lunar Monitor when it opens. If you're using Lunar Helper you probably want to put them in the same folder specified by "lm_path" in its config file(s) for convenience.

Now open `lunar_monitor_config.txt` in a text editor of your choice. It should currently look like this (excluding the comments):

```
rom_path: "C:/Users/BobbyDook/Hack/HackRom.smc"
level_directory: "levels"
lunar_magic_path: "lunar_magic.exe"
mwl_file_format: "level #.mwl"
```

"rom_path" specifies the ROM that Lunar Monitor will monitor for writes. If you're using Lunar Helper you probably want this to be the same as its "dir" path plus the "output" rom name. 

"level_directory" specifies the directory that the .mwls will be exported to. The path is relative to the "rom_path", so in the above example .mwls would be saved to `C:/Users/BobbyDook/Hack/levels`. If you're using Lunar Helper you want this to be the same as its own "levels" path so that they're imported correctly on Build operations.

"lunar_magic_path" specifies the path to the Lunar Magic executable that will be used to export levels. It is again relative to the "rom_path", in the above example the Lunar Magic executable would be at `C:/Users/BobbyDook/Hack/lunar_magic.exe`. If you're using Lunar Helper you probably want this to be the same as its own "lm_path" (but it's not that important as long as the specified Lunar Magic executable supports exporting .mwls).

"mwl_file_format" lets you specify a template for .mwl names that will be exported. In the above example, if you were to save level 10A in Lunar Magic it would be exported as `level 10A.mwl`, since the `#` is replaced by the level number if it's found in the "mwl_file_format". The .mwl file names don't matter for Lunar Helper so it doesn't really matter what "mwl_file_format" is as long as it includes `#` somewhere to distinguish different level numbers.

You should be good to go now. Just open the specified Lunar Magic executable manually or via Lunar Helper's Edit operation, save a level and it should immediately be exported to the specified level directory!

## Notes on compatibility with other workflows

If you're not using Lunar Helper but still want to use Lunar Monitor for some reason know that you can place a ".suspend_lunar_monitor" marker file in the "rom_path" directory to temporarily suspend Lunar Monitor from exporting levels on ROM write until the marker file is removed. This could be used to allow external tools (GPS, asar, etc.) to write to the ROM without potentially triggering Lunar Monitor.

The customized Lunar Helper branch I mentioned earlier uses this marker file to suspend Lunar Monitor during Build operations for example.
