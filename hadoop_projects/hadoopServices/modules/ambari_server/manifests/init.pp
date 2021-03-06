class ambari_server ($ownhostname) {
  Exec {
    path => ["/bin/", "/sbin/", "/usr/bin/", "/usr/sbin/"] }

  # Ambari Repo
  exec { 'get-ambari-server-repo':
    #command => "wget http://public-repo-1.hortonworks.com/ambari/centos6/1.x/GA/ambari.repo",
    command => "wget http://public-repo-1.hortonworks.com/ambari/centos6/1.x/updates/1.2.3.7/ambari.repo",
    cwd     => '/etc/yum.repos.d/',
    creates => '/etc/yum.repos.d/ambari.repo',
    user    => root
  }

  # Fix an error in that repo reference
  #  exec { ‘fix-ambari-server-repo’:
  #  command => “sed -i ‘s/centos6\\/1.x\\/updates\\w*$/centos6\\/1.x\\/updates\\/1.6.0/g’ ambari.repo”, cwd => ‘/etc/yum.repos.d/’,
  #  user => root,
  #  require => Exec[get-ambari-server-repo]
  #}

  # Ambari Server
  package { 'ambari-server':
    ensure  => present,
    require => Exec[get-ambari-server-repo]
  }

  exec { 'ambari-setup':
    command => "ambari-server setup -s",
    user    => root,
    require => Package[ambari-server]
  }

  service { 'ambari-server':
    ensure  => running,
    require => [Package[ambari-server], Exec[ambari-setup]],
    start   => Exec[ambari-server-start]
  }

  exec { 'ambari-server-start':
    command => "ambari-server start",
    require => Service[ambari-server],
    onlyif  => 'ambari-server status | grep "not running"'
  }
}
