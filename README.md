# txchain
tx speed up processing

ubuntu 에서 빌드 방법
빌드 전에 설치해야 할 패키지
sudo apt install -y autoconf libtool cmake g++ libssl-dev pkg-config
sudo apt install -y libboost-system-dev libboost-thread-dev

sudo apt install libzmqpp-dev -y
sudo apt install postgresql -y
sudo apt-get install libpq-dev -y
sudo apt install libleveldb-dev -y


직접 빌드해서 설치해야 할 개발용 라이브러리


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

