T.able I.con E.ntropy
=====================
Description
-----------
A test task solution that lists all imported libraries and methods found in the executable's import table, determines which ones come from WinAPI, calculates how many of methods contain letter 'w', replaces the main icon of the target executable file and, finally, calculates entropy for both target executable and icon.

Usage
-----
**USAGE:** tie *[path_to_executable]*

  `-i` *[path_to_icon]*     .ico file to replace original icon with

  `-o` *[output_filename]*  output .exe file, will write to the same executable if not defined
  
  `-s`                    short report

**EXAMPLE:** tie *test.exe* -o *test_copy.exe* -i *icon1.ico*

This will load *test.exe*, process its header, then create a copy in the same folder with icon injected.

**NOTE:** this app will calculate entropy for any kind of file provided, but will inject icon only of proper format (classic .ico or PNG type)