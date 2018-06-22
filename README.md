# HELPER-node

Website: http://helper.androcs.io/site/ 

This is the reference implementation for the HELPER network stack. 

See docs/getting_started.docx for additional information about RPI setup and testing this reference implementation 

RPI Setup: 

1. Uses Raspbian Stretch Lite operating system, image is available at https://www.raspberrypi.org/downloads/raspbian/ 

2. Lora Transceiver Breakout, information available at https://www.adafruit.com/product/3072?gclid=Cj0KCQjwjN7YBRCOARIsAFCb936_JUJftnJBQTzqC5m3BoLv6wjOvZYwLvghfHhElgVuzSA2p-92RuoaAmgqEALw_wcB 

Software Dependencies and installation instructions: 

1. RadioHead (https://github.com/hallard/RadioHead) 
    Git submodule, see below 
2. bcm2835 (www.airspayce.com/mikem/bcm2835/)
    wget www.airspayce.com/mikem/bcm2835/bcm2835-1.56.tar.gz
    tar zxvf bcm2835-1.56.tar.gz
    cd bcm2835-1.56 
    ./configure
    make
    sudo make check
    sudo make install 
3. ZMQ (http://zeromq.org/)
    sudo apt-get install libzmq3-dev 
4. RapidJson (http://rapidjson.org/md_doc_faq.html)
    sudo apt-get install rapidjson-dev 

Building reference implementation:
1. git clone https://github.com/ANDROComputationalSolutions/HELPER-node.git 
2. cd HELPER-node/ 
3. git submodule update --init 
4. make 

If build is successful, the following exectuables will be in the current working directory 
    helperd_benchmark, benchmark_send_text, and benchmark_recv_text 
 
Testing the reference implementation: 

Testing the reference implementation requires two HELPER nodes. Each node will run an
instance of helperd_benchmark. One node will run benchmark_send_text and the other
will run benchmark_recv_text. 

HELPER node 1 commands 
1. sudo ./helperd_benchmark 1 
2. ./benchmark_send_text 2 

HELPER node 2 commands 
1. sudo ./helperd_benchmark 2 
2. ./benchmark_recv_text 
