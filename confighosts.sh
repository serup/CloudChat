    # Configure /etc/hosts file
    echo "" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "# Host config for Puppet Master and Agent Nodes" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.8    dops.puppet.master               puppet" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.15   jekyll.dops.scanva.com           hadoop1" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.0.101   hadoop1.dops.scanva.com          hadoop1" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.0.102   hadoop2.dops.scanva.com          hadoop2" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.0.103   hadoop3.dops.scanva.com          hadoop3" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.19   javaservices.dops.scanva.com     javaservices" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.21   backend.dops.scanva.com          backend" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.22   cloudchatmanager.com             cloudchatmanager" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.23   cloudchatclient.com              cloudchatclient" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.20   jenkins.dops.scanva.com          jenkins" | sudo tee --append /etc/hosts 2> /dev/null

