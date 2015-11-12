# CloudChat

[![Join the chat at https://gitter.im/serup/CloudChat](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/serup/CloudChat?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
javascript chat via. SCANVA server using CloudChatManager and CloudChatClient

Inorder to check in to this branch then do following:
git add <file>
git commit -m "boble : message"
git push origin HEAD:refs/for/serup

commit via gerrithub from IDEA intellij... -- not working from inside Intellij, that is it does not create a review - you must
checkin like above mentioned --  instead of using the IDE's checkin !!!!!

NB!
inorder to get puppet to work, then put following in .bashrc file:
export DOPS_PUPPET_PATH=/home/serup/GerritHub/CloudChat/puppet/trunk/environments/

-- jenkins should have gerrit build trigger plugin
