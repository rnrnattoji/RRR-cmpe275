#!/bin/bash
#
# run the server


# Like the env PATH, java rutime uses a path (classpath) 
# seperated by a ':' (unix separator) that are either
# directories or files (e.g., .jar) used by the class loader
# to find compiled classes. In the command below the classpath
# is composed of:
#   .             -- all files found in this directory
#   ./classes     -- and files in this directory
#   lib/*.jar     -- all files in ./lib/ ending in .jar
#
# The compiled source (.class) gash.app.ServerApp.class is
# [project directory]/[classes]/gash/app/ServerApp.class
#                         ^
#                         |
#                 This is from my classpath (-cp) search
#
# The [project directory] is the directory where the code
# (project) resides. Thus when defining the classpath to
# envoke the Java runtime, we have to declare where to
# find the class files [.class]. The below command uses
# relative paths to construct the classpath, or what we
# refer to as the current working directory [.]. Since
# the command uses relative paths, the script (.sh) is
# dependent (sensitive to) on where it is envoked.

java -cp .:./classes:lib/*.jar gash.app.ServerApp 

