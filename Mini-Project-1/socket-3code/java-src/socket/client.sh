#!/bin/bash
#
# run the client. See server.sh for comments.

#JAVA_HOME=/where/am/i
#${JAVA_HOME}/java -cp .:./classes:lib/*.jar gash.app.BasicClientApp 

java --enable-preview -cp .:./classes:lib/*.jar gash.app.ClientApp 

