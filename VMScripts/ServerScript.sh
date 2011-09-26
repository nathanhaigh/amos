#!/bin/bash

###################################
# Server Configuration
###################################
Path_to_VB_dir="C:\Users\bryanta\VirtualBox VMs"
Path_to_VBoxManage="C:\Program Files\Oracle\VirtualBox"
Path_to_VMs="E:\BRYANTA\VMs"
home_dir=/home/bryanta
logs_dir=C:/Program\ \Files\ \(x86\)/ICW/home/ssh/VMlogs/
emailClient_dir="C:\Users\bryanta\Downloads\sendEmail-v156-notls"
CC_Emails=dungtq1387@gmail.com,bryanta@cs.umd.edu,atif@cs.umd.edu,mpop@umiacs.umd.edu,dsommer@umiacs.umd.edu
#CC_Emails=bryanta@cs.umd.edu
#To_Email=dungtq1387@gmail.com
Path_to_Storage=bryanta@walnut.umiacs.umd.edu:/scratch1/bryanta/WalnutLast/
###################################
# Physical Machines Configuration
###################################
PhMs=(128.8.126.2) 
PhMs_name_on_log=("MAC OS")
Users=(amos)
HomeDir=(/Users/amos/)

###################################
# VM Configuration
###################################
VMs=(win7_updateFirst ubuntu10.10 winXP_updateFirst fedora15 win7_updateLast centos-5.5-i386-server winXP_updateLast win7_updateFirst_Qt4 ubuntu10.10_Qt4 winXP_updateFirst_Qt4 fedora15_Qt4 win7_updateLast_Qt4 centos-5.5-i386-server_Qt4 winXP_updateLast_Qt4)
VMs_name_on_log=("WIN 7 UPDATE FIRST" "UBUNTU 10.10" "WIN XP UPDATE FIRST" "FEDORA 15" "WIN 7 UPDATE LAST" "CENT OS 5.5" "WIN XP UPDATE LAST" "WIN 7 UPDATE FIRST WITH HAWKEYE" "UBUNTU 10.10 WITH HAWKEYE" "WIN XP UPDATE FIRST WITH HAWKEYE" "FEDORA 15 WITH HAWKEYE" "WIN 7 UPDATE LAST WITH HAWKEYE" "CENT OS 5.5 WITH HAWKEYE" "WIN XP UPDATE LAST WITH HAWKEYE")
VMs_OS=(Windows7 Ubuntu WindowsXP Fedora Windows7 RedHat WindowsXP Windows7 Ubuntu WindowsXP Fedora Windows7 RedHat WindowsXP)
VMs_ssh_port=(2222 2226 2224 2227 2223 2228 2225 2229 2230 2231 2232 2233 2234 2235)
VMs_controller=(PIIX4 PIIX4 PIIX3 PIIX4 PIIX4 PIIX4 PIIX3 PIIX4 PIIX4 PIIX3 PIIX4 PIIX4 PIIX4 PIIX3)
VMs_nictype=(82540EM 82540EM Am79C973 82540EM 82540EM 82540EM Am79C973 82540EM 82540EM Am79C973 82540EM 82540EM 82540EM Am79C973)
VMs_nic=(e1000 e1000 pcnet e1000 e1000 e1000 pcnet e1000 e1000 pcnet e1000 e1000 e1000 pcnet)
update_cmd=("/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ apt-get\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe /UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ apt-get\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe /UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow")  
upgrade_cmd=("echo Sending upgrade command" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ apt-get\ \-y\ \upgrade" "echo Sending upgrade command" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \upgrade" "echo Sending upgrade command" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \upgrade" "echo Sending upgrade command" "echo Sending upgrade command" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ apt-get\ \-y\ \upgrade" "echo Sending update command" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \upgrade" "echo Sending upgrade command" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \upgrade" "echo Sending upgrade command")  
shutdown_cmd=("shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \shutdown\ \-h\ now" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \shutdown\ \-h\ now" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \/sbin/shutdown\ \-h\ now" "shutdown /s" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \shutdown\ \-h\ now" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \shutdown\ \-h\ now" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \/sbin/shutdown\ \-h\ now" "shutdown /s") 
reboot_cmd=("shutdown /r" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ reboot" "shutdown /r" "reboot" "shutdown /r" "reboot" "shutdown /r" "shutdown /r" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ reboot" "shutdown /r" "reboot" "shutdown /r" "reboot" "shutdown /r")
#Git_cmd="git clone git://amos.git.sourceforge.net/gitroot/amos/amos"
Git_cmd="scp -r ssh@sauron.cs.umd.edu:VMlogs/amos ."
################################
# ssh and invoke VM functions
################################

ssh_command ()
{
/usr/bin/expect <<EOD
spawn ssh -l bryanta -p $1 localhost $2
expect {
-re "Are you sure you want to continue connecting" { send "yes\r" }
-re "bryanta@localhost's password:" { send "1234561\r" }
}
expect eof
catch wait result
exit [lindex \$result 3]
EOD
}

register_VM ()
{
cd "$Path_to_VBoxManage"
echo $1 loading...
./VBoxManage createvm --name $1 --register
./VBoxManage modifyvm $1 --ostype $2 --memory 2048 --vram 16 --nestedpaging off --rtcuseutc on --usb on --audio dsound --ioapic on --hwvirtex off --nic1 nat --nictype1 $3
./VBoxManage storagectl $1 --name "IDE Controller" --add ide --controller $4 --sataportcount 2 --bootable on
./VBoxManage storageattach $1 --storagectl "IDE Controller" --port 0 --device 0 --type hdd --medium "${Path_to_VMs}\WalnutLast\\$1.vdi"
./VBoxManage setextradata $1 "VBoxInternal/Devices/$6/0/LUN#0/Config/ssh/HostPort" $5
echo "VBoxInternal/Devices/$6/0/LUN#0/Config/ssh/HostPort" $5
./VBoxManage setextradata $1 "VBoxInternal/Devices/$6/0/LUN#0/Config/ssh/GuestPort" 22
./VBoxManage setextradata $1 "VBoxInternal/Devices/$6/0/LUN#0/Config/ssh/Protocol" TCP
}

unregister_VM ()
{
cd "$Path_to_VBoxManage"
echo "Deleting the old $1..."
./VBoxManage unregistervm $1
rm -rf "$Path_to_VB_dir\\$1"
rm -f "$Path_to_VMs\WalnutLast\\$1.vdi" 
}

invoke_VM ()
{
cd "$Path_to_VBoxManage"
./VBoxManage startvm $1
}

poweroff_VM()
{
cd "$Path_to_VBoxManage"
./VBoxManage controlvm $1 poweroff
}

run_VM()
{
	unregister_VM ${VMs[$1]}         
        echo "Copying the lated update VM from walnut..."
	cd "$Path_to_VMs"
	scp -r $Path_to_Storage${VMs[$1]}.vdi WalnutLast/
	register_VM ${VMs[$1]} ${VMs_OS[$1]} ${VMs_nictype[$1]} ${VMs_controller[$1]} ${VMs_ssh_port[$1]} ${VMs_nic[$1]}
        invoke_VM ${VMs[$1]}	
        echo "Waiting for starting VM..."
	sleep 180
        echo "Registering ssh public key..."
        ssh_command ${VMs_ssh_port[$1]} "echo Registering ssh key"
        sleep 180
       echo "Sending update command..."
       echo ${update_cmd[$1]}
       eval cmd=\${update_cmd[$1]}
       ssh_command ${VMs_ssh_port[$1]} "$cmd"
       sleep 900
       echo "Sending upgrade command..."
       echo ${upgrade_cmd[$1]}
       eval cmd1=\${upgrade_cmd[$1]}
       ssh_command ${VMs_ssh_port[$1]} "$cmd1"
       echo "Waiting for updating..."
       sleep 3600
       echo "Sending restart command..."
       echo ${reboot_cmd[$1]}
       ssh_command ${VMs_ssh_port[$1]} "${reboot_cmd[$1]}"
       sleep 900
       echo "Sending shutdown command..."
       echo ${shutdown_cmd[$1]}
       ssh_command ${VMs_ssh_port[$1]} "${shutdown_cmd[$1]}"
       echo "Wating for shutting down VM..."
       sleep 900
       poweroff_VM ${VMs[$1]} 
       cd $Path_to_VMs
       echo "Copying VM back to Walnut..."
       scp WalnutLast/${VMs[$1]}.vdi $Path_to_Storage        
       invoke_VM ${VMs[$1]} 
       sleep 180 
        ssh_command ${VMs_ssh_port[$1]} "rm -rf amos/" 
        ssh_command ${VMs_ssh_port[$1]} "$Git_cmd"
        if [ $? -ne 0 ]
           then  	
              echo "FAILED: git error" >> "$logs_dir"${VMs[$1]}_Failed.log	
              poweroff_VM ${VMs[$1]} 
           else 	
              echo "sent git"
	      echo "waitting for git to complete..." 
	      sleep 600
	      ssh_command ${VMs_ssh_port[$1]} "cp /home/bryanta/amos/VMScripts/${VMs[$1]}.sh /home/bryanta/"
	      echo "VM script was copied to home directory"
	      ssh_command ${VMs_ssh_port[$1]} "chmod +x ${VMs[$1]}.sh"
	      ssh_command ${VMs_ssh_port[$1]} "./${VMs[$1]}.sh ${VMs[$1]}"
	      echo "VM script was run"
       fi	
	
}
#################################################
# Checkout AMOS and prepare for copying to slaves
#################################################
cd $home_dir
cd amos
past_commit=$(git log --format=format:%H -1)
past_email=$(git log --format=format:%ae -1)
cd ..
rm -rf amos
git clone git://amos.git.sourceforge.net/gitroot/amos/amos
sleep 300
cp -r amos "$logs_dir"
sleep 300
cd amos
current_commit=$(git log --format=format:%H -1)
LastCommit_Email=$(git log --format=format:%ae $past_commit..$current_commit)
LastCommit_Email=$(echo $LastCommit_Email | sed 's/ /,/g')
if [ -z $LastCommit_Email ]
then
 LastCommit_Email=${past_email}
fi

##############################
# Invoke all Physical Machines
##############################
cd "$logs_dir"

rm -f *.log *.txt
for (( i = 0; i < ${#PhMs[$i]}; i++ ))
do
  ssh ${Users[$i]}@${PhMs[$i]} "rm -rf ${PhMs[$i]}*" 
  if [ $? -ne 0 ]
           then  	
              echo "FAILED: Could not ssh to this machine" >> "$logs_dir"${PhMs[$i]}_Failed.log	
           else 	
              ssh ${Users[$i]}@${PhMs[$i]} "rm -rf amos/" 
  	      ssh ${Users[$i]}@${PhMs[$i]} "rm -rf *.log *.sh" 
              ssh ${Users[$i]}@${PhMs[$i]} "$Git_cmd" 
              echo "sent git"
              echo "sleeping for several mins to complete git..."
              sleep 600
              ssh ${Users[$i]}@${PhMs[$i]} "cp amos/VMScripts/${PhMs[$i]}.sh ${HomeDir[$i]}"
              ssh ${Users[$i]}@${PhMs[$i]} "chmod +x ${PhMs[$i]}.sh"
              ssh ${Users[$i]}@${PhMs[$i]} "./${PhMs[$i]}.sh ${PhMs[$i]}" &
  fi	
  
done

##############################
# Invoke all VMs
##############################
mkdir "$Path_to_VMs\WalnutLast"
num_of_VMs=${#VMs[@]}
firstVM=0
while [ $firstVM -lt $num_of_VMs ]
do
        let "secondVM= firstVM + 1"
        let "thirdVM= firstVM + 2"
        run_VM $firstVM &	
        if [ $secondVM -lt $num_of_VMs ]
        then
            run_VM $secondVM &
        fi	
        if [ $thirdVM -lt $num_of_VMs ]
        then
            run_VM $thirdVM & 
        fi	    
        # sleep for 5hs	
        sleep 18600
        # build for a long time
        cd "$Path_to_VBoxManage"
        ./VBoxManage controlvm ${VMs[$firstVM]} poweroff
        if [ $? -eq 0 ]
        then
            echo "FAILED: This VM took longer than expected to finish" >> "$logs_dir"${VMs[$firstVM]}_Failed.log
        fi

        if [ $secondVM -lt $num_of_VMs ]
        then
            ./VBoxManage controlvm ${VMs[$secondVM]} poweroff
            if [ $? -eq 0 ]
              then
                  echo "FAILED: This VM took longer than expected to finish" >> "$logs_dir"${VMs[$secondVM]}_Failed.log
            fi
        fi
        if [ $thirdVM -lt $num_of_VMs ]
        then 
            ./VBoxManage controlvm ${VMs[$thirdVM]} poweroff
            if [ $? -eq 0 ]
              then
                  echo "FAILED: This VM took longer than expected to finish" >> "$logs_dir"${VMs[$thirdVM]}_Failed.log
            fi
        fi
        let "firstVM= firstVM + 3"
done

##################################
# merge logs and send email
##################################
echo "waiting for VM log files..."
num_of_PhMs=${#PhMs[@]}
let "num_of_machines= num_of_VMs + num_of_PhMs"
cd "$logs_dir"
stop=1
while [ $stop == '1' ] 
do
	count=0
	for i in $( ls "$logs_dir" | grep .log ); do
		count=$(( $count + 1 ))
	done
	if [ $count == $num_of_machines ]; then
		stop=$(( $stop +1 ))
	fi
done
sleep 300

echo "combining VMs log files..."
date > log.txt
echo                 >> log.txt
echo "According to the git log, you are the last people who made a commit to the git repository. Below are the build results of your changes from our AMOS daily build system." >> log.txt
echo                 >> log.txt
now=$(date +"%y%m%d")
echo "*** Complete logs stored on http://sauron.cs.umd.edu/$now" >> log.txt
for (( i = 0; i < ${#PhMs[$i]}; i++ ))
do
  echo ============================= >> log.txt
  echo ${PhMs_name_on_log[$i]} >> log.txt
  echo ============================= >> log.txt
  cat "$logs_dir"${PhMs[$i]}_Failed.log | tail -1 >> log.txt
  cat "$logs_dir"${PhMs[$i]}.log | tail -1 >> log.txt
  echo                               >> log.txt
done
for (( i = 0; i < ${#VMs[$i]}; i++ ))
do
  echo ============================= >> log.txt
  echo ${VMs_name_on_log[$i]} >> log.txt
  echo ============================= >> log.txt
  cat "$logs_dir"${VMs[$i]}_Failed.log | tail -1 >> log.txt
  cat "$logs_dir"${VMs[$i]}.log | tail -1 >> log.txt
  echo                               >> log.txt
done

################################
# Sending email to group
################################
echo "log.txt created"
cp log.txt "$emailClient_dir"
cd "$emailClient_dir"

count1=0
for i in $( ls "$logs_dir" | grep Failed )
do
	count1=$(( $count1 + 1 ))
done

echo "Sending email to group..."
if [ $count1 != '0' ]
then
        ./sendEmail -f bryanta@sauron.cs.umd.edu -u [AMOS Daily Build] FAILED -o message-file=log.txt -t $LastCommit_Email -cc $CC_Emails
fi
if [ $count1 == '0' ]
then
        ./sendEmail -f bryanta@sauron.cs.umd.edu -u [AMOS Daily Build] SUCCESS -o message-file=log.txt -t $LastCommit_Email -cc $CC_Emails
fi
echo "Email sent"
rm -f log.txt
cd "$logs_dir"
mkdir `date +%y%m%d`
mv *.log `date +%y%m%d`/ 
mv log.txt `date +%y%m%d`/
rm -rf "$logs_dir"amos
exit


