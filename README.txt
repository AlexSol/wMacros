wMarcos 0.1.1 a
===========

Release date: 

Core
----
+DelSelf 

Library
-------
-	Process:
		+ Run
		+ RunWait
		+ Open
		+ ProcessClose
-	File:
		+ FileCopy
		+ FileDel
-	Dir:
		+ DirCopy
		+ DirDel
		+ DirMove
		+ DirRename
		+? DirCreate
==============================

-RunWait:
	cmd_1: cmd
	cmd_2: 
	os_ver: 6.1
	os_type: all
	win64:  disable
-Open:
	cmd_1: cmd
	cmd_2: ping
	os_type: 0x86
	win64: disable
-Open:
	cmd_1: cmd.exe
	cmd_2: "/?"
	os_ver: 6.1
	os_type: AlL
	win64:  disable