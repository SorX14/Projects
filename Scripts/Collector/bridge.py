#!/usr/bin/env python

import sys
import os
import time
import string
import threading
import socket
import codecs
import serial
import httplib

try:
    True
except NameError:
    True = 1
    False = 0

class SerRead:
	def __init__(self, serial_instance):
		self.serial = serial_instance
		self._write_lock = threading.Lock()
		
		self.startReader()
		
	def startReader(self):
		self.thread_read = threading.Thread(target = self.reader)
		self.thread_read.setDaemon(True)
		self.thread_read.setName('serial->socket')
		self.thread_read.start()
		
	def setConnection(self, socket):
		self.socket = socket
					
	def reader(self):
		global connection
		self.sendHTTP('1', 'starting_reader');
		while True:
			try:
				data = self.serial.read(1)
				if data:
					data = data + self.serial.readline(1024)
					self._write_lock.acquire()
					try:
						string_items = data.rstrip().split('|');
						# Example string
						# ========================
						# 8|temp=23.2&relay=ON
						
						sys.stderr.write('|------ [RX] TTY: ' + data.rstrip() + '\n')
						
						if (len(string_items) > 1): # Make sure we have a second item before trying to reference it
							if string_items[0] != 'SELF':
								self.sendHTTP(string_items[0], string_items[1]) # Send nodeID, data
						
					finally:
						self._write_lock.release()
			except socket.error, msg:
				sys.stderr.write('|---- [ERR] %s\n' % msg)
				break
				
	# Send the data to the PHP script
	def sendHTTP(self, node, data):
		global running
		try:
			conn = httplib.HTTPConnection('127.0.0.1')
			conn.request('GET', '/rf12.php?node=' + node + '&' + data)
			sys.stderr.write('|------ HTTP: node=' + node + '&' + data + '\n')
			conn.close()		
		except (httplib.HTTPException, socket.error) as ex:
			sys.stderr.write("|-- [ERR] Failed to send HTTP" + '\n')
			running = False
			sys.exit(0)

connection = False
running = True
		
if __name__ == '__main__':
	ser = serial.Serial();
	#ser.port = '/dev/ttyJeeNode'
	ser.port = '/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AM01Z7UF-if00-port0'
	ser.baudrate = 115200
	ser.parity = 'N'
	ser.rtscts = False
	ser.xonxoff = False
	ser.timeout = 1
	
	try:
		ser.open()
	except serial.SerialException, e:
		sys.stderr.write("|-- [ERR] Could not open serial port %s: %s\n" % (ser.portstr, e))
		sys.exit(1)
		
		
	# Bind to localhost:7777
	srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	srv.bind(('127.0.0.1', 7777))
	srv.listen(1)
	sys.stderr.write("|-- Listening on port 7777\n");
	sys.stderr.write("|-- [START] %s %s,%s,%s,%s \n" % (ser.portstr, ser.baudrate, 8, ser.parity, 1))
	
	SerRead(ser)
	while True:
		try:			
			if running is False:
				sys.stderr.write('|------ [ERR] Serial capture thread not running!'+'\n')
				sys.exit(0)
				
			# Block and wait for someone to connect
			sys.stderr.write("|---- [WAIT] On port %s...\n" % 7777)
			connection, addr = srv.accept()
			sys.stderr.write('|---- [CONN] %s\n' % (addr,))
			while True:
				try:
					data = connection.recv(1024)
					if not data:
						break
					sys.stderr.write('|------ [TX] TTY: ' + data.rstrip() + '\n')
					ser.write(data)
				except socket.error, msg:
					sys.stderr.write('|-- [ERR] %s\n' % msg)
					break
						
			# If we're here, it means the connection has dropped
			connection.close()
			connection = False
			sys.stderr.write('|---- [DISC] %s\n' % (addr,))
			
			# Looks like we're looping again
		except KeyboardInterrupt:
			break;
		except socket.error, msg:
			sys.stderr.write('|-- [ERR] %s\n' % msg)
			sys.exit(0)
		except:
			break
	
	sys.stderr.write('|-- [EXIT]\n')
