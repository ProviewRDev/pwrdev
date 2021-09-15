#!/usr/bin/python3
#
import paho.mqtt.client as mqtt
import sys
import time
from datetime import datetime
import json
import pwrtest
from rt_gdh_msg import *

# Print reply
def on_message(client, userdata, message):
    #print('New message', message.payload)
    ctx.reply = json.loads(str(message.payload.decode("utf-8")))
    ctx.received = 1
    
class Ctx:
  def get(self):
    # Get request
    d = (
      ('Test01c-MqttServer-Av1.ActualValue', '22.22', GDH__SUCCESS),
      ('Test01c-MqttServer-Av1.No', '', GDH__ATTRIBUTE),
      ('Test01c-MqttServer-Dv1.ActualValue', '0', GDH__SUCCESS),
      ('Test01c-MqttServer-Dv2.ActualValue', '1', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Boolean', '1', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int8', '22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int16', '333', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int32', '4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int64', '55555', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt8', '66', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt16', '777', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt32', '8888', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt64', '99999', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float32', '22.22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float64', '4444.4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.String80', 'Why don\'t you have wings to fly with', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Time', '15-MAY-2020 08:27:50.50', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.DeltaTime', '27 8:27:50.05', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Objid', 'Test01c-MqttServer-Values', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.AttrRef', 'Test01c-MqttServer-Values.AttrRef', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.BooleanArray[4]', '1', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int8Array[4]', '22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int16Array[4]', '333', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int32Array[4]', '4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int64Array[4]', '55555', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt8Array[4]', '66', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt16Array[4]', '777', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt32Array[4]', '8888', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt64Array[4]', '99999', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float32Array[4]', '22.22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float64Array[4]', '4444.4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.String80Array[4]', 'Why don\'t you have wings to fly with', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.TimeArray[4]', '15-MAY-2020 08:27:50.50', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.DeltaTimeArray[4]', '27 8:27:50.05', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.ObjidArray[4]', 'Test01c-MqttServer-Values', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.AttrRefArray[4]', 'Test01c-MqttServer-Values.AttrRefArray[4]', GDH__SUCCESS)
            )
    client.subscribe("repl/get", 1);

    for i in range(len(d)):
      self.received = 0
      msg = '{"action":"get","attribute":"' + d[i][0] + '","reply":"repl/get"}'
      client.publish('proviewr/server', msg)
    
      # Wait for reply
      for j in range (0, 3):
        client.loop_start()
        time.sleep(1)
        if self.received == 1:
          break
        client.loop_stop()
      if self.received == 0:
        self.logger.log('E', 'No response from server')
        return
    
      status = self.reply['status']
      if status != d[i][2]:
        self.logger.vlog('E', 'get, Unexpected status %s != %s', str(status),
                         str(d[i][2]))
        return
      if status & 1:
        value = self.reply['value']
        if str(value)[:len(d[i][1])] != d[i][1]:
          self.logger.vlog('E', 'get, Unexpected value %s', str(value))
          return
          
    self.logger.log('S', 'get, successfull completion')

  def set(self):
    # Set request
    d = (
      ('Test01c-MqttServer-SetValues.Boolean', '1', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Int8', '22', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Int16', '333', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Int32', '4444', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Int64', '55555', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.UInt8', '66', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.UInt16', '777', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.UInt32', '8888', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.UInt64', '99999', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Float32', '22.22', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Float64', '4444.4444', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.String80', 'Why don\'t you have wings to fly with', GDH__SUCCESS, 1),
      ('Test01c-MqttServer-SetValues.Time', '15-MAY-2020 08:27:50.50', GDH__SUCCESS, 1),
      ('Test01c-MqttServer-SetValues.DeltaTime', '27 8:27:50.05', GDH__SUCCESS, 1),
      ('Test01c-MqttServer-SetValues.Objid', 'Test01c-MqttServer-Values', GDH__SUCCESS, 1),
      ('Test01c-MqttServer-SetValues.AttrRef', 'Test01c-MqttServer-Values.AttrRef', GDH__SUCCESS, 1),
      ('Test01c-MqttServer-SetValues.BooleanArray[4]', '1', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Int8Array[4]', '22', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Int16Array[4]', '333', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Int32Array[4]', '4444', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Int64Array[4]', '55555', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.UInt8Array[4]', '66', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.UInt16Array[4]', '777', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.UInt32Array[4]', '8888', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.UInt64Array[4]', '99999', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Float32Array[4]', '22.22', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.Float64Array[4]', '4444.4444', GDH__SUCCESS, 0),
      ('Test01c-MqttServer-SetValues.String80Array[4]', 'Why don\'t you have wings to fly with', GDH__SUCCESS, 1),
      ('Test01c-MqttServer-SetValues.TimeArray[4]', '15-MAY-2020 08:27:50.50', GDH__SUCCESS, 1),
      ('Test01c-MqttServer-SetValues.DeltaTimeArray[4]', '27 8:27:50.05', GDH__SUCCESS, 1),
      ('Test01c-MqttServer-SetValues.ObjidArray[4]', 'Test01c-MqttServer-Values', GDH__SUCCESS, 1),
      ('Test01c-MqttServer-SetValues.AttrRefArray[4]', 'Test01c-MqttServer-Values.AttrRefArray[4]', GDH__SUCCESS, 1)
            )
    client.subscribe("repl/set", 1);

    for i in range(len(d)):
      # Set value
      self.received = 0
      if d[i][3] == 0:
        msg = '{"action":"set","attribute":"' + d[i][0] + '","reply":"repl/set","value":' + d[i][1] + '}'
      else:
        msg = '{"action":"set","attribute":"' + d[i][0] + '","reply":"repl/set","value":"' + d[i][1] + '"}'
      client.publish('proviewr/server', msg)
    
      # Wait for reply
      for j in range (0, 3):
        client.loop_start()
        time.sleep(1)
        if self.received == 1:
          break
        client.loop_stop()
      if self.received == 0:
        self.logger.log('E', 'No response from server')
        return
    
      status = self.reply['status']
      if status != d[i][2]:
        self.logger.vlog('E', 'set, Unexpected status %s != %s', str(status),
                         str(d[i][2]))
        return

      # Get value and check that it's the same
      self.received = 0
      msg = '{"action":"get","attribute":"' + d[i][0] + '","reply":"repl/set"}'
      client.publish('proviewr/server', msg)
    
      # Wait for reply
      for j in range (0, 3):
        client.loop_start()
        time.sleep(1)
        if self.received == 1:
          break
        client.loop_stop()
      if self.received == 0:
        self.logger.log('E', 'set, No response from server')
        return
    
      status = self.reply['status']
      if status != d[i][2]:
        self.logger.vlog('E', 'set, Unexpected status from get %s != %s', str(status),
                         str(d[i][2]))
        return
      if status & 1:
        value = self.reply['value']
        if str(value)[:len(d[i][1])] != d[i][1]:
          self.logger.vlog('E', 'set, Unexpected value %s', str(value))
          return
          
    self.logger.log('S', 'set, successfull completion')

  def subscribe(self):
    # Subscribe request
    d = (
      ('Test01c-MqttServer-Av1.ActualValue', '22.22', GDH__SUCCESS),
      ('Test01c-MqttServer-Dv1.ActualValue', '0', GDH__SUCCESS),
      ('Test01c-MqttServer-Dv2.ActualValue', '1', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Boolean', '1', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int8', '22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int16', '333', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int32', '4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int64', '55555', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt8', '66', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt16', '777', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt32', '8888', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt64', '99999', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float32', '22.22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float64', '4444.4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.String80', 'Why don\'t you have wings to fly with', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Time', '15-MAY-2020 08:27:50.50', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.DeltaTime', '27 8:27:50.05', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Objid', 'Test01c-MqttServer-Values', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.AttrRef', 'Test01c-MqttServer-Values.AttrRef', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.BooleanArray[4]', '1', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int8Array[4]', '22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int16Array[4]', '333', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int32Array[4]', '4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int64Array[4]', '55555', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt8Array[4]', '66', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt16Array[4]', '777', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt32Array[4]', '8888', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt64Array[4]', '99999', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float32Array[4]', '22.22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float64Array[4]', '4444.4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.String80Array[4]', 'Why don\'t you have wings to fly with', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.TimeArray[4]', '15-MAY-2020 08:27:50.50', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.DeltaTimeArray[4]', '27 8:27:50.05', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.ObjidArray[4]', 'Test01c-MqttServer-Values', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.AttrRefArray[4]', 'Test01c-MqttServer-Values.AttrRefArray[4]', GDH__SUCCESS)
            )
    client.subscribe("repl/subscribe", 1);

    for i in range(len(d)):
      self.received = 0
      msg = '{"action":"subscribe","attribute":"' + d[i][0] + '","reply":"repl/subscribe","cycle":"1.0","duration":"100.0"}'
      client.publish('proviewr/server', msg)
    
      # Wait for reply
      for j in range (0, 3):
        client.loop_start()
        time.sleep(1)
        if self.received == 1:
          break
        client.loop_stop()
      if self.received == 0:
        self.logger.log('E', 'No response from server')
        return
    
      status = self.reply['status']
      if status != d[i][2]:
        self.logger.vlog('E', 'subscribe, Unexpected status %s != %s', str(status),
                         str(d[i][2]))
        return
      if status & 1:
        value = self.reply['value']
        if str(value)[:len(d[i][1])] != d[i][1]:
          self.logger.vlog('E', 'subscribe, Unexpected value %s', str(value))
          return

        # Close subscription
        subref = self.reply['subref']
        msg = '{"action":"closesub","subref":' + str(subref) + '}'
        client.publish('proviewr/server', msg)
          
    self.logger.log('S', 'subscribe, successfull completion')

  def sublist(self):
    # Sublist request
    d = (
      ('Test01c-MqttServer-Av1.ActualValue', '22.22', GDH__SUCCESS),
      ('Test01c-MqttServer-Dv1.ActualValue', '0', GDH__SUCCESS),
      ('Test01c-MqttServer-Dv2.ActualValue', '1', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Boolean', '1', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int8', '22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int16', '333', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int32', '4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int64', '55555', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt8', '66', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt16', '777', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt32', '8888', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt64', '99999', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float32', '22.22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float64', '4444.4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.String80', 'Why don\'t you have wings to fly with', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Time', '15-MAY-2020 08:27:50.50', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.DeltaTime', '27 8:27:50.05', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Objid', 'Test01c-MqttServer-Values', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.AttrRef', 'Test01c-MqttServer-Values.AttrRef', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.BooleanArray[4]', '1', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int8Array[4]', '22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int16Array[4]', '333', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int32Array[4]', '4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Int64Array[4]', '55555', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt8Array[4]', '66', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt16Array[4]', '777', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt32Array[4]', '8888', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.UInt64Array[4]', '99999', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float32Array[4]', '22.22', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.Float64Array[4]', '4444.4444', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.String80Array[4]', 'Why don\'t you have wings to fly with', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.TimeArray[4]', '15-MAY-2020 08:27:50.50', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.DeltaTimeArray[4]', '27 8:27:50.05', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.ObjidArray[4]', 'Test01c-MqttServer-Values', GDH__SUCCESS),
      ('Test01c-MqttServer-Values.AttrRefArray[4]', 'Test01c-MqttServer-Values.AttrRefArray[4]', GDH__SUCCESS)
            )
    client.subscribe("repl/sublist", 1);

    self.received = 0
    msg = '{"action":"sublist","reply":"repl/sublist","cycle":"1.0","duration":"100.0","attribute":['
    for i in range(len(d)):
        msg += '{' + str(i+1) + ',"' + d[i][0] + '"}'
        if i != len(d) - 1:
            msg += ','
    msg += ']}'
    client.publish('proviewr/server', msg)
    
    # Wait for reply
    for j in range (0, 3):
        client.loop_start()
        time.sleep(1)
        if self.received == 1:
          break
        client.loop_stop()
    if self.received == 0:
        self.logger.log('E', 'No response from server')
        return
    
    for i in range(len(d)):
        value = self.reply['a'][i]['value']
        if str(value)[:len(d[i][1])] != d[i][1]:
          self.logger.vlog('E', 'sublist, Unexpected value %s %s', str(i), str(value))
          return

    # Close subscription
    subref = self.reply['subref']
    msg = '{"action":"closesublist","subref":' + str(subref) + '}'
    client.publish('proviewr/server', msg)
          
    self.logger.log('S', 'sublist, successfull completion')



# Connect to MQTT on localhost
ctx = Ctx()
ctx.logger = pwrtest.logger('rt-mqtt_server', '$pwrp_log/mqtt_server.tlog')
client = mqtt.Client('MyClient')
client.username_pw_set('pwrp','pwrp')
client.on_message = on_message
client.connect('localhost')

ctx.get()
ctx.set()
ctx.subscribe()
ctx.sublist()

client.disconnect()
time.sleep(1)
