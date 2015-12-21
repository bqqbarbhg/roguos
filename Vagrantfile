#
# https://vagrantup.com
#

VAGRANTFILE_API_VERSION = '2'

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.box = 'ubuntu/trusty32'

  config.vm.provision 'shell', path: './provisioning/provisioning.sh',
                               privileged: false
end

