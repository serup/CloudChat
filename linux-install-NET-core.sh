#!/bin/sh
echo "************************************************"
echo " INSTALL .NET CORE "
echo "************************************************"
PLATFORM=$(uname -i)
if [ "i686" == "$PLATFORM" ]; then
  echo "SORRY NO 32bit version for now"
else
  sudo apt-get install libicu-dev
  sudo sh -c 'echo "deb [arch=amd64] https://apt-mo.trafficmanager.net/repos/dotnet/ trusty main" > /etc/apt/sources.list.d/dotnetdev.list' 
  sudo sh -c 'echo "deb http://security.ubuntu.com/ubuntu trusty-security main" >> /etc/apt/sources.list.d/dotnetdev.list' 
  sudo apt-get update
  sudo dpkg -i libicu52_52.1-3ubuntu0.4_amd64.deb 
  sudo apt-key adv --keyserver apt-mo.trafficmanager.net --recv-keys 417A0893
  sudo apt-get install dotnet-dev-1.0.0-preview1-002702
  echo "************************************************"
  echo " - lets test it: "
  if [ ! -d "hwapp" ]; then
    mkdir hwapp
  fi
  cd hwapp
  dotnet new
  dotnet restore
  dotnet run
  echo "- done"
fi
