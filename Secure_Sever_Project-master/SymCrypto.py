from Crypto.Cipher import AES
from Crypto import Random
import base64
import os

# the block size for the cipher object; must be 16, 24, or 32 for AES
BLOCK_SIZE = 32
PADDING = '{'

# pad the text to be encrypted
pad = lambda s: s + (BLOCK_SIZE - len(s) % BLOCK_SIZE) * PADDING

# one-liners to encrypt/encode and decrypt/decode a string
# encrypt with AES, encode with base64
def encodeAES(cipher, msg):
	return base64.b64encode(cipher.encrypt(pad(msg)))

def decodeAES(cipher, encodedMSG):
	return cipher.decrypt(base64.b64decode(encodedMSG)).rstrip(PADDING)
#	
#generate secret key with GLOBAL block size	
def generateKey():
	return os.urandom(BLOCK_SIZE)

#generate AES object with passed in secret key
def generate_AES_Cipher(key):
	return AES.new(key)


#######################################################
#													  #
#					BEGIN EXAMPLE USAGES			  #
#													  #
#######################################################

# generate a random secret key
#secret = generateKey()

#create a cipher object using the random secret
#cipher = generate_AES_Cipher(generateKey())

# encode a string
#encoded = encodeAES(cipher, 'password')
#print 'Encrypted string:', encoded

# decode the encoded string
#decoded = decodeAES(cipher, encoded)
#print 'Decrypted string:', decoded
