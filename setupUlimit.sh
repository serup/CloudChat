#!/usr/bin/env bash
#somehow individual file does not work - why?
#user=$(whoami)
#echo $user
#sudo touch /etc/security/limits.d/$user.conf
#sudo chown $user /etc/security/limits.d/$user.conf
#sudo echo "$user soft nofile 2048570" > /etc/security/limits.d/$user.conf
#sudo echo "$user soft nproc  2048570" >> /etc/security/limits.d/$user.conf

# run as sudo bash ./setupUlimit.sh
#info : http://ithubinfo.blogspot.dk/2013/07/how-to-increase-ulimit-open-file-and.html"
echo "fs.file-max = 1048570" >> /etc/sysctl.conf
echo "# /etc/security/limits.conf
#
#Each line describes a limit for a user in the form:
#
#<domain>        <type>  <item>  <value>
#
#Where:
#<domain> can be:
#        - a user name
#        - a group name, with @group syntax
#        - the wildcard *, for default entry
#        - the wildcard %, can be also used with %group syntax,
#                 for maxlogin limit
#        - NOTE: group and wildcard limits are not applied to root.
#          To apply a limit to the root user, <domain> must be
#          the literal username root.
#
#<type> can have the two values:
#        - "soft" for enforcing the soft limits
#        - "hard" for enforcing hard limits
#
#<item> can be one of the following:
#        - core - limits the core file size (KB)
#        - data - max data size (KB)
#        - fsize - maximum filesize (KB)
#        - memlock - max locked-in-memory address space (KB)
#        - nofile - max number of open files
#        - rss - max resident set size (KB)
#        - stack - max stack size (KB)
#        - cpu - max CPU time (MIN)
#        - nproc - max number of processes
#        - as - address space limit (KB)
#        - maxlogins - max number of logins for this user
#        - maxsyslogins - max number of logins on the system
#        - priority - the priority to run user process with
#        - locks - max number of file locks the user can hold
#        - sigpending - max number of pending signals
#        - msgqueue - max memory used by POSIX message queues (bytes)
#        - nice - max nice priority allowed to raise to values: [-20, 19]
#        - rtprio - max realtime priority
#        - chroot - change root to directory (Debian-specific)
#
#<domain>      <type>  <item>         <value>
#

#*               soft    core            0
#root            hard    core            100000
#*               hard    rss             10000
#@student        hard    nproc           20
#@faculty        soft    nproc           20
#@faculty        hard    nproc           50
#ftp             hard    nproc           0
#ftp             -       chroot          /ftp
#@student        -       maxlogins       4
# End of file

*          soft     nproc          1048570
*          hard     nproc          1048570
*          soft     nofile         1048570
*          hard     nofile         1048570
*          soft     nproc          1048570
*          hard     nproc          1048570
*          soft     nofile         1048570
*          hard     nofile         1048570
*	   soft     stack          1048570
*	   hard     stack          1048570" > /etc/security/limits.conf
echo "*          soft     nproc          1048570
*          hard     nproc          1048570
*          soft     nofile         1048570
*          hard     nofile         1048570" > /etc/security/limits.d/90-nproc.conf
