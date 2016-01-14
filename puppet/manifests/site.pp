class grails_springboot {
    include maven
    include apt
    class { "archive::prerequisites": } -> class { "idea::community": version => "15.0.1" }
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
   # commented out since it is already added via class idea
   #     "curl",   
        "git-core",
        "bash"]:
        ensure => present,
        require => Exec["apt-get update"],
        before => Apt::Ppa["ppa:webupd8team/java"],
    }

    package { 'hiera':
        ensure => '1.3.4-1',      
        require => Exec["apt-get update"],
    }

    package { ["oracle-java8-installer"]:
        ensure => present,
        require => Exec["apt-get update 2"],
    }

    exec {
        "accept_license":
        command => "echo debconf shared/accepted-oracle-license-v1-1 select true | sudo debconf-set-selections && echo debconf shared/accepted-oracle-license-v1-1 seen true | sudo debconf-set-selections",
        cwd => "/home/vagrant",
        user => "vagrant",
        path => "/usr/bin/:/bin/",
        before => Package["oracle-java8-installer"],
        logoutput => true,
    }

# Does NOT work for Ubuntu vivid, thus use "install-grails" exec instead
#    package { ["grails-ppa"]:
#        ensure => present,
#        require => Package["oracle-java8-installer"],
#    }

    exec { "add_java_home":
        command => '/bin/echo "export JAVA_HOME=/usr/lib/jvm/java-8-oracle" >> /home/vagrant/.bashrc',
        require => Package["oracle-java8-installer"],
    }

    exec { "install-grails":
        command => '/usr/bin/sudo apt-get install -yq unzip; touch setup.sh; chmod 777 setup.sh; sudo chown vagrant:vagrant setup.sh; curl -s get.sdkman.io >> setup.sh; echo "source ~/.sdkman/bin/sdkman-init.sh" >> setup.sh; echo "echo installing grails" >> setup.sh; echo "sdk install grails < /dev/null" >> setup.sh; echo "sdk install springboot < /dev/null" >> setup.sh; echo "ln -s ~/.sdkman/candidates/springboot/current/bin/spring /usr/bin/spring" >> setup.sh ; echo "sudo ln -s ~/.sdkman/candidates/grails/current/bin/grails /usr/bin/grails" >> setup.sh; echo "/usr/bin/sudo grails -version" >> setup.sh; sudo bash setup.sh > output.log',
        require => Exec["add_java_home"],
    }
    
#    exec { "cleanupgrails":
#        command => '/bin/rm setup.sh',
#	require => Exec["install-grails"],
#    }

}

class hadoop_install {

  include apt
  apt::ppa { "ppa:webupd8team/java": }

  exec { "prereq":
     command => "/usr/bin/apt-get install -yq ssh; /usr/bin/apt-get install -yq rsync",
  }


  # howto manually apply this manifest file -- make sure you are sudo
  # puppet apply /vagrant/puppet/manifests/site.pp --modulepath /vagrant/puppet/trunk/environments/devtest/modules/

  # message
  notify { "fetch and save hadoop on ${fqdn}": }

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

    package { 'hiera':
        ensure => '1.3.4-1',      
        require => Exec["apt-get update"],
    }

    package { ["oracle-java8-installer"]:
        ensure => present,
        require => Exec["apt-get update 2"],
    }

    exec {
        "accept_license":
        command => "echo debconf shared/accepted-oracle-license-v1-1 select true | sudo debconf-set-selections && echo debconf shared/accepted-oracle-license-v1-1 seen true | sudo debconf-set-selections",
        cwd => "/home/vagrant",
        user => "vagrant",
        path => "/usr/bin/:/bin/",
        before => Package["oracle-java8-installer"],
        logoutput => true,
    }

    exec { "add_java_home":
        command => '/bin/echo "export JAVA_HOME=/usr/lib/jvm/java-8-oracle" >> /home/vagrant/.bashrc',
        require => Package["oracle-java8-installer"],
    }


#  exec { "hadoop_common":
#     command => "/usr/bin/wget -nc http://www.eu.apache.org/dist/hadoop/common/KEYS;/usr/bin/wget -nc http://www.eu.apache.org/dist/hadoop/common/hadoop-2.7.1/hadoop-2.7.1.tar.gz.asc; /usr/bin/wget -nc http://www.eu.apache.org/dist/hadoop/common/hadoop-2.7.1/hadoop-2.7.1.tar.gz; gpg --import KEYS; gpg --verify hadoop-2.7.1.tar.gz.asc hadoop-2.7.1.tar.gz",
#     require => Exec["add_java_home"],
#  }
#
#  exec { "extract_hadoop":
#     command => "/bin/tar -xvf hadoop-2.7.1.tar.gz | nl | tail -1 | awk '{print \"lines extracted: \" $1}'; ./hadoop-2.7.1/bin/hadoop version|grep Hadoop| awk '{print $2}' ",
#     require => Exec["hadoop_common"],
#  }
#
#  exec { "standalone_hadoop":
#     command => "/bin/mkdir input; /bin/cp ./hadoop-2.7.1/etc/hadoop/*.xml input; ./hadoop-2.7.1/bin/hadoop jar ./hadoop-2.7.1/share/hadoop/mapreduce/hadoop-mapreduce-examples-2.7.1.jar grep input output 'dfs[a-z.]+'; cat output/* ",
#     require => Exec["extract_hadoop"],
#  }

#  exec { "setup_passphrase":
##     path => ['/usr/bin','/usr/sbin','/bin' ],
#     command => "/usr/bin/ssh-keygen -t dsa -P '' -f ~/.ssh/id_dsa; /bin/cat ~/.ssh/id_dsa.pub >> ~/.ssh/authorized_keys ",
#     require => Exec["standalone_hadoop"],
#  }
#
##  exec { "Execution_hadoop":
##     path => ['./hadoop-2.7.1/bin','./hadoop-2.7.1/sbin' ],
#     #command => "./hadoop-2.7.1/bin/hdfs namenode -format; ./hadoop-2.7.1/sbin/start-dfs.sh ",
#     command => "hadoop-2.7.1/bin/hdfs namenode -format ",
#     require => Exec["setup_passphrase"],
#     #require => Exec["standalone_hadoop"],
#  }


}

##########################3333
#
# NODES
#
##########################3333

node default {

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


}

node /^javaservices.*/ {

   include grails_springboot
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

node /^hadoop.*/ {

  include hadoop_install

#    include ambari
#    class { 'ambari::server':
#	  ownhostname => 'hadoop.scanva.com'
#    }

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
