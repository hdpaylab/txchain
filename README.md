# txchain README

tx speed up project


# ubuntu required packages

sudo apt install  autoconf libtool cmake g++ libssl-dev pkg-config

sudo apt install  libboost-system-dev libboost-thread-dev

sudo apt install libzmqpp-dev 

sudo apt install postgresql 

sudo apt-get install libpq-dev 

sudo apt install libleveldb-dev 



# Basic packages 


secp256k1

git clone https://github.com/bitcoin-core/secp256k1.git

cd secp256k1

./autogen.sh

./configure --enable-module-recovery

make && sudo make install

cd -



json_spirit

git clone https://github.com/smlee-hdactech/json_spirit.git

cd json_spirit

cmake -S . -B_build

cmake --build _build

sudo make install -C_build

cd -

