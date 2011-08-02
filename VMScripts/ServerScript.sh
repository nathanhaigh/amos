#!/bin/bash
###############################
# Server Configuration
###############################
Path_to_VB_dir="C:\Users\bryanta\VirtualBox VMs"
Path_to_VBoxManage="C:\Program Files\Oracle\VirtualBox"
Path_to_VMs="E:\BRYANTA\VMs"
home_dir=/home/bryanta
logs_dir=C:/Program\ \Files\ \(x86\)/ICW/home/ssh/VMlogs/
###############################
# VM Configuration
###############################
VMs=(win7_updateFirst ubuntu10.10 winXP_updateFirst fedora15 win7_updateLast centos-5.5-i386-server winXP_updateLast)
VMs_OS=(Windows7 Ubuntu WindowsXP Fedora Windows7 RedHat WindowsXP)
VMs_ssh_port=(2222 2226 2224 2227 2223 2228 2225)
VMs_controller=(PIIX4 PIIX4 PIIX3 PIIX4 PIIX4 PIIX4 PIIX3)
VMs_nictype=(82540EM 82540EM Am79C973 82540EM 82540EM 82540EM Am79C973)
VMs_nic=(e1000 e1000 pcnet e1000 e1000 e1000 pcnet)
update_cmd=("/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ apt-get\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe /UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \yum\ \-y\ \update" "/cygdrive/c/Windows/System32/wuauclt.exe\ \/UpdateNow")  
shutdown_cmd=("shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \shutdown\ \-h\ now" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \shutdown\ \-h\ now" "shutdown /s" "echo\ \"1234561\"\ \|\ \sudo\ \-S\ \/sbin/shutdown\ \-h\ now" "shutdown /s") 
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
# Invoke all VMs
##############################

mkdir "$Path_to_VMs\WalnutLast"
for (( i = 0; i < ${#VMs[$i]}; i++ ))
do
        unregister_VM ${VMs[$i]}         
        echo "Copying the lated update VM from walnut..."
	cd "$Path_to_VMs"
	scp -r bryanta@walnut.umiacs.umd.edu:/scratch1/bryanta/WalnutLast/${VMs[$i]}.vdi WalnutLast/
	register_VM ${VMs[$i]} ${VMs_OS[$i]} ${VMs_nictype[$i]} ${VMs_controller[$i]} ${VMs_ssh_port[$i]} ${VMs_nic[$i]}
        invoke_VM ${VMs[$i]}	
        echo "Waiting for starting VM..."
	sleep 180
	echo "Sending update command..."
	echo ${update_cmd[$i]}
	eval cmd=\${update_cmd[$i]}
	ssh_command ${VMs_ssh_port[$i]} "$cmd"	
	echo "Waiting for updating (1 hours)..."
	sleep 1800
	echo "Sending shutdown command..."
	echo ${shutdown_cmd[$i]}
	ssh_command ${VMs_ssh_port[$i]} "${shutdown_cmd[$i]}"
	echo "Wating for shutting down VM..."
	sleep 1800
	cd $Path_to_VMs
	echo "Copying VM back to Walnut..."
	scp WalnutLast/${VMs[$i]}.vdi bryanta@walnut.umiacs.umd.edu:/scratch1/bryanta/WalnutLast/        
        invoke_VM ${VMs[$i]} 
        sleep 180 
        cd $home_dir 
        rm -rf amos/ 
        git clone git://amos.git.sourceforge.net/gitroot/amos/amos	
        if [ $? -ne 0 ]
        then  	
           echo "FAILED: git error" >> "$logs_dir"${VMs[$i]}_Failed.log	
        else 	
           echo "ok to git" 
           ssh_commnad ${VMs_ssh_port[$i]} "rm -rf amos/" 
           ssh_command ${VMs_ssh_port[$i]} "$Git_cmd"
           echo "sent git"
	   echo "sleep for 5 mins to complete git" 
	   sleep 300
	   ssh_command ${VMs_ssh_port[$i]} "cp /home/bryanta/amos/VMScritps/${VMs[$i]}.sh /home/bryanta/"
	   echo "VM script was copied to home directory"
	   ssh_command ${VMs_ssh_port[$i]} "chmod +x ${VMs[$i]}.sh"
	   ssh_command ${VMs_ssh_port[$i]} "./${VMs[$i]}.sh ${VMs[$i]}"
	   echo "VM script was run"
        fi	
        let "even_or_odd= i + 1"
        let "even_or_odd= even_or_odd % 3"	
        if [ $even_or_odd == 0 ]
	then
		sleep 1800
	fi

done

##################################
# merge logs and send email
##################################
echo "waiting for 8 VM log files..."

cd "$logs_dir"
stop=1
while [ $stop == '1' ] 
do
	count=0
	for i in $( ls "$logs_dir" | grep .log ); do
		count=$(( $count + 1 ))
	done
	if [ $count == '8' ]; then
		stop=$(( $stop +1 ))
	fi
done
sleep 300

echo "combining 8 VMs log files..."
date > log.txt
echo "*** Complete logs stored on sauron.cs.umd.edu" >> log.txt
echo ======================== >> log.txt
echo WIN XP UPDATE FIRST      >> log.txt
echo ======================== >> log.txt

cat "$logs_dir"winXP_updateFirst_Failed.log | tail -1 >> log.txt
cat "$logs_dir"winXP_updateFirst.log | tail -1 >> log.txt

echo ======================== >> log.txt
echo WIN 7 UPDATE FIRST   >> log.txt
echo ======================== >> log.txt
cat "$logs_dir"win7_updateFirst_Failed.log | tail -1 >> log.txt
cat "$logs_dir"win7_updateFirst.log | tail -1 >> log.txt

echo ======================== >> log.txt
echo UBUNTU 10.10             >> log.txt
echo ======================== >> log.txt
cat "$logs_dir"ubuntu10.10_Failed.log | tail -1 >> log.txt
cat "$logs_dir"ubuntu10.10.log | tail -1 >> log.txt

echo ======================== >> log.txt
echo CENT OS 5.5              >> log.txt
echo ======================== >> log.txt
cat "$logs_dir"centos-5.5-i386-server_Failed.log | tail -1 >> log.txt
cat "$logs_dir"centOS-5.5-i386-server.log | tail -1 >> log.txt

echo ======================== >> log.txt
echo FEDORA 15                >> log.txt
echo ======================== >> log.txt
cat "$logs_dir"fedora15_Failed.log | tail -1 >> log.txt
cat "$logs_dir"fedora15.log | tail -1 >> log.txt

echo ======================== >> log.txt
echo WIN XP UPDATE LAST       >> log.txt
echo ======================== >> log.txt
cat "$logs_dir"winXP_updateLast_Failed.log | tail -1 >> log.txt
cat "$logs_dir"winXP_updateLast.log | tail -1 >> log.txt

echo ======================== >> log.txt
echo WIN 7 UPDATE LAST        >> log.txt
echo ======================== >> log.txt
cat "$logs_dir"win7_updateLast_Failed.log | tail -1 >> log.txt
cat "$logs_dir"win7_updateLast.log | tail -1 >> log.txt

echo ======================== >> log.txt
echo MAC OS                   >> log.txt
echo ======================== >> log.txt
cat "$logs_dir"macOS_Failed.log | tail -1 >> log.txt
cat "$logs_dir"macOS.log | tail -1 >> log.txt

echo "log.txt created"
cp log.txt C:/Users/bryanta/VMlogs/log.txt
cd "C:\Users\bryanta\Downloads\sendEmail-v156-notls"

count1=0
for i in $( ls "$logs_dir" | grep Failed )
do
	count1=$(( $count1 + 1 ))
done

###############################
# Sending email to group
###############################
echo "Sending email to group..."
if [ $count1 != '0' ]
then
	./sendEmail -f bryanta@sauron.cs.umd.edu -u [AMOS Daily Build] FAILED -o message-file=C:/Users/bryanta/VMlogs/log.txt -t dungtq1387@gmail.com -cc bryanta@cs.umd.edu,atif@cs.umd.edu,mpop@umiacs.umd.edu,dsommer@umiacs.umd.edu
fi
if [ $count1 == '0' ]
then
	./sendEmail -f bryanta@sauron.cs.umd.edu -u [AMOS Daily Build] SUCCESS -o message-file=C:/Users/bryanta/VMlogs/log.txt -t dungtq1387@gmail.com -cc bryanta@cs.umd.edu,atif@cs.umd.edu,mpop@umiacs.umd.edu,dsommer@umiacs.umd.edu
fi
echo "Email sent"

cd "$logs_dir"
mkdir `date +%y%m%d`
mv *.log `date +%y%m%d`/ 
mv log.txt `date +%y%m%d`/
exit


