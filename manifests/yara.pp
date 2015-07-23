class prober::yara (
  $package = 'python-yara',
  $version = '2.0.0-2'
) {
    package { $package:
    ensure  => $version,
    name    => $package,
  }
} 