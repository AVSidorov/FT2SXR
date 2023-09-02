HDFView 3.3.0
------------------------------------------------------------------------------

This directory contains the binary (release) distribution of
HDFView 3.3.0 that was compiled on:
      Windows win-amd64

with:  Java JDK 19.0.2

It was built with the following options:
   -- SHARED HDF 4.2.16
   -- SHARED HDF5 1.14.0

===========================================================================
Note: By default HDFView runs on the included Java JRE 19.
===========================================================================

The contents of this directory are:

   COPYING                 - Copyright notice
   README.txt              - This file
   HDFView       - HDFView Directory

Running
===========================================================================
To install HDFView for Windows, copy HDFView directory to where you want
to install HDFView and:

1. cd into the new HDFView
2. Execute ./HDFView.exe
3. Or execute .\app\HDFView.bat
===========================================================================

The executable will be in the installation location,
    which by default is at current-dir\HDFView

The general directory layout for each of the supported platforms follows:
===========================================================================
Linux
===========================================================================
HDFView/
  bin/            // Application launchers
    HDFView
  lib/
    app/
      doc/        // HDFView documents
      extra/      // logging jar for simple logs
      mods/       // Application duplicates
      samples/    // HDFView sample files
      HDFView.cfg     // Configuration info, created by jpackage
      HDFView.jar     // JAR file, copied from the --input directory
    runtime/      // Java runtime image
===========================================================================
macOS
===========================================================================
HDFView.app/
  Contents/
    Info.plist
    MacOS/         // Application launchers
      HDFView
    Resources/           // Icons, etc.
    app/
      doc/        // HDFView documents
      extra/      // logging jar for simple logs
      mods/       // Application duplicates
      samples/    // HDFView sample files
      HDFView.cfg     // Configuration info, created by jpackage
      HDFView.jar     // JAR file, copied from the --input directory
    runtime/      // Java runtime image
===========================================================================
Windows
===========================================================================
HelloWorld/
  HDFView.exe       // Application launchers
  app/
    doc/        // HDFView documents
    extra/      // logging jar for simple logs
    mods/       // Application duplicates
    samples/    // HDFView sample files
    HDFView.cfg     // Configuration info, created by jpackage
    HDFView.jar     // JAR file, copied from the --input directory
  runtime/      // Java runtime image
===========================================================================

Documentation for this release can be found at the following URL:
   https://portal.hdfgroup.org/display/HDFVIEW/HDFView

See the HDF-JAVA home page for further details:
   https://www.hdfgroup.org/downloads/hdfview/

Bugs should be reported to help@hdfgroup.org.