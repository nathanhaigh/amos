#!/bin/bash
if [ -z "$1" ]
then
   echo "Usage: $1 <OS name: Windows7/WindowsXP/ubuntu/fedora/centos/MacOS>" 
   exit 1 
fi
if [ -z "$2" ]
then
   echo "Usage: $2 <password>"
   exit 1
fi
#####################################
# install diff packages for diff OSes
#####################################

if [[ $1 == Windows7 || $1 == WindowsXP ]]
then
   echo "installing CPAN..."
/usr/bin/expect <<EOD
spawn cpan
expect "Would you like me to configure as much as possible automatically?"

send "yes\r"
expect eof
EOD

   cpan DBI
   echo "installing ssh server..." 
   if [ $1 == Windows7 ]
   then
      setx CYGWIN "ntsec tty" /M
      netsh firewall set opmode disable
   fi
   cd c:
   cd cygwin/etc/
   ssh-host-config -y -c "ntsec tty" -w "Aa123"
   net start sshd
   #shutdown /s
   cd .. 
fi

if [ $1 == ubuntu ]
then
    echo "1234561" | sudo -S apt-get update
    echo "1234561" | sudo -S apt-get -y install git libstatistics-descriptive-perl libdbi-perl libzzip-dev libzip-dev zlib1g-dev libglib2.0-0 libglib2.0-bin libglib2.0-dev
    echo "1234561" | sudo -S apt-get -y install ssh expect git-svn wget ash coreutils gawk gcc automake mummer mummer-doc libboost-dev g++ libqt4-core libqt4-dev libqt4-gui
fi

if [[ $1 == fedora || $1 == centos ]]
then
    echo "1234561" | sudo -S yum update
    echo "1234561" | sudo -S yum -y install ssh expect git curl-devel wget automake boost boost-devel libXmu libXmu-devel libXi libXi-devel expat expat-devel gettext-devel openssl-devel zlib-devel g++ perl-Statistics-Descriptive.noarch perl-Statistics-Descriptive-2.6-2.el5.1.noarch.rpm tcsh openssh.i386 openssh-clients.i386 openssh-server.i386 openssh.i686 openssh-clients.i686 openssh-server.i686 perl-XML-Parser.i386 perl-XML-Parser.i686 perl-DBI.i386 perl-DBI.i686 zlib.i686 zlib-devel.i686 zlibrary-ui-qt.i686 zlib.i386 zlib-devel.i386 glib2.i386 glib2-devel.i386 glib2.i686 glib2-devel.i686 qt.i686 qt-devel.i686 qt4.i386 qt4-devel.i386 qt-dev-tools 
fi

if [ $1 == centos ]
then
   wget http://kernel.org/pub/software/scm/git/git-1.7.4.1.tar.gz
   tar xvfz git-1.7.4.1.tar.gz
   cd git-1.7.4.1
   make prefix=/usr/local
   echo "1234561" | sudo -S make prefix=/usr/local install
   cd ..
fi

##########################
# install common packages
##########################
echo "registering ssh public key..."
echo > ssh_reg.txt
/usr/bin/expect <<EOD
spawn scp ssh_reg.txt ssh@sauron.cs.umd.edu:VMlogs
expect "Are you sure you want to continue connecting (yes/no)?"
send "yes\r"
expect eof
EOD
rm -f ssh_reg.txt

echo "installing wget..."
if [[ $1 != Windows7 && $1 != WindowsXP && $1 != MacOS ]]
then
   if [ $1 == ubuntu ]
   then
       echo "1234561" | sudo -S apt-get install wget
   else
       echo "1234561" | sudo -S yum -y install wget
   fi
fi
echo "installing mummer..."
if [ $1 != ubuntu ]
then
   wget http://superb-sea2.dl.sourceforge.net/project/mummer/mummer/3.22/MUMmer3.22.tar.gz 
   tar zvxf MUMmer3.22.tar.gz
   cd MUMmer3.22
   make check
   if [[ $1 == Windows7 || $1 == WindowsXP ]]
   then 
       make install
   elif [[ $1 == MacOS ]]
   then
	echo $2 | sudo -S make install
   else 
       echo "1234561" | sudo -S make install
   fi
   if [[ $1 == Windows7 || $1 == WindowsXP ]]
   then 
        chmod 777 /usr/local/bin/
   elif [[ $1 == MacOS ]]
   then
	echo $2 | sudo -S chmod 777 /usr/local/bin/
   else 
       echo "1234561" | sudo -S chmod 777 /usr/local/bin/
   fi
  
   cp nucmer delta-filter.exe delta-filter show-coords show-coords.exe /usr/local/bin/
   chmod 755 /usr/local/bin/nucmer
   chmod 755 /usr/local/bin/delta-filter
   chmod 755 /usr/local/bin/delta-filter.exe
   chmod 755 /usr/local/bin/show-coords.exe
   chmod 755 /usr/local/bin/show-coords
   NUCMER=/usr/local/bin/nucmer
   export NUCMER
   cd ..
fi
if [[ $1 != ubuntu && $1 != fedora ]]
then
   echo "installing Statistics-Descriptive-Perl..." 
   wget http://cpan-du.viaverio.com/authors/id/C/CO/COLINK/Statistics-Descriptive-2.6.tar.gz
   tar zvxf Statistics-Descriptive-2.6.tar.gz
   cd Statistics-Descriptive-2.6
   perl Makefile.PL 
   make
   make test
   if [[ $1 == Windows7 || $1 == WindowsXP ]]
   then 
        make install
   elif [[ $1 == MacOS ]]
   then
	echo $2 | sudo -S make install
   else 
       echo "1234561" | sudo -S make install
   fi
   cd .. 
fi

echo "installing BLAT..."
   mkdir temp
   cd temp 
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/linux/blatSuite.34.zip
   unzip blatSuite.34.zip
   if [[ $1 == Windows7 || $1 == WindowsXP ]]
   then 
        chmod 777 /usr/local/bin/
   elif [[ $1 == MacOS ]]
   then
	echo $2 | sudo -S chmod 777 /usr/local/bin/
   else 
       echo "1234561" | sudo -S chmod 777 /usr/local/bin/
   fi
   mv 11.ooc blat faToNib faToTwoBit gfClient gfServer nibFrag pslPretty pslReps pslSort twoBitInfo twoBitToFa webBlat /usr/local/bin/
   chmod 755 /usr/local/bin/11.ooc 
   chmod 755 /usr/local/bin/blat 
   chmod 755 /usr/local/bin/faToNib
   chmod 755 /usr/local/bin/faToTwoBit 
   chmod 755 /usr/local/bin/gfClient 
   chmod 755 /usr/local/bin/gfServer 
   chmod 755 /usr/local/bin/nibFrag 
   chmod 755 /usr/local/bin/pslPretty 
   chmod 755 /usr/local/bin/pslReps 
   chmod 755 /usr/local/bin/pslSort 
   chmod 755 /usr/local/bin/twoBitInfo 
   chmod 755 /usr/local/bin/twoBitToFa 
   chmod 755 /usr/local/bin/webBlat
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/data/4.pooc
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/data/5.pooc
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/data/10.ooc
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/data/10empty.ooc
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/data/11.fat.ooc
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/data/11.ooc
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/data/11empty.ooc
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/data/12.ooc
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/data/12r.ooc 
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/data/hg17_11.ooc
   wget http://hgwdev.cse.ucsc.edu/~kent/exe/data/mouse11.ooc
   mv *.pooc /usr/local/bin/
   mv *.ooc /usr/local/bin/
   chmod 755 /usr/local/bin/4.pooc
   chmod 755 /usr/local/bin/5.pooc
   chmod 755 /usr/local/bin/10.ooc
   chmod 755 /usr/local/bin/10empty.ooc
   chmod 755 /usr/local/bin/11.fat.ooc
   chmod 755 /usr/local/bin/11.ooc
   chmod 755 /usr/local/bin/11empty.ooc
   chmod 755 /usr/local/bin/12.ooc
   chmod 755 /usr/local/bin/12r.ooc
   chmod 755 /usr/local/bin/hg17_11.ooc
   chmod 755 /usr/local/bin/mouse11.ooc
   cd ..
   rm -rf temp/
