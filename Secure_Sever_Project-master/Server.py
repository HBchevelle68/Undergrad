from bcrypt import hashpw, gensalt
import AsymCrypto
import SymCrypto
import threading
import socket
import os.path
import cPickle
import base64


LOGO = r'''
            ______                     ____    __                     
           / ____/_  ______ __________/ / /   / /___ _____ ___  ____ _
          / / __/ / / / __ `/ ___/ __  / /   / / __ `/ __ `__ \/ __ `/
         / /_/ / /_/ / /_/ / /  / /_/ / /___/ / /_/ / / / / / / /_/ / 
         \____/\__,_/\__,_/_/   \__,_/_____/_/\__,_/_/ /_/ /_/\__,_/  
                                                                     
			'''


RSA_obj = 0


def key_Exchange(sock):
	#pull my RSA public key from secure storage
	print '[*] Sending RSA public key\n'
	pubKey = AsymCrypto.get_pubKeyRSA() 
	to_send=cPickle.dumps(pubKey) 
	sock.send(to_send)
	print pubKey

	#Test server public key successful transfer
	test_obj = AsymCrypto.get_keyRSA()
	test = sock.recv(1024)
	print test_obj.decrypt(test)

	#Begin receiving clients public RSA key
	print '[*] Receiving client public RSA key'
	s = sock.recv(1024)
	Client_RSApk = cPickle.loads(s) 
	print '[*] Client RSA Public Key => ', Client_RSApk

	#Test clients public key successful transfer
	test1 = Client_RSApk.encrypt("this is a test for client", '')
	sock.sendall(test1[0])

	#generate AES key (unique per session)
	#generate AES Cipher object
	Session_AESkey = SymCrypto.generateKey()
	print len(Session_AESkey) 
	AES_Cipher = SymCrypto.generate_AES_Cipher(Session_AESkey) 
	print AES_Cipher
	
	#ENCRYPT
	#ENCODE
	ecryptd_session_key = Client_RSApk.encrypt(Session_AESkey, '') 
	encoded =  base64.b64encode(ecryptd_session_key[0])
	sock.sendall(encoded)

	encry = sock.recv(1024)
	decry = SymCrypto.decodeAES(AES_Cipher, encry)
	print decry
	return AES_Cipher

def handle_login(sock, addr):
	print 'login'

def handle_new_acct(sock, addr, Session_AES):
	print 'creating account'
	taken_flag = '0'
	while taken_flag == '0': 
		encr_dusr = sock.recv(1024)
		desired_user = SymCrypto.decodeAES(Session_AES, encr_dusr)
		print '%s ==DECRYPTED==> %s' % (encr_dusr,desired_user)
		fileSearch = desired_user + '.txt'
		print fileSearch
		if not os.path.isfile(fileSearch):
			taken_flag = '1'
		else:
			taken_flag = '0'
		sock.sendall(taken_flag)



	sock.close()

def handle_check_in(sock, addr, LOGO):
	print '[*] Got connection from: ', addr
	Session_AES = key_Exchange(sock)

	sock.sendall(LOGO)
	opt = sock.recv(4)
	print opt
	if opt == '1':
		handle_login(sock, addr, Session_AES)
	elif opt == '2':
		handle_new_acct(sock, addr, Session_AES)
	else:
		sock.close()

#########################################################
#														#
#						BEGIN MAIN 						#
#														#
#########################################################

if __name__ == '__main__':
	
	print LOGO

	global RSA_obj
	RSA_obj = AsymCrypto.RSA_init() 
	print RSA_obj
	print "[+] Guard Llama server is up"

	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	PORT = 1778
	HOST = '192.168.2.113'
	sock.bind((HOST, PORT))
	sock.listen(5)
	while True:
	   connection, addr = sock.accept()
	   pyth_thread = threading.Thread(target=handle_check_in, args=(connection, addr, LOGO))
	   pyth_thread.start()


