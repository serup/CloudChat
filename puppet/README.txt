put following in ~/.bashrc file

export DOPS_PUPPET_PATH=/home/serup/GerritHub/CloudChat/puppet/trunk/environments/

in windows :
set DOPS_PUPPET_PATH=D:\GerritHub\CloudChat\puppet\trunk\environments\
echo %DOPS_PUPPET_PATH%

after install of puppet for windows (https://puppet.com/docs/puppet/5.3/install_windows.html):
use special command prompt for puppet ( should exists after above install )
puppet module install puppetlabs-apache --modulepath d:\GerritHub\CloudChat\puppet\trunk\environments\devtest\modules
 puppet module install saz/sudo --modulepath d:\GerritHub\CloudChat\puppet\trunk\environments\devtest\modules
puppet module install puppetlabs-git --modulepath d:\GerritHub\CloudChat\puppet\trunk\environments\devtest\modules
