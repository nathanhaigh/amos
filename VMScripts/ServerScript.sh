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
Path_to_Storage=bryanta@walnut.umiacs.umd.edu:/scratch1/bryanta/WalnutLast/
###################################
# Physical Machines Configuration
###################################
PhMs=(128.8.126.2)
Users=(amos)

###################################
# VM Configuration
###################################
#VMs=(win7_updateFirst ubuntu10.10 winXP_updateFirst fedora15 win7_updateLast centos-5.5-i386-server winXP_updateLast win7_updateFirst_Qt4 ubuntu10.10_Qt4 winXP_updateFirst_Qt4 fedora15_Qt4 win7_updateLast_Qt4 centos-5.5-i386-server_Qt4 winXP_updateLast_Qt4)
#VMs_OS=(Windows7 Ubuntu WindowsXP Fedora Windows7 RedHat WindowsXP Windows7 Ubuntu WindowsXP Fedora Windows7 RedHat WindowsXP)
#VMs_ssh_port=(2222 2226 2224 2227 2223 2228 2225 2229 2230 2231 2232 2233 2234 2235)
#VMs_controller=(PIIX4 PIIX4 PIIX3 PIIX4 PIIX4 PIIX4 PIIX3 PIIX4 PIIX4 PIIX3 PIIX4 PIIX4 PIIX4 PIIX3)
#VMs_nictype=(82540EM 82540EM Am79C973 82540EM 82540EM 82540EM Am79C973 82540EM 82540EM Am79C973 82540EM 82540EM 82540EM Am79C973)
#VMs_nic=(e1000 e1000 pcnet e1000 e1000 e1000 pcnet e1000 e1000 pcnet e1000 e1000 e1000 pcnet)
#update_cmd=("/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ apt-get\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe /UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ apt-get\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe /UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow")  
#shutdown_cmd=("shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \shutdown\ \-h\ now" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \shutdown\ \-h\ now" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \/sbin/shutdown\ \-h\ now" "shutdown /s" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \shutdown\ \-h\ now" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \shutdown\ \-h\ now" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \/sbin/shutdown\ \-h\ now" "shutdown /s") 

VMs=(ubuntu10.10 centos-5.5-i386-server_Qt4)
VMs_OS=(Ubuntu RedHat)
VMs_ssh_port=(2226 2234)
VMs_controller=(PIIX4 PIIX4)
VMs_nictype=(82540EM 82540EM)
VMs_nic=(e1000 e1000)
update_cmd=("echo\ \"1234561\"\ \|\ \sudo\ \-S\ apt-get\ \-y\ \update" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \update")  
shutdown_cmd=("echo\ \"1234561\"\ \|\ \sudo\ \-S\ \shutdown\ \-h\ now" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \/sbin/shutdown\ \-h\ now") 
Git_cmd="git clone git://amos.git.sourceforge.net/gitroot/amos/amos"

################################
# ssh and invoke VM functions
################################

ssh_command ()
{
/usr/bin/expect <<EOD
spawn ssh -l bryanta -p $1 localhost $2
expect "bryanta@localhost's password:"
send "1234561\r"
expect eof
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

##############################
# Invoke all Physical Machines
##############################
for (( i = 0; i < ${#PhMs[$i]}; i++ ))
do
  ssh ${Users[$i]}@${PhMs[$i]} "$Git_cmd" 
  echo "sent git"
  echo "sleeping for 5 mins to complete git..."
  sleep 300
  ssh ${Users[$i]}@${PhMs[$i]} "cp amos/VMScripts/${PhMs[$i]}.sh /home/${Users[$i]}/"
  ssh ${Users[$i]}@${PhMs[$i]} "chmod +x ${PhMs[$i]}.sh"
  ssh ${Users[$i]}@${PhMs[$i]} "./${PhMs[$i]}.sh ${PhMs[$i]}"
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
        unregister_VM ${VMs[$firstVM]}         
        echo "Copying the lated update VM from walnut..."
	cd "$Path_to_VMs"
	scp -r $Path_to_Storage${VMs[$firstVM]}.vdi WalnutLast/
	register_VM ${VMs[$firstVM]} ${VMs_OS[$firstVM]} ${VMs_nictype[$firstVM]} ${VMs_controller[$firstVM]} ${VMs_ssh_port[$firstVM]} ${VMs_nic[$firstVM]}
        invoke_VM ${VMs[$firstVM]}	
        echo "Waiting for starting VM..."
	sleep 180
        echo "Registering ssh public key..."
        ssh_command ssh_command ${VMs_ssh_port[$firstVM]} ""
        sleep 120
	echo "Sending update command..."
	echo ${update_cmd[$firstVM]}
	eval cmd=\${update_cmd[$firstVM]}
	ssh_command ${VMs_ssh_port[$firstVM]} "$cmd"
        
        if [ $secondVM -lt $num_of_VMs ]
        then
            unregister_VM ${VMs[$secondVM]}         
            echo "Copying the lated update VM from walnut..."
	    cd "$Path_to_VMs"
	    scp -r $Path_to_Storage${VMs[$secondVM]}.vdi WalnutLast/
	    register_VM ${VMs[$secondVM]} ${VMs_OS[$secondVM]} ${VMs_nictype[$secondVM]} ${VMs_controller[$secondVM]} ${VMs_ssh_port[$secondVM]} ${VMs_nic[$secondVM]}
            invoke_VM ${VMs[$secondVM]}	
            echo "Waiting for starting VM..."
	    sleep 180
            echo "Registering ssh public key..."
            ssh_command ssh_command ${VMs_ssh_port[$secondVM]} ""
            sleep 120
	    echo "Sending update command..."
	    echo ${update_cmd[$secondVM]}
	    eval cmd=\${update_cmd[$secondVM]}
	    ssh_command ${VMs_ssh_port[$secondVM]} "$cmd"
        fi	
        if [ $thirdVM -lt $num_of_VMs ]
        then
            unregister_VM ${VMs[$thirdVM]}         
            echo "Copying the lated update VM from walnut..."
	    cd "$Path_to_VMs"
	    scp -r $Path_to_Storage${VMs[$thirdVM]}.vdi WalnutLast/
	    register_VM ${VMs[$thirdVM]} ${VMs_OS[$thirdVM]} ${VMs_nictype[$thirdVM]} ${VMs_controller[$thirdVM]} ${VMs_ssh_port[$thirdVM]} ${VMs_nic[$thirdVM]}
            invoke_VM ${VMs[$thirdVM]}	
            echo "Waiting for starting VM..."
	    sleep 180
            echo "Registering ssh public key..."
            ssh_command ssh_command ${VMs_ssh_port[$thirdVM]} ""
            sleep 120
	    echo "Sending update command..."
	    echo ${update_cmd[$thirdVM]}
	    eval cmd=\${update_cmd[$thirdVM]}
	    ssh_command ${VMs_ssh_port[$thirdVM]} "$cmd"
        fi	

	echo "Waiting for updating..."
	sleep 3600

	echo "Sending shutdown command..."
	echo ${shutdown_cmd[$firstVM]}
	ssh_command ${VMs_ssh_port[$firstVM]} "${shutdown_cmd[$firstVM]}"
	if [ $secondVM -lt $num_of_VMs ]
        then
            echo ${shutdown_cmd[$secondVM]}
	    ssh_command ${VMs_ssh_port[ $secondVM]} "${shutdown_cmd[$secondVM]}"
        fi	
        if [ $thirdVM -lt $num_of_VMs ]
        then
            echo ${shutdown_cmd[$thirdVM]}
	    ssh_command ${VMs_ssh_port[$thirdVM]} "${shutdown_cmd[$thirdVM]}"
        fi	

        echo "Wating for shutting down VM..."
	sleep 3600

	cd $Path_to_VMs
	echo "Copying VM back to Walnut..."
	scp WalnutLast/${VMs[$firstVM]}.vdi $Path_to_Storage        
        invoke_VM ${VMs[$firstVM]} 
        sleep 180 
        if [ $secondVM -lt $num_of_VMs ]
        then
            cd $Path_to_VMs
            scp WalnutLast/${VMs[$secondVM]}.vdi $Path_to_Storage        
            invoke_VM ${VMs[$secondVM]} 
            sleep 180
        fi	
        if [ $thirdVM -lt $num_of_VMs ]
        then
            cd $Path_to_VMs
            scp WalnutLast/${VMs[$thirdVM]}.vdi $Path_to_Storage        
            invoke_VM ${VMs[$thirdVM]} 
            sleep 180
        fi

        cd $home_dir 
        rm -rf amos/ 
        git clone git://amos.git.sourceforge.net/gitroot/amos/amos	
        if [ $? -ne 0 ]
        then  	
           echo "FAILED: git error" >> "$logs_dir"${VMs[$firstVM]}_Failed.log	
           if [ $secondVM -lt $num_of_VMs ]
           then
               echo "FAILED: git error" >> "$logs_dir"${VMs[$secondVM]}_Failed.log
           fi	
           if [ $thirdVM -lt $num_of_VMs ]
           then
            echo "FAILED: git error" >> "$logs_dir"${VMs[$thirdVM]}_Failed.log
           fi
        else 	
           echo "ok to git" 
           ssh_command ${VMs_ssh_port[$firstVM]} "rm -rf amos/" 
           ssh_command ${VMs_ssh_port[$firstVM]} "$Git_cmd"
           echo "sent git"
	   echo "waitting for git to complete..." 
	   sleep 300
	   ssh_command ${VMs_ssh_port[$firstVM]} "cp /home/bryanta/amos/VMScripts/${VMs[$firstVM]}.sh /home/bryanta/"
	   echo "VM script was copied to home directory"
	   ssh_command ${VMs_ssh_port[$firstVM]} "chmod +x ${VMs[$firstVM]}.sh"
	   ssh_command ${VMs_ssh_port[$firstVM]} "./${VMs[$firstVM]}.sh ${VMs[$firstVM]}"
	   echo "VM script was run"
           if [ $secondVM -lt $num_of_VMs ]
           then
               ssh_command ${VMs_ssh_port[$secondVM]} "rm -rf amos/" 
	       ssh_command ${VMs_ssh_port[$secondVM]} "$Git_cmd"
	       echo "sent git"
	       echo "sleep for 5 mins to complete git" 
	       sleep 300
	       ssh_command ${VMs_ssh_port[$secondVM]} "cp /home/bryanta/amos/VMScripts/${VMs[$secondVM]}.sh /home/bryanta/"
	       echo "VM script was copied to home directory"
	       ssh_command ${VMs_ssh_port[$secondVM]} "chmod +x ${VMs[$secondVM]}.sh"
	       ssh_command ${VMs_ssh_port[$secondVM]} "./${VMs[$secondVM]}.sh ${VMs[$secondVM]}"
               echo "VM script was run"
           fi	
           if [ $thirdVM -lt $num_of_VMs ]
           then
               ssh_command ${VMs_ssh_port[$thirdVM]} "rm -rf amos/" 
	       ssh_command ${VMs_ssh_port[$thirdVM]} "$Git_cmd"
	       echo "sent git"
	       echo "sleep for 5 mins to complete git" 
	       sleep 300
	       ssh_command ${VMs_ssh_port[$thirdVM]} "cp /home/bryanta/amos/VMScripts/${VMs[$thirdVM]}.sh /home/bryanta/"
	       echo "VM script was copied to home directory"
	       ssh_command ${VMs_ssh_port[$thirdVM]} "chmod +x ${VMs[$thirdVM]}.sh"
	       ssh_command ${VMs_ssh_port[$thirdVM]} "./${VMs[$thirdVM]}.sh ${VMs[$thirdVM]}"
               echo "VM script was run"
           fi
        fi	
	sleep 3600
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
echo "*** Complete logs stored on sauron.cs.umd.edu" >> log.txt
for (( i = 0; i < ${#PhMs[$i]}; i++ ))
do
  echo ============================= >> log.txt
  case "${PhMs[$i]}" in
  "128.8.126.2") echo MAC OS >> log.txt
                 ;;
  esac 
  echo ============================= >> log.txt
  cat "$logs_dir"${PhMs[$i]}_Failed.log | tail -1 >> log.txt
  cat "$logs_dir"${PhMs[$i]}.log | tail -1 >> log.txt
done
for (( i = 0; i < ${#VMs[$i]}; i++ ))
do
  echo ============================= >> log.txt
  case "${VMs[$i]}" in
  "winXP_updateFirst") echo WIN XP UPDATE FIRST >> log.txt
                       ;;
  "winXP_updateLast")  echo WIN XP UPDATE LAST >> log.txt
                       ;;
  "win7_updateFirst")  echo WIN 7 UPDATE FIRST >> log.txt
                       ;;
  "win7_updateLast")   echo WIN 7 UPDATE LAST >> log.txt
                       ;;
  "ubuntu10.10")       echo UBUNTU 10.10      >> log.txt
                       ;;
  "fedora15")          echo FEDORA 15         >> log.txt
                       ;;
  "centos-5.5-i386-server") echo CENT OS 5.5  >> log.txt 
                            ;;
  "winXP_updateFirst_Qt4") echo WIN XP UPDATE FIRST WITH HAWKEYE >> log.txt
                       ;;
  "winXP_updateLast_Qt4")  echo WIN XP UPDATE LAST WITH HAWKEYE >> log.txt
                       ;;
  "win7_updateFirst_Qt4")  echo WIN 7 UPDATE FIRST WITH HAWKEYE >> log.txt
                       ;;
  "win7_updateLast_Qt4")   echo WIN 7 UPDATE LAST WITH HAWKEYE >> log.txt
                       ;;
  "ubuntu10.10_Qt4")       echo UBUNTU 10.10 WITH HAWKEYE      >> log.txt
                       ;;
  "fedora15_Qt4")          echo FEDORA 15 WITH HAWKEYE         >> log.txt
                       ;;
  "centos-5.5-i386-server_Qt4") echo CENT OS 5.5 WITH HAWKEYE  >> log.txt 
                            ;;
  esac 
  echo ============================= >> log.txt
  cat "$logs_dir"${VMs[$i]}_Failed.log | tail -1 >> log.txt
  cat "$logs_dir"${VMs[$i]}.log | tail -1 >> log.txt
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
	./sendEmail -f bryanta@sauron.cs.umd.edu -u [AMOS Daily Build] FAILED -o message-file=log.txt -t dungtq1387@gmail.com -cc bryanta@cs.umd.edu
fi
if [ $count1 == '0' ]
then
	./sendEmail -f bryanta@sauron.cs.umd.edu -u [AMOS Daily Build] SUCCESS -o message-file=log.txt -t dungtq1387@gmail.com -cc bryanta@cs.umd.edu
fi
echo "Email sent"
rm -f log.txt
cd "$logs_dir"
mkdir `date +%y%m%d`
mv *.log `date +%y%m%d`/ 
mv log.txt `date +%y%m%d`/
exit


