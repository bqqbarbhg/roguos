@echo off

@REM HACK: Use the SSH executable from hard-coded git path
set PATH=%PATH%;C:\Program Files (x86)\Git\bin

vagrant ssh -c "bash /vagrant/vagrant_build.sh"
