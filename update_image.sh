sudo losetup /dev/loop0 bin/floppy.img
sudo mount /dev/loop0 /mnt
sudo cp bin/kernel /mnt/kernel
sudo umount /dev/loop0
sudo losetup -d /dev/loop0