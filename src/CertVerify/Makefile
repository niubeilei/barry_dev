KERNELHOME = /usr/src/linux

CERT_VERIFY_SRC_DIR = $(AOSSRC)/CertVerify

CERT_VERIFY_DEST_DIR = $(KERNELHOME)/net/CertVerify

CREATE_DIR = (if [ -d $(CERT_VERIFY_DEST_DIR) ]; then echo ""; else mkdir $(CERT_VERIFY_DEST_DIR); fi)

install:
	$(CREATE_DIR)
	ln -s -f $(CERT_VERIFY_SRC_DIR)/Makefile-CertVerify $(CERT_VERIFY_DEST_DIR)/Makefile
	ln -s -f $(CERT_VERIFY_SRC_DIR)/aosAccessManage.h	$(CERT_VERIFY_DEST_DIR)/aosAccessManage.h
	ln -s -f $(CERT_VERIFY_SRC_DIR)/aosCertVerify.h		$(CERT_VERIFY_DEST_DIR)/aosCertVerify.h
	ln -s -f $(CERT_VERIFY_SRC_DIR)/aostcpapi.h			$(CERT_VERIFY_DEST_DIR)/aostcpapi.h
	ln -s -f $(CERT_VERIFY_SRC_DIR)/asn1.h				$(CERT_VERIFY_DEST_DIR)/asn1.h
	ln -s -f $(CERT_VERIFY_SRC_DIR)/cert_verify.h		$(CERT_VERIFY_DEST_DIR)/cert_verify.h
	ln -s -f $(CERT_VERIFY_SRC_DIR)/common.h			$(CERT_VERIFY_DEST_DIR)/common.h
	ln -s -f $(CERT_VERIFY_SRC_DIR)/crypt_hash.h		$(CERT_VERIFY_DEST_DIR)/crypt_hash.h
	ln -s -f $(CERT_VERIFY_SRC_DIR)/_list.h				$(CERT_VERIFY_DEST_DIR)/_list.h
	ln -s -f $(CERT_VERIFY_SRC_DIR)/x509.h				$(CERT_VERIFY_DEST_DIR)/x509.h
	ln -s -f $(CERT_VERIFY_SRC_DIR)/x509_object.h		$(CERT_VERIFY_DEST_DIR)/x509_object.h
	ln -s -f $(CERT_VERIFY_SRC_DIR)/x509_util.h			$(CERT_VERIFY_DEST_DIR)/x509_util.h
	ln -s -f $(CERT_VERIFY_SRC_DIR)/jns_ocsp.h			$(CERT_VERIFY_DEST_DIR)/jns_ocsp.h

	ln -s -f $(CERT_VERIFY_SRC_DIR)/access_manage.c		$(CERT_VERIFY_DEST_DIR)/access_manage.c
	ln -s -f $(CERT_VERIFY_SRC_DIR)/asn1_decode.c		$(CERT_VERIFY_DEST_DIR)/asn1_decode.c
	ln -s -f $(CERT_VERIFY_SRC_DIR)/asn1_encode.c		$(CERT_VERIFY_DEST_DIR)/asn1_encode.c
	ln -s -f $(CERT_VERIFY_SRC_DIR)/cert_verify.c		$(CERT_VERIFY_DEST_DIR)/cert_verify.c
	ln -s -f $(CERT_VERIFY_SRC_DIR)/md5.c				$(CERT_VERIFY_DEST_DIR)/md5.c
	ln -s -f $(CERT_VERIFY_SRC_DIR)/sha1.c				$(CERT_VERIFY_DEST_DIR)/sha1.c
	ln -s -f $(CERT_VERIFY_SRC_DIR)/x509.c				$(CERT_VERIFY_DEST_DIR)/x509.c
	ln -s -f $(CERT_VERIFY_SRC_DIR)/x509_object.c		$(CERT_VERIFY_DEST_DIR)/x509_object.c
	ln -s -f $(CERT_VERIFY_SRC_DIR)/x509_util.c			$(CERT_VERIFY_DEST_DIR)/x509_util.c
	ln -s -f $(CERT_VERIFY_SRC_DIR)/jns_ocsp.c			$(CERT_VERIFY_DEST_DIR)/jns_ocsp.c

uninstall:
	rm -r $(CERT_VERIFY_DEST_DIR)/aosAccessManage.h
	rm -r $(CERT_VERIFY_DEST_DIR)/aosCertVerify.h
	rm -r $(CERT_VERIFY_DEST_DIR)/aostcpapi.h
	rm -r $(CERT_VERIFY_DEST_DIR)/asn1.h
	rm -r $(CERT_VERIFY_DEST_DIR)/cert_verify.h
	rm -r $(CERT_VERIFY_DEST_DIR)/common.h
	rm -r $(CERT_VERIFY_DEST_DIR)/crypt_hash.h
	rm -r $(CERT_VERIFY_DEST_DIR)/_list.h
	rm -r $(CERT_VERIFY_DEST_DIR)/x509.h
	rm -r $(CERT_VERIFY_DEST_DIR)/x509_object.h
	rm -r $(CERT_VERIFY_DEST_DIR)/x509_util.h
	rm -r $(CERT_VERIFY_DEST_DIR)/jns_ocsp.h

	rm -r $(CERT_VERIFY_DEST_DIR)/access_manage.c
	rm -r $(CERT_VERIFY_DEST_DIR)/asn1_decode.c
	rm -r $(CERT_VERIFY_DEST_DIR)/asn1_encode.c
	rm -r $(CERT_VERIFY_DEST_DIR)/cert_verify.c
	rm -r $(CERT_VERIFY_DEST_DIR)/md5.c
	rm -r $(CERT_VERIFY_DEST_DIR)/sha1.c
	rm -r $(CERT_VERIFY_DEST_DIR)/x509.c
	rm -r $(CERT_VERIFY_DEST_DIR)/x509_object.c
	rm -r $(CERT_VERIFY_DEST_DIR)/x509_util.c
	rm -r $(CERT_VERIFY_DEST_DIR)/jns_ocsp.c



