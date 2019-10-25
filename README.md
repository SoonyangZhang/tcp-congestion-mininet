# tcp-congestion-mininet
test tcp congestion fairness on mininet  
'''
cd  tcp-congestion-mininet  
mkdir build &&cd build  
cmake ..  
make  
cd ..  
sudo su  
python 2h2r.py  
'''  
client:  echo_client.cc  
server:  echo_server.cc  

```  
iperf -s -p 5566 -i 1  
iperf -c 10.0.0.2 -p 5566  
```  
what a lession I learnt: don not close fd in redis net callback.  

