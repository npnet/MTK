"""
ECDSA key management
"""

from abc import ABC, abstractmethod
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.hashes import SHA256
from cryptography.hazmat.primitives.hashes import SHA384
from cryptography.hazmat.primitives.hashes import SHA512

from .general import KeyClass


class ECDSAUsageError(Exception):
    pass


class ECDSAPublic(KeyClass):
    def __init__(self, key):
        self.key = key

    def _unsupported(self, name):
        raise ECDSAUsageError("Operation {} requires private key".format(name))

    def _get_public(self):
        return self.key

    def get_public_bytes(self):
        # The key is embedded into MBUboot in "SubjectPublicKeyInfo" format
        return self._get_public().public_bytes(
            encoding=serialization.Encoding.DER,
            format=serialization.PublicFormat.SubjectPublicKeyInfo)

    def get_private_bytes(self, minimal):
        self._unsupported('get_private_bytes')

    def export_private(self, path, passwd=None):
        self._unsupported('export_private')

    def export_public(self, path):
        """Write the public key to the given file."""
        pem = self._get_public().public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.SubjectPublicKeyInfo)
        with open(path, 'wb') as f:
            f.write(pem)

    @abstractmethod
    def shortname(self):
        pass

    @abstractmethod
    def sig_type(self):
        pass

    @abstractmethod
    def sig_tlv(self):
        pass

    @abstractmethod
    def sig_len(self):
        pass

    def verify(self, signature, payload, sig_algo):
        k = self.key
        if isinstance(self.key, ec.EllipticCurvePrivateKey):
            k = self.key.public_key()
        return k.verify(signature=signature, data=payload,
                        signature_algorithm=sig_algo)
#                       signature_algorithm=ec.ECDSA(SHA256()))


class ECDSAPriv(ECDSAPublic):
    """
    Wrapper around an ECDSA private key.
    """

    def __init__(self, key):
        """key should be an instance of EllipticCurvePrivateKey"""
        self.key = key

#   @staticmethod
    def generate(self, key_algo):
        pass
#       pk = ec.generate_private_key(
#               key_algo,
#               ec.SECP256R1(),
#               backend=default_backend())
#       return ECDSA256P1(pk)

    def _get_public(self):
        return self.key.public_key()

    def _build_minimal_ecdsa_privkey(self, der):
        pass

    def get_private_bytes(self, minimal):
        priv = self.key.private_bytes(
            encoding=serialization.Encoding.DER,
            format=serialization.PrivateFormat.PKCS8,
            encryption_algorithm=serialization.NoEncryption())
        if minimal:
            priv = self._build_minimal_ecdsa_privkey(priv)
        return priv

    def export_private(self, path, passwd=None):
        """Write the private key to the given file, protecting it with the optional password."""
        if passwd is None:
            enc = serialization.NoEncryption()
        else:
            enc = serialization.BestAvailableEncryption(passwd)
        pem = self.key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.PKCS8,
            encryption_algorithm=enc)
        with open(path, 'wb') as f:
            f.write(pem)

    def raw_sign(self, payload):
        pass

    def sign(self, payload):
        # To make fixed length, pad with one or two zeros.
        sig = self.raw_sign(payload)
        sig += b'\000' * (self.sig_len() - len(sig))
        return sig


class ECDSA256P1Public(ECDSAPublic):
    def shortname(self):
        return "ecdsa256"

    def sig_type(self):
        return "ECDSA256_SHA256"

    def sig_tlv(self):
        return "ECDSA256"

    def sig_len(self):
        # The DER encoding depends on the high bit, and can be
        # anywhere from 70 to 72 bytes.  Because we have to fill in
        # the length field before computing the signature, however,
        # we'll give the largest, and the sig checking code will allow
        # for it to be up to two bytes larger than the actual
        # signature.
        return 72

    def verify(self, signature, payload):
        super().verify(self, signature, payload, ec.ECDSA(SHA256()))


class ECDSA256P1(ECDSAPriv, ECDSA256P1Public):
    """
    Wrapper around an ECDSA256 private key.
    """

    @staticmethod
    def generate():
        pk = ec.generate_private_key(
            ec.SECP256R1(),
            backend=default_backend())
        return ECDSA256P1(pk)

    def _build_minimal_ecdsa_privkey(self, der):
        '''
        Builds a new DER that only includes the EC private key, removing the
        public key that is added as an "optional" BITSTRING.
        '''
        offset_PUB = 68
        EXCEPTION_TEXT = "Error parsing ecdsa key. Please submit an issue!"
        if der[offset_PUB] != 0xa1:
            raise ECDSAUsageError(EXCEPTION_TEXT)
        len_PUB = der[offset_PUB + 1]
        b = bytearray(der[:-offset_PUB])
        offset_SEQ = 29
        if b[offset_SEQ] != 0x30:
            raise ECDSAUsageError(EXCEPTION_TEXT)
        b[offset_SEQ + 1] -= len_PUB
        offset_OCT_STR = 27
        if b[offset_OCT_STR] != 0x04:
            raise ECDSAUsageError(EXCEPTION_TEXT)
        b[offset_OCT_STR + 1] -= len_PUB
        if b[0] != 0x30 or b[1] != 0x81:
            raise ECDSAUsageError(EXCEPTION_TEXT)
        b[2] -= len_PUB
        return b

    def raw_sign(self, payload):
        """Return the actual signature"""
        return self.key.sign(
            data=payload,
            signature_algorithm=ec.ECDSA(SHA256()))


class ECDSA384R1Public(ECDSAPublic):
    def shortname(self):
        return "ecdsa384"

    def sig_type(self):
        return "ECDSA384_SHA384"

    def sig_tlv(self):
        return "ECDSA384"

    def sig_len(self):
        # The DER encoding depends on the high bit, and can be
        # anywhere from 102 to 104 bytes. Because we have to fill in
        # the length field before computing the signature, however,
        # we'll give the largest, and the sig checking code will allow
        # for it to be up to two bytes larger than the actual
        # signature.
        return 104

    def verify(self, signature, payload):
        super().verify(self, signature, payload, ec.ECDSA(SHA384()))


class ECDSA384R1(ECDSAPriv, ECDSA384R1Public):
    """
    Wrapper around an ECDSA384 private key.
    """

    @staticmethod
    def generate():
        pk = ec.generate_private_key(
            ec.SECP384R1(),
            backend=default_backend())
        return ECDSA384R1(pk)

    def _build_minimal_ecdsa_privkey(self, der):
        '''
        Builds a new DER that only includes the EC private key, removing the
        public key that is added as an "optional" BITSTRING.
        '''
        raise NotImplementedError

    def raw_sign(self, payload):
        """Return the actual signature"""
        return self.key.sign(
            data=payload,
            signature_algorithm=ec.ECDSA(SHA384()))


class ECDSA521R1Public(ECDSAPublic):
    def shortname(self):
        return "ecdsa521"

    def sig_type(self):
        return "ECDSA521_SHA512"

    def sig_tlv(self):
        return "ECDSA521"

    def sig_len(self):
        # The DER encoding depends on the high bit, and can be
        # anywhere from 102 to 104 bytes. Because we have to fill in
        # the length field before computing the signature, however,
        # we'll give the largest, and the sig checking code will allow
        # for it to be up to two bytes larger than the actual
        # signature.
        return 140

    def verify(self, signature, payload):
        super().verify(self, signature, payload, ec.ECDSA(SHA512()))


class ECDSA521R1(ECDSAPriv, ECDSA521R1Public):
    """
    Wrapper around an ECDSA521 private key.
    """

    @staticmethod
    def generate():
        pk = ec.generate_private_key(
            ec.SECP521R1(),
            backend=default_backend())
        return ECDSA521R1(pk)

    def _build_minimal_ecdsa_privkey(self, der):
        '''
        Builds a new DER that only includes the EC private key, removing the
        public key that is added as an "optional" BITSTRING.
        '''
        raise NotImplementedError

    def raw_sign(self, payload):
        """Return the actual signature"""
        return self.key.sign(
            data=payload,
            signature_algorithm=ec.ECDSA(SHA512()))
