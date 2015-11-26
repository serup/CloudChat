    # Configure /etc/hosts file
    echo "" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "# Host config for Puppet Master and Agent Nodes" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.32.8    puppet.dops.local           puppet" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.32.19   javaservices.scanva.com     javaservices" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.32.21   backend.scanva.com          backend" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.32.22   cloudchatmanager.com        cloudchatmanager" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.32.23   cloudchatclient.com         cloudchatclient" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.32.20   jenkins.scanva.com          jenkins" | sudo tee --append /etc/hosts 2> /dev/null

