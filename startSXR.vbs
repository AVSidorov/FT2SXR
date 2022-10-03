Set WshShell = CreateObject("WScript.Shell")
loderfile = CreateObject("Scripting.FileSystemObject").GetParentFolderName(WScript.ScriptFullName) + "\loader.bat"
WshShell.Run chr(34) & loderfile & Chr(34), 0
Set WshShell = Nothing