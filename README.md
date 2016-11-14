# Dedicated Server Plugin
This Plugin is designed to eliminate some shortcomings in UE4's Dedicated Server support.

## Features
* Console Input
 * Basics, enter to execute console command, backspace and delete for input modification
 * Cursor Movement via arrow-left/right, home(pos1), end
 * AutoCompletion via tab (WIP)
 * CommandHistory via arrow-up/down
* Autoupdate
 * See [Autoupdate wiki page](https://github.com/yas-online/UE4-DedicatedServer/wiki/Autoupdate)
* RCon (WIP)
 * TCP Socket
 * WebSocket

## Usage
Drop the plugin into your game's plugin folder.
To start your dedicated server with console input pass `-console` instead of `-log` to the command-line.

To enable rcon support add the following lines to your DefaultEngine.ini:
```ini
[/Script/Engine.GameEngine]
+NetDriverDefinitions=(DefName="RConNetDriver",DriverClassName="",DriverClassNameFallback="")

[RConNetDriver]
bEnableWebSocket=true
BanPenalty=0
MaxFailures=10
MinFailures=5
MinFaulureTime=30
```

you then need to add the following to the server command-line `-RConPassword=<yourpassword>`,
other options are: `-MULTIHOME_RCon=<IP to bind to (defaults to -MULTIHOME)>`
