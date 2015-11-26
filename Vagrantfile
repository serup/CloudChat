# vi: set ft=ruby :

# puppet setup. Starts a puppet master with modules mapped in, and a client
# to tests 

# Based on:
# Builds Puppet Master and multiple Puppet Agent Nodes using JSON config file
# Author: Gary A. Stafford

# read vm and chef configurations from JSON files
nodes_config = (JSON.parse(File.read("nodes.json")))['nodes']
puppet_source = ENV['DOPS_PUPPET_PATH']
  
if puppet_source == nil
#   puts "NodeOS: " + nodeOS
   puts "Please set DOPS_PUPPET_PATH to your checkout of devtest, test and production"
   puppet_source = "$DIR""/puppet/trunk/environments/"
   #exit
end

VAGRANTFILE_API_VERSION = "2"
Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|

  nodes_config.each do |node|
   node_name   = node[0] # name of node
   node_values = node[1] # content of node

   if node_name == "dops.puppet.master"
	config.vm.define "dops.puppet.master", primary: true
   else
	config.vm.define node_name.to_s, autostart: false
   end	

   config.vm.define node_name do |config|   
      # Enable provisioning with Puppet stand alone.
      config.vm.provision :puppet do |puppet|
	puppet.manifests_path = "puppet/manifests"
	puppet.manifest_file  = "site.pp"
	puppet.module_path = "puppet/trunk/environments/devtest/modules"
	#puppet.options = "--verbose --debug"
        puppet.hiera_config_path = "puppet/hiera/node_site_config.yaml"
        puppet.working_directory = "/tmp/vagrant-puppet-3/"
      end 

      # configures all forwarding ports in JSON array
      ports = node_values['ports']
      ports.each do |port|
        config.vm.network :forwarded_port,
          host:  port[':host'],
          guest: port[':guest'],
          id:    port[':id']
      end

      if node_name == "dops.puppet.master"
        config.vm.synced_folder puppet_source, '/etc/puppet/environments'
      end

      config.vm.synced_folder("puppet/hiera", "/tmp/vagrant-puppet-3/hiera")

      config.vm.provision :shell, :path => node_values['bootstrap']
      # inline: "apt-get update -y" # https://github.com/mitchellh/vagrant/pull/5860
      config.vm.box = node_values['nodeOS']
      #config.vm.provision :shell, inline: "hostnamectl set-hostname " + node_values[':hostname'] 
      config.vm.hostname = node_values[':hostname']
      config.vm.network :private_network, ip: node_values[':ip']

      config.vm.provider :virtualbox do |vb|
        vb.customize ["modifyvm", :id, "--memory", node_values[':memory']]
        vb.customize ["modifyvm", :id, "--name", node_name]
        vb.customize ["modifyvm", :id, "--vram", "16"]
        vb.customize ["modifyvm", :id, "--nictype1", "virtio"]
	    vb.customize ["modifyvm", :id, "--natdnshostresolver1", "on"]
	    vb.customize ["modifyvm", :id, "--natdnsproxy1", "on"]
      end
    end
  end
end
