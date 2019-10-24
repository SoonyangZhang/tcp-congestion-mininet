#!/usr/bin/python
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.cli import CLI
from mininet.link import TCLink
import time
import subprocess
import os,signal
import sys
#    ___r1____
#   /          \0  1
# h1            r3---h2
#             
#
bottleneckbw=4
nonbottlebw=500;  
max_queue_size =bottleneckbw*1000*100/(1500*8) 
net = Mininet( cleanup=True )
h1 = net.addHost('h1',ip='10.0.1.1')
r1 = net.addHost('r1',ip='10.0.1.2')
r3 = net.addHost('r3',ip='10.0.5.1')
h2 = net.addHost('h2',ip='10.0.5.2')
c0 = net.addController('c0')
net.addLink(h1,r1,intfName1='h1-eth0',intfName2='r1-eth0',cls=TCLink , bw=nonbottlebw, delay='20ms', max_queue_size=max_queue_size)
net.addLink(r1,r3,intfName1='r1-eth1',intfName2='r3-eth0',cls=TCLink , bw=bottleneckbw, delay='20ms', max_queue_size=max_queue_size)
net.addLink(r3,h2,intfName1='r3-eth1',intfName2='h2-eth0',cls=TCLink , bw=nonbottlebw, delay='10ms', max_queue_size=max_queue_size)
net.build()
h1.cmd("ifconfig h1-eth0 10.0.1.1/24")
h1.cmd("route add default gw 10.0.1.2 dev h1-eth0")

r1.cmd("ifconfig r1-eth0 10.0.1.2/24")
r1.cmd("ifconfig r1-eth1 10.0.2.1/24")
r1.cmd("ip route add to 10.0.1.0/24 via 10.0.1.1")
r1.cmd("ip route add to 10.0.2.0/24 via 10.0.2.2")
r1.cmd("ip route add to 10.0.5.0/24 via 10.0.2.2")
r1.cmd('sysctl net.ipv4.ip_forward=1')

r3.cmd("ifconfig r3-eth0 10.0.2.2/24")
r3.cmd("ifconfig r3-eth1 10.0.5.1/24")
r3.cmd("ip route add to 10.0.1.0/24 via 10.0.2.1")
r3.cmd("ip route add to 10.0.2.0/24 via 10.0.2.1")
r3.cmd("ip route add to 10.0.5.0/24 via 10.0.5.2")
r3.cmd('sysctl net.ipv4.ip_forward=1')

h2.cmd("ifconfig h2-eth0 10.0.5.2/24")
h2.cmd("route add default gw 10.0.5.1")

net.start()
time.sleep(1)
print "host1 ip",h1.IP()
print "host2 ip", h2.IP()
serv_port=3333
log_name="server_test.txt"
server_cmd_common="./build/echo_server  -p %s -l %s"
server_cmd=server_cmd_common%(str(serv_port),log_name)
client_cmd_common="./build/echo_client -h %s  -p %s -c %s -f %s"
client_id=1
flows=3
client_cmd=client_cmd_common%(h2.IP(),str(serv_port),str(client_id),str(flows))
server_p=h2.popen(server_cmd)
client_p=h1.popen(client_cmd)
while 1:
    ret=subprocess.Popen.poll(client_p)
    if ret is None:
        continue
    else:
        break
os.killpg(os.getpgid(server_p.pid),signal.SIGTERM)
server_p.wait();
net.stop()
print "stop"

