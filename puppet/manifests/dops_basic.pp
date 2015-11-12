class dops_basic () {

  ##
  #DOPS basic
  ##

  class { dops: }
  class { dops_apt: }
  class { dops_firewall: }
  class { dops_puppetagent:  }

}
