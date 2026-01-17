<!-- TOC -->
## Visual Studio Code Setup
  - [Download VSCode](https://code.visualstudio.com/Download) and open the application
  - Go to extensions tab (circled in yellow)
  - Download extensions necessary to run vexcode (circled in red):
    - VEX Robotics
    - VEX Robotics Feedback
  - Vex tab should pop up after (circled in green) that shows hardware information and some documentation links as well as the ability to create and import new vex projects
<img width="1192" height="794" alt="Extensions Preview" src="https://github.com/user-attachments/assets/8ac2c8a1-96a6-4da5-a636-e7f01e354411" />

## Importing this project for use
 - On the home screen of Visual Studio Code, there is a button to clone a git repositry, click it. 
    It'll prompt you to either clone from github or manually type a url
    cloning from github usually requires you to sign into a github account, you can just paste this repo's url, `https://github.com/linmjie/Robot_2026.git`
 - This repo is comprised of multiple projects, each one getting its own folder, the main robot one is at [Axel_Controller](https://github.com/linmjie/Robot_2026/tree/main/Axel_Controller)
 - When you want to open a project, it is imperative you open its containing folder directly in VSCode,
  i.e. if you want to access the Axel_Controller project, you click "Open" or "Open Folder" inside of VSCode's start screen and then find the Axel_Controller folder
 - If you open the parent folder Robot_2026 in VSCode and navigate to the project folder, it will not work and you will be unable to access the vexcode functionality
 - You'll know if you've opened a vexcode project correctly if you see the special vexcode terminal popup (which may take up to a minute sometimes, so wait a little)

https://github.com/user-attachments/assets/25153fc6-72cc-44ae-adba-7b405307abc7

## Running the project
  - The [base documentation](https://kb.vex.com/hc/en-us/articles/21412349719956-Downloading-and-Running-a-VEX-Project-in-VS-Code)
    is pretty good, the toolbar it talks about is at the bottom of the window (you can see it in the screen recording once the vex project loads)

## Miscellaneous useful information
  - Click the little arrow next to any `#pragma region ... ` to collapse or expand them
  - Bring up command palette with <kbd>Cmd + Shift + P</kbd> on Mac or <kbd>Ctrl + Shift + P</kbd> on Windows and Linux
    - Command palette offers many useful commands, notably:
      - `VEX | Project: VEX Project Settings` to edit the project settings
      - `VEX | Command Help: VEX Command Help` to bring up a list of various vexcode functions
      - `Shell Command: Install 'code' command in PATH` to use 'code' in the command line to open vscode
  - Please don't use `Brain.Screen.print(...)` - it's pretty terrible - use [my logging library](https://github.com/linmjie/VexcodeLogger) instead (already included in main robot project)
  - Both `Brain.Screen.print(...)` and my `Logger.print(...)` or `Logger.println(...)` use [printf](https://www.it.uc3m.es/pbasanta/asng/course_notes/input_output_printf_en.html) formatting
    - Vex (and I) have provided two exceptions to the format string that allows you to print numbers slightly easier
      - `...print(4)` automatically resolves into `...print("%d", 4)`
      - `...print(3.451)` automatically resolves into `...print("%.2f, 3.451)` which always rounds to two decimal places
