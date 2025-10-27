# BR_ManagementTool

This tool is compatible with version 1.9, but has not been fully tested!

A tool for assisting 24/7 server OPs of Brick Rigs. Currently not restricted to Linux systems, contrary to original plan.

Currently, there are three planned stages of development: Alpha, Beta, and Release. The project is in alpha, meaning that many of the features will be implemented, but in a rough state. In beta, the functions will be cleaned up and grouped into a stable format, before being tested for release.

Working Features:
- End / Restart matches
- Reset all player positions
- Destroy vehicles
- Kill players
- Send Chat Messages as an Admin
- Kick Players
- Ban Players
- Retrieve Chat Messages

TODO:
- Start the server headlessly
- Get vehicle steamid
- Change map system

## Building
Run:
- `meson setup --cross-file x86_64-w64-mingw32.txt build`
- `meson compile -C build`

## Tips
If running BR in a Proton container, use:
- `WINEFSYNC=1 WINEPREFIX=~/.steam/steam/steamapps/compatdata/552100/pfx/ WINEARCH=win64 ~/.steam/steam/steamapps/common/Proton\ -\ Experimental/files/bin/wine64 brlmt.exe`
