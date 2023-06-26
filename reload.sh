#!/bin/bash

sudo rm /dev/mmap_dev
sudo rmmod mmap
make
sudo insmod mmap.ko
sudo mknod /dev/mmap_dev c 505 0
sudo chmod a+w /dev/mmap_dev

# sudo rmmod proc
# make
# sudo insmod proc.ko