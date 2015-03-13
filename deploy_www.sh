rm -rf /var/www/DOPS
mkdir -p /var/www/DOPS/CloudChat/CloudChatManager
mkdir -p /var/www/DOPS/CloudChat/CloudChatClient
mkdir -p /var/www/DOPS/DataEncoderDecoder
mkdir -p /var/www/DOPS/socketserverAPI
rm -rf /var/scanva/release/master/
cd /var/scanva/release/
mkdir master
cd master
git clone -b master https://github.com/serup/CloudChat.git
cp -r /var/scanva/release/master/CloudChat/javascript_projects/CloudChat/CloudChatManager /var/www/DOPS/CloudChat
cp -r /var/scanva/release/master/CloudChat/javascript_projects/CloudChat/CloudChatClient /var/www/DOPS/CloudChat
cp -r /var/scanva/release/master/CloudChat/javascript_projects/DataEncoderDecoder /var/www/DOPS
cp -r /var/scanva/release/master/CloudChat/javascript_projects/socketserverAPI /var/www/DOPS
cp -r /var/scanva/release/master/CloudChat/codeblocks_projects/DataDictionary /var/www/DOPS
