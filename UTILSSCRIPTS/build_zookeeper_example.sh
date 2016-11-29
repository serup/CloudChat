#!/usr/bin/env bash
mkdir -p ~/workspace
cd ~/workspace; mvn archetype:generate -DgroupId=com.zooktutorial.app -DartifactId=zook-app-tutorial -DarchetypeArtifactId=maven-archetype-quickstart -DinteractiveMode=false
cd ~/workspace/zook-app-tutorial; mvn package; mvn clean
echo "NB! IMPORTANT - change in pom file the JUnit version to 4.12, then set project structure->project to 8-lambdas, then in modules move Junit dependency to top of list using arrows, then rebuild"
