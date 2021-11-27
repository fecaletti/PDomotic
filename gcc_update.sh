echo 'export LD_LIBRARY_PATH=/opt/gcc-10.1.0/lib:$LD_LIBRARY_PATH' >> ~/.bashrc &&
. ~/.bashrc &&
sudo ln -s /usr/include/arm-linux-gnueabihf/sys /usr/include/sys &&
sudo ln -s /usr/include/arm-linux-gnueabihf/bits /usr/include/bits &&
sudo ln -s /usr/include/arm-linux-gnueabihf/gnu /usr/include/gnu &&
sudo ln -s /usr/include/arm-linux-gnueabihf/asm /usr/include/asm &&
sudo ln -s /usr/lib/arm-linux-gnueabihf/crti.o /usr/lib/crti.o &&
sudo ln -s /usr/lib/arm-linux-gnueabihf/crt1.o /usr/lib/crt1.o &&
sudo ln -s /usr/lib/arm-linux-gnueabihf/crtn.o /usr/lib/crtn.o