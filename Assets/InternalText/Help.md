# Simple Process Tracker v2.0.2

A simple, lightweight app that tracks how long user-selected processes have cumulatively been running on the system.

Source code - https://github.com/kik4444/simple-process-tracker

Credits:

https://www.svgrepo.com - for SVG icons used in Simple Process Tracker

## Upper Toolbar

### Add button
Add a process to track from a list of found processes on this PC. Additionally you may select an icon for the process.

### Poll button
Manually check which processes are currently working. Processes are normally checked on a configurable interval of time. If a process stops or starts, this won't be reflected in Simple Process Tracker until the next scheduled check. This button can be used to force a check ahead of schedule.

### Options button
Specify in seconds how often Simple Process Tracker should check which processes are working. For example, if the schedule is 5 seconds, if a tracked process started 1 second ago, it will take 4 seconds until Simple Process Tracker detects it and starts counting it. Use with lower values for more accuracy and more frequent CPU usage spikes, or higher values for less accuracy and less CPU usage spikes.

### Export button
Export **all** processes in Simple Process Tracker to a json file.

### Import button
Import a JSON file containing one or more processes' saved data.

### Stretch button
Resize the processes table's columns equally to fit the available space.

### Help button
Show this help page, the Qt version information and Simple Process Tracker's license.

### Exit button
Shut down Simple Process Tracker without putting it in the background.

### Text field
Filter the process list by process name and notes with regular expressions.

## Categories table
In this table you can create, rename or remove custom categories that can be assigned to one or more processes. Every process can have zero or more categories assigned to it. Right-click in this table to reveal options for adding, renaming or removing categories.

### Selecting a category
Left-click on a category to show only processes that belong to it. Click it again to show all processes.

### Move Up / Down buttons
Move the selected category up or down by one position.

## Lower Toolbar
Four buttons to move processes around. Select one or more processes and press one button to move the selected processes up / down by one position, or to the top / bottom buttons to move the selection to the top or bottom at once.

## Processes table
This is the main table that contains all processes' data separated by columns. Right-click the columns to show or hide them.

### # column
The number of the process.

### Tracking column
An indicator whether Simple Process Tracker is actively tracking the process. Double-click to pause or resume tracking.

### Icon column
The user-defined icon for this process. Double-click to change it.

### Name column
The name of the process as it appears in this PC. Double-click to remove the process and data associated with it. This column is searchable through the top-right text field.

### Duration column
The cumulative amount of time the process has been working in days, hours, minutes and seconds. Double-click to set a custom duration in dd:hh:mm:ss format. For example 2 days, 1 hour, 32 minutes and 42 seconds is 02:01:32:42.

### Notes column
User notes for the process. Write whatever you wish in this column. This column is searchable through the top-right text field.

### Last seen column
The time the process was last found to be working.

### Date added column
The time the process was added to Simple Process Tracker.

## Right-click actions
Select one or multiple processes and right-click them to reveal extra actions for category management and other.

## System tray
Simple Process Tracker will create an icon on startup in the system tray (bottom right of the screen) to remind you that it is running in the background. Right-click this icon for additional actions to open or exit Simple Process Tracker, pause or resume all currently **shown** processes (does not affect processes that are filtered out by category or text search).
