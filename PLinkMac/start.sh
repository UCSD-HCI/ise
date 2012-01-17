#!/bin/sh

cd "$1"
pwd
#clear


if [ ! -d "RefreshMyDesk" ]; then
	echo "Unzipping plink archive..."
	unzip plink_Java.zip
  echo "Changing r/w permissions.."
  chmod 777 ./RefreshMyDesk
  echo "Changed r/w permissions."
fi

cd "RefreshMyDesk"
echo "Starting plink..."
java -Xms512m -Xmx512m -cp ./bin:./lib/* edu.ucsd.hci.refresh.RefreshMyDeskHsqlDB
