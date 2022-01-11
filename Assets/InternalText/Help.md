# Simple Process Tracker v2.0

A lightweight, simple Qt-based app that tracks how user-selected processes have been running on the system.
Source code - https://github.com/kik4444/simple-process-tracker

## How to use

### Adding processes to track
Click the "Add" button on the top left. A new window will appear. In this window you can find the process you wish to track. Additionally you can set an icon for it. When you have found a process, double-click it to add it, or click it once and then the "ok" button.

### Pause / resume time tracking
Double-click the "Tracking" cell for a given row to pause or resume tracking a process.

### Add custom icon to process
Double-click the "Icon" cell to change the icon of a process.

### Removing processes
Double-click the "Name" cell to confirm removing a tracked process. Removing is irreversible as it will also remove the saved data for that process.

### Custom notes
Double-click the "Notes" cell to start writing in it. You can write anything in this cell.

### Set custom running duration
Double-click the "Duration" cell and an input dialog will appear. Write your custom duration in dd::hh:mm:ss format. E.g. 2 days, 1 hour, 32 minutes and 42 seconds is 02:01:32:42. Click "Ok" and the processes' duration will start counting up from that.

### Hide / show columns
Right-click on the horizontal header (where it says "Tracking", "Icon", etc.) and a context menu will popup where you can hide or show columns.

### Hide / show rows
Right-click on the vertical header on the left with the numbers and a context menu will popup where you can hide or show rows. Alternatively you can right-click a process to show most available actions for it.

### Manually poll processes
Click the "Poll" button on the main window button to force check processes. This is useful if you've set the poll interval to be very infrequent in the options window and wish to initiate a poll immediately.

### Stretching columns to fit
Click the "Stretch" button to automatically resize all columns to fit the available window space.

### Options
Click the "Options" button on the main window to open the options window. Here you can set how often Simple Process Tracker should check whether a process is running. The default value is 5 seconds. Lower values = more accuracy + more CPU usage. Higher values = less accuracy + less CPU usage. I recommend 15 seconds.

### About
Click the "About" button to open this help section.

### Running in the background
This program can work in the background if you close the main window. It is visible in the system tray icon. To close the program completely, right click the system tray icon and click "Exit" or click the big "Exit" button on the main window.

Credits
https://www.svgrepo.com - for SVG icons