class role_nexus_server {

  # puppetlabs-java
  # NOTE: Nexus requires
  class{ '::java': }

  class{ '::nexus':
    version    => '2.8.0',
    revision   => '05',
    nexus_root => '/srv', # All directories and files will be relative to this
  }

  Class['::java'] ->
  Class['::nexus']
}

class bamboo (
  $version = '4.4.0',
  $extension = 'tar.gz',
  $installdir = '/usr/local',
  $home = '/home/vagrant',
  $user = 'vagrant'){
  
  include wget

  $srcdir = '/usr/local/src'
  $dir = "${installdir}/bamboo-${version}"

  File {
    owner  => $user,
    group  => $user,
  }

  if !defined(User[$user]) {
    user { $user:
      ensure     => present,
      home       => $home,
      managehome => false,
      system     => true,
    }
  }

  wget::fetch { 'bamboo':
    source      => "http://www.atlassian.com/software/bamboo/downloads/binary/atlassian-bamboo-${version}.${extension}",
    destination => "${srcdir}/atlassian-bamboo-${version}.tar.gz",
  } ->
  exec { 'bamboo':
    path => ['/usr/bin','/usr/sbin','/bin' ],
    command => "tar zxvf ${srcdir}/atlassian-bamboo-${version}.tar.gz && mv atlassian-bamboo-${version} bamboo-${version} && chown -R ${user} bamboo-${version}",
    creates => "${installdir}/bamboo-${version}",
    cwd     => $installdir,
    logoutput => "on_failure",
  } ->
  file { $home:
    ensure => directory,
  } ->
  file { "${home}/logs":
    ensure => directory,
  } ->
  file { "${dir}/webapp/WEB-INF/classes/bamboo-init.properties":
    content => "bamboo.home=${home}/data",
  } ->
  file { '/etc/init.d/bamboo':
    ensure => link,
    target => "${dir}/bamboo.sh",
  } ~>
  file { '/etc/default/bamboo':
    ensure  => present,
    content => "RUN_AS_USER=${user}
BAMBOO_PID=${home}/bamboo.pid
BAMBOO_LOG_FILE=${home}/logs/bamboo.log",
  } ~>
  service { 'bamboo':
    ensure     => running,
    enable     => false, # service bamboo does not support chkconfig
    hasrestart => true,
    hasstatus  => true,
  }

}

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

# NB! did not work - moved to bootstrap-node-hadoop.sh script which starts during boot of node hadoop
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


node /^gerrit.*/ {

  class { apache : } 

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
  #include grails_springboot

  class { 'ruby':
    gems_version => 'latest'
  }
   

#  include ::gerrit 
	
}


node /^jekyll.*/ {

  class { apache : } 

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

  class { 'ruby':
    gems_version => 'latest'
  }

  class {'jekyll':}
  
  class { 'nginx': }
  nginx::resource::vhost { 'nginx.scanva.com':
  www_root => '/var/www/nginx.scanva.com',
  }

}

node /^nginx.*/ {

  class { apache : } 

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

  class { 'ruby':
    gems_version => 'latest'
  }

  class { 'nginx': }
  nginx::resource::vhost { 'nginx.dops.scanva.com':
  www_root => '/var/www/nginx.dops.scanva.com',
  }

}


node /^javaservices.*/ {

   include grails_springboot
}


node /^jenkins.*/ {

  # howto manually apply this manifest file -- make sure you are sudo
  # puppet apply /vagrant/puppet/manifests/site.pp --modulepath /vagrant/puppet/trunk/environments/devtest/modules/

  include git
  include sudo
  
  # Add adm group to sudoers with NOPASSWD
  sudo::conf { 'vagrant':
    priority => 01,
    content  => "vagrant ALL=(ALL) NOPASSWD: ALL",
  }

  class { apache : } 
 
#      wget -q -O - https://jenkins-ci.org/debian/jenkins-ci.org.key | sudo apt-key add -
#      sudo sh -c 'echo deb http://pkg.jenkins-ci.org/debian binary/ > /etc/apt/sources.list.d/jenkins.list'
#      sudo apt-get update -yq
#      sudo apt-get install -yq jenkins
 
  exec { 'install_jenkins_package_keys':
    path => ['/usr/bin', '/bin', '/sbin'],
    command => 'wget -q -O - http://pkg.jenkins-ci.org/debian/jenkins-ci.org.key | sudo apt-key add - ',
    #command => '/usr/bin/wget -q -O - https://jenkins-ci.org/debian/jenkins-ci.org.key | sudo /usr/bin/apt-key add - ',
  }

  exec { 'append_to_jenkins_list' :
    path => ['/usr/bin', '/bin', '/sbin'],
    command => '/usr/bin/sudo /bin/sh -c "/bin/echo deb http://pkg.jenkins-ci.org/debian binary/ > /etc/apt/sources.list.d/jenkins.list"',
    require  => Exec['install_jenkins_package_keys'],
  }

  exec { "apt-update":
    command => "/usr/bin/sudo apt-get update",
    require  => Exec['append_to_jenkins_list'],
  }
  Exec["apt-update"] -> Package <| |>

  exec { 'install_jenkins' :
    command => '/usr/bin/apt-get install -yq jenkins',
    require  => Exec['apt-update'],
  }

  service { 'jenkins':
    ensure => running,
    require  => Exec['install_jenkins'],
  }

#  exec { 'fetch_jenkins_cli' :
#    path => ['/usr/bin', '/bin', '/sbin'],
#    command => 'wget -q -O - http://jenkins.dops.scanva.com:8080/jnlpJars/jenkins-cli.jar -',
#    require  => Exec['install_jenkins'],
#  }

#  exec { 'install_jenkins_plugin' : 
#    path => ['/usr/bin', '/bin', '/sbin'],
#    command => '/usr/bin/java -jar jenkins-cli.jar -s http://jenkins.dops.scanva.com:8080/ install-plugin cucumber-testresult-plugin',
#    require  => Exec['fetch_jenkins_cli'],
#  }
#  
#  exec { 'restart_jenkins' :
#    command => '/usr/bin/java -jar jenkins-cli.jar -s http://jenkins.dops.scanva.com:8080/ restart',
#    require  => Exec['install_jenkins_plugin'],
#  }
 
  class { 'cucumber':
    version => 'latest',
    require  => Exec['install_jenkins'],
  }

}

node /^hadoop.*/ {

  class { apache : } 

  include hadoop_install

#    include ambari
#    class { 'ambari::server':
#	  ownhostname => 'hadoop.scanva.com'
#    }

}

node /^bamboo.*/ {

  include grails_springboot
  #class { apache : }
  # bamboo.dops.scanva.com:8085   - Atlassian Bamboo 
  class { bamboo : } 

}

node /^nexus.*/ {

  include grails_springboot
  class { role_nexus_server: }  
  # it will run here : 
  # http://nexus.dops.scanva.com:8081/nexus
  # username : admin, password : admin123
  # after change of password, then to change back to admin123 then change in file nexus/conf/security.xml to following:
  # $shiro1$SHA-512$1024$G+rxqm4Qw5/J54twR6BrSQ==$2ZUS4aBHbGGZkNzLugcQqhea7uPOXhoY4kugop4r4oSAYlJTyJ9RyZYLuFBmNzDr16Ii1Q+O6Mn1QpyBA1QphA==
  # this equals admin123
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
