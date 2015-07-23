node default {
# Test message
  notify { "Debug output on ${hostname} node.": }

  #include ntp, git
}

node /^node01.*/ {
  # Test message
  notify { "Debug output on ${fqdn}": }

  # Add adm group to sudoers with NOPASSWD
  sudo::conf { 'vagrant':
    priority => 01,
    content  => "vagrant ALL=(ALL) NOPASSWD: ALL",
  }
  
  #class { scom : }
  
  ##liste over "profiler" som kan benytes - fjern kommentar til den type du vil benytte

  # Datacollector server
  #include datacollector
  
  # HHS
  # include hhs
  
  # DNS setup. 
  # include dns
  
  # Mongo data server
  #include mongodataserver
  
  #Mirror server
  #include mirror
 
  class { dops : }
}
