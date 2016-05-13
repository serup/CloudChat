# VM-Configuration of the ambari server. It functions as the name node and resource manager.

# Turn off interfering services
include interfering_services

# Install and enable ntp
include ntp

# Ensure that servers can find themselves even in absence of dns
class { 'etchosts':
  ownhostname => 'one.cluster'
}

# Install and enable ambari server -- somehow this is NOT working, hence the need for bootstrap-one.sh 
# TODO: find a way to solve this so bootstrap script is not used
#class { 'ambari_server':
#  ownhostname => 'one.cluster'
#}

# Install and enable ambari agent
#class { 'ambari_agent':
#  ownhostname    => 'one.cluster',
#  serverhostname => 'one.cluster'
#}

# Establish ordering
#Class['interfering_services'] -> Class['ntp'] -> Class['etchosts'] -> Class['ambari_server'] -> Class['ambari_agent']
Class['interfering_services'] -> Class['ntp'] -> Class['etchosts']

