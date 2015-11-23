node default {
  # Test message
  notify { "Debug output on ${hostname} node.": }
  #include ntp, git
}

node /^node01.*/ {

  # howto manually apply this manifest file -- make sure you are sudo
  # puppet apply /vagrant/puppet/manifests/site.pp --modulepath /vagrant/puppet/trunk/environments/devtest/modules/

  # Test message
  notify { "Debug output on ${fqdn}": }

  class { apache : } 

  # NB! Needed BEFORE docker class, otherwise it will fail during install
  exec { "apt-update":
    command => "/usr/bin/apt-get update"
  }
  Exec["apt-update"] -> Package <| |>

  include sudo
  include 'docker'

  # Add adm group to sudoers with NOPASSWD
  sudo::conf { 'vagrant':
    priority => 01,
    content  => "vagrant ALL=(ALL) NOPASSWD: ALL",
  }

  docker::image { 'ubuntu':
   image_tag => 'trusty',
  }

  docker::image { 'skeleton':
   docker_tar => '/vagrant/puppet/trunk/environments/devtest/modules/docker_images_download/docker-image-skeleton.tar' 
  }

  exec { "docker_run":
     command => "/usr/bin/sudo docker run -d -t docker-image-skeleton",
     require => docker::image[skeleton]
  }

}

class grails {
    include apt
    apt::ppa { "ppa:webupd8team/java": }
    apt::ppa { "ppa:groovy-dev/grails": }

    exec { 'apt-get update':
        command => '/usr/bin/apt-get update',
        before => Apt::Ppa["ppa:webupd8team/java"],
    }

    exec { 'apt-get update 2':
        command => '/usr/bin/apt-get update',
        require => [ Apt::Ppa["ppa:webupd8team/java"], Apt::Ppa["ppa:groovy-dev/grails"], Package["git-core"] ],
    }

    package { ["vim",
        "curl",
        "git-core",
        "bash"]:
        ensure => present,
        require => Exec["apt-get update"],
        before => Apt::Ppa["ppa:webupd8team/java"],
    }

    package { ["oracle-java7-installer"]:
        ensure => present,
        require => Exec["apt-get update 2"],
    }

    exec {
        "accept_license":
        command => "echo debconf shared/accepted-oracle-license-v1-1 select true | sudo debconf-set-selections && echo debconf shared/accepted-oracle-license-v1-1 seen true | sudo debconf-set-selections",
        cwd => "/home/vagrant",
        user => "vagrant",
        path => "/usr/bin/:/bin/",
        before => Package["oracle-java7-installer"],
        logoutput => true,
    }

    package { ["grails-ppa"]:
        ensure => present,
        require => Package["oracle-java7-installer"],
    }

    exec { "add_java_home":
        command => '/bin/echo "export JAVA_HOME=/usr/lib/jvm/java-7-oracle" >> /home/vagrant/.bashrc',
    }
}

node /^javaservices.*/ {

#  include grails
   include java
   include maven
   include git

}


node /^jenkins.*/ {

  # howto manually apply this manifest file -- make sure you are sudo
  # puppet apply /vagrant/puppet/manifests/site.pp --modulepath /vagrant/puppet/trunk/environments/devtest/modules/

  class { apache : } 

  # NB! Needed BEFORE docker class, otherwise it will fail during install
  exec { "apt-update":
    command => "/usr/bin/apt-get update"
  }
  Exec["apt-update"] -> Package <| |>

  include sudo
  # Add adm group to sudoers with NOPASSWD
  sudo::conf { 'vagrant':
    priority => 01,
    content  => "vagrant ALL=(ALL) NOPASSWD: ALL",
  }

  include git

}


node /^backend.*/ {

  class { apache : } 

  # NB! Needed BEFORE docker class, otherwise it will fail during install
  exec { "apt-update":
    command => "/usr/bin/apt-get update"
  }
  Exec["apt-update"] -> Package <| |>

  include sudo
  # Add adm group to sudoers with NOPASSWD
  sudo::conf { 'vagrant':
    priority => 01,
    content  => "vagrant ALL=(ALL) NOPASSWD: ALL",
  }

  include git

}


node /^cloudchatmanager.*/ {

  # howto manually apply this manifest file -- make sure you are sudo
  # puppet apply /vagrant/puppet/manifests/site.pp --modulepath /vagrant/puppet/trunk/environments/devtest/modules/

  class { apache : } 

  # NB! Needed BEFORE docker class, otherwise it will fail during install
  exec { "apt-update":
    command => "/usr/bin/apt-get update"
  }
  Exec["apt-update"] -> Package <| |>

  include sudo
  # Add adm group to sudoers with NOPASSWD
  sudo::conf { 'vagrant':
    priority => 01,
    content  => "vagrant ALL=(ALL) NOPASSWD: ALL",
  }

  include git

}

node /^cloudchatclient.*/ {

  # howto manually apply this manifest file -- make sure you are sudo
  # puppet apply /vagrant/puppet/manifests/site.pp --modulepath /vagrant/puppet/trunk/environments/devtest/modules/

  class { apache : } 

  # NB! Needed BEFORE docker class, otherwise it will fail during install
  exec { "apt-update":
    command => "/usr/bin/apt-get update"
  }
  Exec["apt-update"] -> Package <| |>

  include sudo
  # Add adm group to sudoers with NOPASSWD
  sudo::conf { 'vagrant':
    priority => 01,
    content  => "vagrant ALL=(ALL) NOPASSWD: ALL",
  }

  include git
 
}
