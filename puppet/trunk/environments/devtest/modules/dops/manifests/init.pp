class dops (
  $packages              = $dops::params::packages,
  $automation_passphrase = undef,
  $vmware_passphrase     = undef,
  $dops_id = $dops::params::dops_id,
  $confluence_server     = $dops::params::confluence_server,
) inherits dops::params {
  anchor { 'dops::begin': } ->
  class  { 'dops::install': } ->
  class  { 'dops::service': } ->
  anchor { 'dops::end': }
}
