DSPTool Basketball - Signal processing and labeling tool.
It allows analysis and labeling of synced signals, multiple videos and labels with accuracy upto 1 ms.
We intended it for labeling, development and testing in the pre-production and production phases when high accuracy and consistency are essential.


DSPTool built and tested for Linux Ubuntu 16.04. If you have another OS please let us know at algos@winkam.com.


First launch:
1. install FFmpeg via terminal: sudo apt-get install ffmpeg
2. right click on executable file “DSPTool_#_##_##” -> Properties -> Permissions -> Allow executing file as a program
3. open terminal in the “DSP Tool Ubuntu” folder and type command: sh dsp_tool_exec.sh	
4. click F1 for Help


Updating to a newer version:
1. remove executable file “DSPTool_#_##_##” from folder “DSP Tool Ubuntu” and paste a newer executable file there
2. right click on executable file “DSPTool_#_##_##” -> Properties -> Permissions -> Allow executing file as a program
3. open terminal in the “DSP Tool Ubuntu” folder and type command: sh dsp_tool_exec.sh


Bug reports:
If you have an issue contact us at algos@winkam.com 

License:
Non-commercial license:
Winkam grants ShotTracker Inc (ShotTracker.com) for 2 years starting August 1, 2020 a non-exclusive, worldwide, royalty-free license to use
DSPTool software of Winkam by following means: 
internal R&D of signal processing and machine learning algorithm/filters/models,
labeling (annotation), 
testing and debugging of the models/algorithms; 
without right of sale or right of sublicensing.


WINKAM (C) all rights reserved. 
www.winkam.com
Contact: algos@winkam.com


Nota Bene DSPTool utilizes several libraries under their licensing terms:
- Qt https://www.qt.io/licensing/
- Qwt https://qwt.sourceforge.io/qwtlicense.html
- OpenCV https://opencv.org/license/
- FFmpeg http://ffmpeg.org/legal.html



Guides:
To avoid errors please use the ST dataset file naming and labeling format [https://docs.google.com/document/d/1AFGh6Zhreg8EZIS7DuyIg0z9pLmXREbISo2fUKQMd6w/edit?usp=sharing]
Here is the folder to video guides [https://drive.google.com/drive/folders/1lSTc8m51PIqb7DAy1o_f8SMPKZT0Tzo3]
Here is the folder with dataset example [https://drive.google.com/drive/folders/19afO44k9AGuo18ezK_cQv3MP0UyS3lp-]

General Shortcuts:
F1 - Help and shortcuts
Ctrl + X - Exit

Opening data:
Ctrl + O - Open file with the XYZ data (in the ST dataset format). Video and labels will be automatically added to quick access panels.
Ctrl + L - Adding extra labeling file to quick access 
Ctrl + M - Opening meta-file 
Ctrl + V - Adding extra video to quick access

Actions with plots:
F5 - Apply signals settings and refresh plots
Ctrl + scroll up (down) - Zoom in (out)
Ctrl + R  OR Shift + Left click - Ruler
Ctrl + T - Zoom of the selected area

Time navigation:
W - Skip to the next frame/sample forward. 
Q - Skip to the previous frame/sample backward.
S - Seek forward one second.
A - Seek backward one second.
Shift + W - Seek forward ten frames/samples. 
Shift + Q - Seek backward ten frames/samples. 
Shift + S - Seek forward ten seconds.
Shift + A - Seek backward ten seconds.

Labeling:
P - Copy current timestamp, ms

Settings:
If you need to work with the original signal turn off in the settings "Work with 25 Hz signals". It may affect the performance while working with a lot of sensors simultaneously.
You can switch light / dark themes. 

