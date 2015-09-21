###########################################
# Debian boot script for Android          #
# Built by Bruzzo Paolo and Casula Dario  #
# Thanks to:                              #
# Matteo Ferroni                          #
###########################################

###########################################
# Variables                               #
###########################################
export host=phone1
export kit=/sdcard
export mnt=/data/local/mnt
busybox mkdir -p $mnt

export PATH=/usr/bin:/usr/sbin:/bin:$PATH
export TERM=linux
export HOME=/root

###########################################
# Mounting filesystem                     #
###########################################
echo "Mounting Loop Device..."
busybox mount -o loop,noatime $kit/debian.img $mnt
if [ $? -ne 0 ];then echo "Unable to mount the loop device!"; else echo "loop device mounted"; fi

busybox chroot $mnt mount -t devpts devpts /dev/pts
if [ $? -ne 0 ];then echo "Unable to mount /dev/pts!"; else echo "pts mounted"; fi

busybox chroot $mnt mount -t proc proc /proc
if [ $? -ne 0 ];then echo "Unable to mount /proc!"; else echo "proc mounted"; fi

busybox chroot $mnt mount -t sysfs sysfs /sys
if [ $? -ne 0 ];then echo "Unable to mount /sys!"; else echo "sys mounted"; fi

###########################################
# Setting hostname                        #
###########################################
echo -n "Setting hostname..."
echo $host > /proc/sys/kernel/hostname
echo $host > $mnt/etc/hostname

echo "Done"

###########################################
# CHROOT                                  #
###########################################
echo "Chrooting into Debian Squeeze!"
busybox chroot $mnt /bin/bash


###########################################
# After exiting from Debian               #
###########################################
echo -n "Umount filesystem..."
busybox chroot $mnt umount /proc /dev/pts /sys
umount -l $mnt
echo "Done"
