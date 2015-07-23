class dops::install inherits dops {

  # User to run the script
  group { 'dops':
    name => 'dops',
    ensure => 'present',
    system => true,
    gid => $dops_id,
  }
  user { 'dops':
    name => 'dops',
    ensure => 'present',
    system => true,
    uid => $dops_id,
    gid => $dops_id,
    home => '/usr/sbin/nologin',
    require => Group ['dops']
  }

  # The dependencies and binaries
  package { $packages : }
  # gems are hardcoded (yes it is ugly, but we need to take care of a specific version of nokogiri, and parametizing that is not less ugly)
  package { ['rvc','rbvmomi']:
    provider => 'gem',
    require => [Package['zlib1g-dev']],
  }
  package { 'nokogiri-absent':
    name     => 'nokogiri',
    ensure   => absent,
    provider => 'gem',
    require  => Package[rbvmomi],
  }
  exec { 'nokogiri':
  command  => "/usr/bin/gem install nokogiri -v '1.5.5'",
  require  => Package[nokogiri-absent],
  unless   => "/usr/bin/test $(/usr/bin/gem list | /bin/grep 'nokogiri (1.5.5)' | /usr/bin/wc -l) -eq 1",
  }
  exec { "prawn":
    command  => "/usr/bin/gem install prawn -v '1.3.0'",
    require  => Exec[nokogiri],
    unless   => "/usr/bin/test $(/usr/bin/gem list | /bin/grep 'prawn (1.3.0)' | /usr/bin/wc -l) -eq 1",
  }

  file { 'dops_files':
    path => '/usr/local/bin/dops',
    source => 'puppet:///modules/dops/dops',
    recurse => true,
    require => User ['dops'],
  }

}
