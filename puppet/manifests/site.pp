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

# Does NOT work for Ubuntu vivid, thus use "install-grails" exec instead
#    apt::ppa { "ppa:groovy-dev/grails": }

    exec { 'apt-get update':
        command => '/usr/bin/apt-get update',
        before => Apt::Ppa["ppa:webupd8team/java"],
    }

    exec { 'apt-get update 2':
        command => '/usr/bin/apt-get update',
        #require => [ Apt::Ppa["ppa:webupd8team/java"], Apt::Ppa["ppa:groovy-dev/grails"], Package["git-core"] ],
        require => [ Apt::Ppa["ppa:webupd8team/java"],  Package["git-core"] ],
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

# Does NOT work for Ubuntu vivid, thus use "install-grails" exec instead
#    package { ["grails-ppa"]:
#        ensure => present,
#        require => Package["oracle-java7-installer"],
#    }

    exec { "add_java_home":
        command => '/bin/echo "export JAVA_HOME=/usr/lib/jvm/java-7-oracle" >> /home/vagrant/.bashrc',
        require => Package["oracle-java7-installer"],
    }

    exec { "install-grails":
        command => '/usr/bin/sudo apt-get install -yq unzip; touch setup.sh; chmod 777 setup.sh; sudo chown vagrant:vagrant setup.sh; curl -s get.sdkman.io >> setup.sh; echo "source ~/.sdkman/bin/sdkman-init.sh" >> setup.sh; echo "echo installing grails" >> setup.sh; echo "sdk install grails < /dev/null" >> setup.sh; echo "sdk install springboot < /dev/null" >> setup.sh; echo "sudo ln -s ~/.sdkman/canditates/springboot/current/bin/spring /usr/bin/spring" >> setup.sh ; echo "sudo ln -s ~/.sdkman/candidates/grails/current/bin/grails /usr/bin/grails" >> setup.sh; echo "/usr/bin/sudo grails -version" >> setup.sh; sudo bash setup.sh > output.log',
        require => Exec["add_java_home"],
    }
    
#    exec { "cleanupgrails":
#        command => '/bin/rm setup.sh',
#	require => Exec["install-grails"],
#    }

}

node /^javaservices.*/ {

   include grails
#   include springboot
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
