rm -rf /var/scanva/release/master/
cd /var/scanva/release/
mkdir master
cd master
git clone -b master https://github.com/serup/CloudChat.git
cp -r /var/scanva/release/master/CloudChat/javascript_projects/CloudChat/CloudChatManager /var/www/DOPS/CloudChat/CloudChatManager
cp -r /var/scanva/release/master/CloudChat/javascript_projects/CloudChat/CloudChatClient /var/www/DOPS/CloudChat/CloudChatClient
