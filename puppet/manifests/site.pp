node default {
# Test message
  notify { "Debug output on ${hostname} node.": }
  #include ntp, git
}

node /^node01.*/ {
  # Test message
  notify { "Debug output on ${fqdn}": }

  # Add adm group to sudoers with NOPASSWD
  #sudo::conf { 'vagrant':
  #  priority => 01,
  #  content  => "vagrant ALL=(ALL) NOPASSWD: ALL",
  #}

  # Configuring apache
#  include apache
#  include apache::mod::php
#
#  apache::vhost { $::fqdn:
#	  port => '80',
#	       docroot => '/var/www/test',
#	       require => File['/var/www/test'],
#  }
#
#  # Setting up the document root
#  file { ['/var/www', '/var/www/test'] : # [6]
#	  ensure => directory,
#  }
#
#  file { '/var/www/test/index.php' : # [7]
#	  content => '>?php echo \'>p<Hello world!>/p<\' ?<',
#  }

  class { apache : } 
#  class { dops : }
}
