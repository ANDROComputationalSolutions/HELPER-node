# Makefile
# Adapted from rf95_client and rf95_server from RadioHead/examples/raspi/rf95

CXX            = g++
override CXXFLAGS        += -fPIC -std=c++11 -DRASPBERRY_PI -DBCM2835_NO_DELAY_COMPATIBILITY -D__BASEFILE__=\"$*\"
override RADIOHEADBASE    = RadioHead
override INCLUDE         += -I$(RADIOHEADBASE)
override LDFLAGS         += -L${PWD}
override LDLIBS          += -lbcm2835 -lpthread -lzmq -lrh -lgps

.PHONY: all clean

all: benchmark

RH = RH_RF95 RasPi RHHardwareSPI RHGenericDriver RHGenericSPI RHSPIDriver
RH_OBJS = $(RH:=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDE) $<

RasPi.o: $(RADIOHEADBASE)/RHutil/RasPi.o
RH_RF95.o: $(RADIOHEADBASE)/RH_RF95.o
RHDatagram.o: $(RADIOHEADBASE)/RHDatagram.o
RHHardwareSPI.o: $(RADIOHEADBASE)/RHHardwareSPI.o
RHSPIDriver.o: $(RADIOHEADBASE)/RHSPIDriver.o
RHGenericDriver.o: $(RADIOHEADBASE)/RHGenericDriver.o
RHGenericSPI.o: $(RADIOHEADBASE)/RHGenericSPI.o

#benchmark 
benchmark: helperd_benchmark benchmark_send_text benchmark_recv_text 

helperd_benchmark: $(RH_OBJS) helperd_benchmark.o LoraMac.impl.o LoraRadio.o RoutingBenchmark.o helper_packet.o helperMessage.recv.o helperMessage.send.o 
	$(CXX) -o helperd_benchmark $^ -lzmq -lpthread -lbcm2835 --std=c++11

benchmark_send_text: 
	$(CXX) -o benchmark_send_text benchmark_send_text.cpp helperMessage.send.cpp -lzmq 
	
benchmark_recv_text: 
	$(CXX) -o benchmark_recv_text benchmark_recv_text.cpp helperMessage.recv.cpp -lzmq 

clean:
	rm -f *.o 
	rm -f helperd_benchmark
	rm -f benchmark_send_text
	rm -f benchmark_recv_text
	
