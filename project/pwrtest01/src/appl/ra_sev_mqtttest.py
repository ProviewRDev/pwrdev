#!/usr/bin/python3
#
import paho.mqtt.client as mqtt
import sys
import time
from datetime import datetime
import pandas as pd
import json
import pwrtest
from rt_sev_msg import *
from rt_hash_msg import *

# Print reply
def on_message(client, userdata, message):
    #print('New message', message.payload)
    ctx.reply = json.loads(str(message.payload.decode("utf-8")))
    ctx.received = 1
    
class Ctx:
  def history(self):
    # History request
    d = (
      ('VolPwrTest01d:Test01d-H1-Av1', 'ActualValue', 45, 55, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H111-Av1', 'ActualValue', 45, 55, HASH__NOTFOUND),
      ('VolPwrTest01d:Test01d-H1-Iv1', 'ActualValue', 45, 55, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H1-Dv5', 'ActualValue', 0.3, 0.7, SEV__SUCCESS)
            )
    client.subscribe("repl/history", 1);

    for i in range(len(d)):
      self.received = 0
      #print(i, d[i][0], d[i][1])
      msg = '{"action":"history","reply":"repl/history","server":"localhost","object":"' + d[i][0] + '","attribute":"' + d[i][1] + '","from":"0:15:0","to":"now","maxrows":2000}'
      #print(msg)
      client.publish('proviewr/server', msg)
    
      # Wait for reply
      for j in range (0, 3):
        client.loop_start()
        time.sleep(1)
        if self.received == 1:
          break
        client.loop_stop()
      if self.received == 0:
        self.logger.log('E', 'history, No response from server')
        return
    
      status = self.reply['status']
      #print("Status", status)
      if status != d[i][4]:
        self.logger.vlog('E', 'history, Unexpected status %s != %s', str(status),
                         str(d[i][0]))
        return
      if status & 1:
        #print(ctx.reply)
        ser = pd.Series(ctx.reply['values'])
        mean = ser.mean()
        std = ser.std()
        #print('mean', mean, 'std', std)
        if mean < d[i][2] or mean > d[i][3]:
          self.logger.vlog('E', 'history, Unexpected mean value %s', str(mean))
          return
          
    self.logger.log('S', 'history, successfull completion')

  def eventhist(self):
    # Eventhist request
    # Eventprio 1, A 1, B 2, C 4, D 8
    # Options   2, time 1, type 2, prio 4, text 8, name 16, id 32
    # Eventtype 3, ack 1, block 2, cancel 4, cancelblock 8, missing 16,
    #              reblock 32, return 64, unblock 128, infosuccess 256,
    #              alarm 512, maintenancealarm 1024, systemalarm 2048,
    #              useralarm1-4 4096, 8192, 16384, 32768, info 65536
    # Object, eventprio, options, eventtype, minlen, maxlen, status
    d = (
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 0, 0, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 1, 0, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 2, 0, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 4, 0, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 8, 0, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 16, 0, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 32, 0, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 1 | 8 | 16, 0, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 63, 0, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 0, 512, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 0, 64, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 0, 0, 512 | 64, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 1, 0, 0, 20, 20, SEV__SUCCESS),
      ('VolPwrTest01d:Test01d-H2-HistEvents', 8, 0, 0, 20, 20, SEV__NOPOINTS),
      ('VolPwrTest01d:Test01d-H2-No', 8, 0, 0, 20, 20, HASH__NOTFOUND)
        )
    client.subscribe("repl/histevent", 1);

    for i in range(len(d)):
      self.received = 0
      #print(i, d[i][0], d[i][1])
      msg = '{"action":"eventhist","reply":"repl/histevent","server":"localhost","object":"' + d[i][0] + '","from":"7 0:15:0","to":"now","maxrows":20'
      if d[i][1] != 0:
          msg += ',"eventprio":' + str(d[i][1])
      if d[i][2] != 0:
          msg += ',"options":' + str(d[i][2])
      if d[i][3] != 0:
          msg += ',"eventtype":' + str(d[i][3])
      msg += '}'
      #print(msg)
      client.publish('proviewr/server', msg)
    
      # Wait for reply
      for j in range (0, 3):
        client.loop_start()
        time.sleep(1)
        if self.received == 1:
          break
        client.loop_stop()
      if self.received == 0:
        self.logger.log('E', 'eventhist, No response from server')
        return
    
      status = self.reply['status']
      #print("Status", status)
      if status != d[i][6]:
        self.logger.vlog('E', 'eventhist, Unexpected status %s != %s', str(status),
                         str(d[i][0]))
        return
      if status & 1:
        #print(ctx.reply)
        if d[i][2] & 1 or d[i][2] == 0:
            length = len(ctx.reply['time'])
            #print("time len", length)
            if length < d[i][4] or length > d[i][5]:
                self.logger.vlog('E', 'eventhist, Unexpected time length %s', str(length))
        if d[i][2] & 2 or d[i][2] == 0:
            length = len(ctx.reply['type'])
            #print("type len", length)
            if length < d[i][4] or length > d[i][5]:
                self.logger.vlog('E', 'eventhist, Unexpected type length %s', str(length))
        if d[i][2] & 4:
            length = len(ctx.reply['prio'])
            #print("prio len", length)
            if length < d[i][4] or length > d[i][5]:
                self.logger.vlog('E', 'eventhist, Unexpected prio length %s', str(length))
        if d[i][2] & 8 or d[i][2] == 0:
            length = len(ctx.reply['text'])
            #print("text len", length)
            if length < d[i][4] or length > d[i][5]:
                self.logger.vlog('E', 'eventhist, Unexpected text length %s', str(length))
        if d[i][2] & 16:
            length = len(ctx.reply['name'])
            #print("name len", length)
            if length < d[i][4] or length > d[i][5]:
                self.logger.vlog('E', 'eventhist, Unexpected name length %s', str(length))
        if d[i][2] & 32:
            length = len(ctx.reply['id_idx'])
            #print("idx  len", length)
            if length < d[i][4] or length > d[i][5]:
                self.logger.vlog('E', 'eventhist, Unexpected id length %s', str(length))

            
    self.logger.log('S', 'eventhist, successfull completion')



# Connect to MQTT on localhost
ctx = Ctx()
ctx.logger = pwrtest.logger('sev-mqtt_server', '$pwrp_log/sev_mqtt_server.tlog')
client = mqtt.Client('MyClient')
client.username_pw_set('pwrp','pwrp')
client.on_message = on_message
client.connect('localhost')

ctx.history()
ctx.eventhist()

client.disconnect()
time.sleep(1)
