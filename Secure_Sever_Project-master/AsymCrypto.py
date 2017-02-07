from Crypto.PublicKey import RSA
from Crypto import Random
from Crypto.Hash import SHA256
import sys
import os.path
import cPickle 


#stores RSA object - password protected
def save_keyRSA(keypair):
	f = open('myRSA.pem', 'w')
	f.write(keypair.exportKey('PEM', 'test_phrase', pkcs=1))
	f.close()

#retrieve RSA object from file
#Password protected - will throw exception if password fails
def get_keyRSA():
	f = open('myRSA.pem', 'r')
	key = RSA.importKey(f.read(), 'test_phrase')
	return key

def get_pubKeyRSA():
	f = open('myRSA.pem', 'r')
	key = RSA.importKey(f.read(), 'test_phrase')
	return key.publickey

def RSA_init():
	if not os.path.isfile('myRSA.pem'):
		print 'RSA secure store file not found...generating RSA\n'
		# RSA config
		KEY_LENGTH = 2048 
		random_gen = Random.new().read

		#RSA keypair
		RSA_obj = RSA.generate(KEY_LENGTH, random_gen)
		save_keyRSA(RSA_obj)
		return RSA_obj
	else:
		print '[+] RSA secure store file found\n[+] Pulling RSA object\n'
		RSA_obj = get_keyRSA()

		return RSA_obj
