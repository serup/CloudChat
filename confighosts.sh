    # Configure /etc/hosts file
    echo "" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "# Host config for Puppet Master and Agent Nodes" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.8    dops.puppet.master          puppet" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.16   hadoop1.scanva.com          hadoop1" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.17   hadoop2.scanva.com          hadoop2" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.18   hadoop.scanva.com           hadoop" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.19   javaservices.scanva.com     javaservices" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.21   backend.scanva.com          backend" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.22   cloudchatmanager.com        cloudchatmanager" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.23   cloudchatclient.com         cloudchatclient" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.31.20   jenkins.scanva.com          jenkins" | sudo tee --append /etc/hosts 2> /dev/null

