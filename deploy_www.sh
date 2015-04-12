rm -rf /var/www/DOPS
mkdir -p /var/www/DOPS/CloudChat/CloudChatManager
mkdir -p /var/www/DOPS/CloudChat/CloudChatClient
mkdir -p /var/www/DOPS/DataEncoderDecoder
mkdir -p /var/www/DOPS/socketserverAPI
#rm -rf /var/scanva/release/serup/
#cd /var/scanva/release/
#mkdir serup 
#cd serup 
#git clone -b serup ssh://serup@review.gerrithub.io:29418/serup/CloudChat
#cd CloudChat
#./run.sh
cp -r /var/lib/jenkins/workspace/GerritCloudChat serup branch/javascript_projects/CloudChat/CloudChatManager /var/www/DOPS/CloudChat
cp -r /var/lib/jenkins/workspace/GerritCloudChat serup branch/javascript_projects/CloudChat/CloudChatClient /var/www/DOPS/CloudChat
cp -r /var/lib/jenkins/workspace/GerritCloudChat serup branch/javascript_projects/DataEncoderDecoder /var/www/DOPS
cp -r /var/lib/jenkins/workspace/GerritCloudChat serup branch/javascript_projects/socketserverAPI /var/www/DOPS
cp -r /var/lib/jenkins/workspace/GerritCloudChat serup branch/codeblocks_projects/DataDictionary /var/www/DOPS
#cd codeblocks_projects
./stopScanvaserver
./startScanvaserver
