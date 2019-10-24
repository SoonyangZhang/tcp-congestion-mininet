#!/usr/bin/python

import smtplib
from email.mime.text import MIMEText
from email.header import Header

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.cli import CLI
import time
import datetime
import subprocess
import os,signal
import sys
#    ___r1____
#   /          \0  1
# h1            r3---h2
#  \           /2
#   ---r2-----
bottleneckbw=16
nonbottlebw=500;
max_rtt=300
bottleneckQ=bottleneckbw*1000*max_rtt/(1500*8)
nonbottleneckQ=nonbottlebw*1000*max_rtt/(1500*8)
net = Mininet( cleanup=True )
h1 = net.addHost('h1',ip='10.0.1.1')
r1 = net.addHost('r1',ip='10.0.1.2')
r2 = net.addHost('r2',ip='10.0.3.2')
r3 = net.addHost('r3',ip='10.0.5.1')
h2 = net.addHost('h2',ip='10.0.5.2')
c0 = net.addController('c0')
net.addLink(h1,r1,intfName1='h1-eth0',intfName2='r1-eth0',cls=TCLink , bw=nonbottlebw, delay='10ms', max_queue_size=nonbottleneckQ)
net.addLink(r1,r3,intfName1='r1-eth1',intfName2='r3-eth0',cls=TCLink , bw=nonbottlebw, delay='10ms', max_queue_size=nonbottleneckQ)
net.addLink(r3,h2,intfName1='r3-eth1',intfName2='h2-eth0',cls=TCLink , bw=bottleneckbw, delay='20ms', max_queue_size=bottleneckQ)
net.addLink(h1,r2,intfName1='h1-eth1',intfName2='r2-eth0',cls=TCLink , bw=nonbottlebw, delay='20ms', max_queue_size=nonbottleneckQ)
net.addLink(r2,r3,intfName1='r2-eth1',intfName2='r3-eth2',cls=TCLink , bw=nonbottlebw, delay='30ms', max_queue_size=nonbottleneckQ)

net.build()

h1.cmd("ifconfig h1-eth0 10.0.1.1/24")
h1.cmd("ifconfig h1-eth1 10.0.3.1/24")
h1.cmd("ip route flush all proto static scope global")
h1.cmd("ip route add 10.0.1.1/24 dev h1-eth0 table 5000")
h1.cmd("ip route add default via 10.0.1.2 dev h1-eth0 table 5000")

h1.cmd("ip route add 10.0.3.1/24 dev h1-eth1 table 5001")
h1.cmd("ip route add default via 10.0.3.2 dev h1-eth1 table 5001")
h1.cmd("ip rule add from 10.0.1.1 table 5000")
h1.cmd("ip rule add from 10.0.3.1 table 5001")
h1.cmd("ip route add default gw 10.0.1.2  dev h1-eth0")
#that be a must or else a tcp client would not know how to route packet out
h1.cmd("route add default gw 10.0.1.2  dev h1-eth0") #would not work for the second part when a tcp client bind a address


r1.cmd("ifconfig r1-eth0 10.0.1.2/24")
r1.cmd("ifconfig r1-eth1 10.0.2.1/24")
r1.cmd("ip route add to 10.0.1.0/24 via 10.0.1.1")
r1.cmd("ip route add to 10.0.2.0/24 via 10.0.2.2")
r1.cmd("ip route add to 10.0.5.0/24 via 10.0.2.2")
r1.cmd('sysctl net.ipv4.ip_forward=1')

r3.cmd("ifconfig r3-eth0 10.0.2.2/24")
r3.cmd("ifconfig r3-eth1 10.0.5.1/24")
r3.cmd("ifconfig r3-eth2 10.0.4.2/24")
r3.cmd("ip route add to 10.0.1.0/24 via 10.0.2.1")
r3.cmd("ip route add to 10.0.2.0/24 via 10.0.2.1")
r3.cmd("ip route add to 10.0.5.0/24 via 10.0.5.2")
r3.cmd("ip route add to 10.0.4.0/24 via 10.0.4.1")
r3.cmd("ip route add to 10.0.3.0/24 via 10.0.4.1")
r3.cmd('sysctl net.ipv4.ip_forward=1')

r2.cmd("ifconfig r2-eth0 10.0.3.2/24")
r2.cmd("ifconfig r2-eth1 10.0.4.1/24")
r2.cmd("ip route add to 10.0.3.0/24 via 10.0.3.1")
r2.cmd("ip route add to 10.0.4.0/24 via 10.0.4.2")
r2.cmd("ip route add to 10.0.5.0/24 via 10.0.4.2")
r2.cmd('sysctl net.ipv4.ip_forward=1')

h2.cmd("ifconfig h2-eth0 10.0.5.2/24")
h2.cmd("route add default gw 10.0.5.1")
#ping -I src dst
net.start()
time.sleep(1)
print "host2 ip", h2.IP()
local_ip1="10.0.1.1"
local_ip2="10.0.3.1"
serv_port=3333
congestion="bbr"
log_name="server_test.txt"
flows1=2
flows2=2
total_flows=flows1+flows2
server_cmd_common="./build/echo_server  -p %s -l %s -f %s"
server_cmd=server_cmd_common%(str(serv_port),log_name,str(total_flows))
client_cmd_common="./build/echo_client -l %s -h %s -g %s -p %s -c %s -f %s"
client_id=1
client_cmd=client_cmd_common%(local_ip1,h2.IP(),congestion,str(serv_port),str(client_id),str(flows1))
server_p=h2.popen(server_cmd)
client_p=[]
p=h1.popen(client_cmd)
client_p.append(p)
client_id=client_id+flows1
client_cmd=client_cmd_common%(local_ip2,h2.IP(),congestion,str(serv_port),str(client_id),str(flows2))
p=h1.popen(client_cmd)
client_p.append(p)
total=len(client_p)
clients_done=False;
server_done=False
while 1:
    alive=total
    for i in range(total):
        ret=subprocess.Popen.poll(client_p[i])
        if ret is None:
            continue
        else:
            alive-=1
    if alive==0:
        clients_done=True;
        break
    else:
        continue
    ret=subprocess.Popen.poll(server_p)
    if ret is None:
        continue
    else:
        server_done=True
        break
if server_done==False:
    os.killpg(os.getpgid(server_p.pid),signal.SIGTERM)
    server_p.wait();
net.stop()
print "stop"
msg_from = '865678017@qq.com'  
passwd = 'ask qq for auth code'
msg_to = '865678017@qq.com'  

subject = "python tcp rate test"  
content = "5h running done"
msg = MIMEText(content)
msg['Subject'] = subject
msg['From'] = msg_from
msg['To'] = msg_to
try:
    s = smtplib.SMTP_SSL("smtp.qq.com", 465)
    s.login(msg_from, passwd)
    s.sendmail(msg_from, msg_to, msg.as_string())
    print "success"
except s.SMTPException, e:
    print "error"
finally:
    s.quit()
