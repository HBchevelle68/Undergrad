from bcrypt import hashpw, gensalt
import SymCrypto
import AsymCrypto
import socket
import sys
import os
import cPickle
import base64

LOGO = ''
Server_RSA = 0
My_RSA = 0
session_key = 0
session_cipher = 0




def Key_exchange(sock):	
	try:
		global Server_RSA
		global session_key
		global session_cipher
		print '[*] Receiving server public RSA key'
		s = sock.recv(1024)
		Server_RSA = cPickle.loads(s) #de-serialize RSA public key from server
		print '[*] Servers RSA Public Key => ', Server_RSA

		#testing purposes, verifying public key works with server
		test1 = Server_RSA.encrypt("this is a test", 32)
		sock.sendall(test1[0])

		my_pubKey = AsymCrypto.get_pubKeyRSA() #pull my RSA public key from secure storage
		to_send=cPickle.dumps(my_pubKey) #SEND RSA PUBLIC KEY
		sock.send(to_send)

		test = sock.recv(1024)
		print My_RSA.decrypt(test)

		temp_encr = sock.recv(1024)
		print len(temp_encr)
		decoded = base64.b64decode(temp_encr) #DECODE
		print len(decoded)
		session_key = My_RSA.decrypt(decoded) #DECRYPT
		print 'AES key len: ', len(session_key)
		print 'AES key size in bytes: ',sys.getsizeof(session_key)
		session_cipher = SymCrypto.generate_AES_Cipher(session_key) 
		print session_cipher

		#test_msg = 'hello world'
		encr = SymCrypto.encodeAES(session_cipher, 'hello world')
		sock.sendall(encr)

	except KeyboardInterrupt:
		secure_wipe()
		sys.exit()


def create_acct(sock):
	free = '0'
	match = 0
	while free == '0':
		username = raw_input('Username: ')
		print username
		usr_encr = SymCrypto.encodeAES(session_cipher, username)
		print usr_encr
		print len(usr_encr)
		sock.sendall(usr_encr)
		free = sock.recv(1)
		if free == '0': print 'Username not available\n'
	print 'username is free\n'
	while match == 0:
		password1 = raw_input('Password: ')
		password2 = raw_input('Retype Password: ')
		if password1 == password2:
			match = 1
		else:
			print 'Passwords do not match!!'

	sock.close()
	

def secure_wipe():
	print '\n[*] SECURE DESTROY IN PROGRESS'
	global Server_RSA
	global My_RSA
	global session_key
	global session_cipher
	srsa = sys.getsizeof(Server_RSA)
	mrsa = sys.getsizeof(My_RSA)
	sesK = sys.getsizeof(session_key)
	sesC = sys.getsizeof(session_cipher)
	Server_RSA = bytearray(os.urandom(sys.getsizeof(srsa)))
	My_RSA = bytearray(os.urandom(sys.getsizeof(mrsa)))
	session_key = bytearray(os.urandom(sys.getsizeof(sesK)))
	session_cipher = bytearray(os.urandom(sys.getsizeof(sesC)))
	print '[*] COMPLETE!'


def display_options():

	try:
		while 1:
			raw = raw_input('[1] Login\n[2] Create Account\n') 
			if raw == '1' or raw == '2':
				return raw
	except KeyboardInterrupt:
		secure_wipe()
		sys.exit(1)

	

####################################################################

if __name__ == '__main__':
	try:
		global My_RSA 
		My_RSA = AsymCrypto.RSA_init()
		print My_RSA

		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		PORT = 1778
		HOST = '192.168.2.113'
		sock.connect((HOST, PORT))

		Key_exchange(sock)


		LOGO = sock.recv(1024)
		print LOGO
		opt = display_options()
		sock.sendall(opt)
		if opt == '1':
			pass
		elif opt == '2':
			create_acct(sock)
		else:
			sock.close()

	except KeyboardInterrupt:
		secure_wipe()
		sys.exit()
	
