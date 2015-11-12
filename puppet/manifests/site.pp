node default {
  # Test message
  notify { "Debug output on ${hostname} node.": }
  #include ntp, git
}

node /^node01.*/ {
  # Test message
  notify { "Debug output on ${fqdn}": }

  class { apache : } 

  # NB! Needed BEFORE docker class, otherwise it will fail during install
  exec { "apt-update":
    command => "/usr/bin/apt-get update"
  }
  Exec["apt-update"] -> Package <| |>

#  class { apt : 
#     notify  => Exec['apt_update']
#  }
  # By default, Puppet runs `apt-get update` on the first Puppet run after you include the `apt` class, and anytime `notify  => Exec['apt_update']` occurs; i.e., whenever config files get updated or other relevant changes occur. If you set `update['frequency']` to 'always', the update runs on every Puppet run. You can also set `update['frequency']` to 'daily' or 'weekly'

  include 'docker'
#  include sudo
#
#  # Add adm group to sudoers with NOPASSWD
#  sudo::conf { 'vagrant':
#    priority => 01,
#    content  => "vagrant ALL=(ALL) NOPASSWD: ALL",
#  }
#
##  class { dops : }
#
  docker::image { 'base':
   ensure => 'absent',
#   command => '/bin/sh -c "while true; do echo hello world; sleep 1; done"',
  }

  docker::image { 'ubuntu':
   ensure  => 'absent',
   tag     => 'precise'
  }

##  docker::run { 'helloworld':
##        image   => 'ubuntu',
## 	command => '/bin/sh -c "while true; do echo hello docker world; sleep 1; done"',
##  }

}
