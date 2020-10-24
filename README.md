# CloudChat 
via vagrant a master and one node is created in VirtualBox and via puppet master and puppet agent on master and node, then the puppetlabs modules are  installed on several nodes 

*********************************************************************
**get started with CloudChat project on new machine                **
*********************************************************************
**Clone the gerrit repository**
```javascript 
  git clone ssh://serup@review.gerrithub.io:29418/serup/CloudChat && scp -p -P 29418 serup@review.gerrithub.io:hooks/commit-msg CloudChat/.git/hooks/
  
  or like this
  
  git clone "https://serup@review.gerrithub.io/a/serup/CloudChat" && (cd "CloudChat" && mkdir -p .git/hooks && curl -Lo `git rev-parse --git-dir`/hooks/commit-msg https://serup@review.gerrithub.io/tools/hooks/commit-msg; chmod +x `git rev-parse --git-dir`/hooks/commit-msg)
  
  NB! This will probably NOT work if your firewall is running, so close it or add a pass for the port 
  
  cd CloudChat/
```
**Create your own branch and checkout**
```javascript 
  git branch <your branch name>
  git checkout <your branch name>
```
**Setup environment and start creating / modifying files**
```
  run the install script - it will setup environment variables, and install needed modules for the project, used later in vagrant up
  . ./install.sh <docker image project name fx. skeleton>
  use your favorite editor to write code
```
**Checkin to your own branch using this setup**
 first time you checkin your branch needs to be created on gerrithub, thus make following command
```javascript 
   git add <your files..>
   git commit -m "<your checkin info>"
   git push -u origin <your branch name>
```
 next time use normal push - this will create a new review on GerritHub and you must review and approve it before submit - you can find your reviews here:
```
  https://review.gerrithub.io/#/q/project:  < your branch name >  /CloudChat
```
 Normal push command :
```javascript 
   git push origin HEAD:refs/for/<your branch name>
```
 NB! Inorder to make checkin on this GerritHub project, then you need to create a RSA public key and send to administrator - he will then add it to users, making it possible for you to make reviews on your own branch

*********************************************************************
**Following can be done after install**
*********************************************************************
**Start vagrant**
```javascript 
  vagrant up
```
**Start javaservices**
```javascript 
  vagrant up javaservices 
```
**log into javaservices**
```javascript 
  vagrant -XY ssh javaservices 
```
**if needed then run puppet agent**
```javascript 
  sudo -s
  puppet agent -t
```

**Inorder to replicate to GitHub manually, then setup upstream on your GitHub clone as follows**
```javascript 
  git remote add upstream https://serup@review.gerrithub.io/a/serup/CloudChat
  git pull upstream <your branch name>
  git push
```

*****************************
[![Join the chat at https://gitter.im/serup/CloudChat](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/serup/CloudChat?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
javascript chat via. SCANVA server using CloudChatManager and CloudChatClient

