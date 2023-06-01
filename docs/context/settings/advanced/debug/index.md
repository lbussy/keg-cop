(debug)=

# Debug Configuration

Debug Configuration deals with both the verbosity of messages and if Telnet is available.

```{image} debug_conf.png
:align: center
:alt: Debug Calibration Screen
:scale: 30%
```

## Log Level

The drop-down shows seven verbosity levels:

0.  `LOG_LEVEL_SILENT`     No logging output is displayed
1.  `LOG_LEVEL_FATAL`      Shows only fatal events
2.  `LOG_LEVEL_ERROR`      Shows errors and fatal events  
3.  `LOG_LEVEL_WARNING`    Shows errors and warnings 
4.  `LOG_LEVEL_NOTICE`     Shows errors, warnings, and notices 
5.  `LOG_LEVEL_TRACE`      Shows errors, warnings, notices, and traces 
6.  `LOG_LEVEL_VERBOSE`    Shows all log output

Higher logging levels are helpful for debugging tasks.  There is no performance impact from the various levels.  Serial logging is enabled by default, and you may connect with a micro USB *data* cable to your computer.  The baud rate is 115200, and the connected controller is easily powered through this cable.

## Telnet

Telnet is a valuable tool when debugging or monitoring is required, and the system is in a place where connecting a serial cable is impractical.  You may connect to the controller via port 23; no authentication is required.  Because this can present a security risk in some environments, this is disabled by default.

Never open your controller, either the web UI or Telnet, to the Internet.
