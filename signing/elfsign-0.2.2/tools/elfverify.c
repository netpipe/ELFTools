/*
 * elfverify: Validates ELF binary signatures that were generated with elfsign.
 *
 * skape
 * mmiller@hick.org
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "openssl/err.h"
#include "openssl/x509.h"
#include "openssl/pkcs7.h"

#include "verify.h"

#include "../elfsign.h"

unsigned long callback(void *userParam, X509 *cert, unsigned long trustedSigner);
const char *getTrustString(unsigned long ret);

int main(int argc, char **argv)
{
	char *elfFile = NULL, *caFile = "none", *caPath = "none";
	extern char *optarg;
	int c, ret = 0;

   CRYPTO_malloc_init();
   ERR_load_crypto_strings();
   OpenSSL_add_all_algorithms();

	while ((c = getopt(argc, argv, "f:c:p:h")) != EOF)
	{
		switch (c)
		{
			case 'f':
				elfFile = optarg;
				break;
			case 'c':
				caFile = optarg;
				break;
			case 'p':
				caPath = optarg;
				break;
			case 'h':
				fprintf(stdout, 
					"\n"
					"elfverify -- verify the signing of an elf image\n"
					"\n"
					"Usage: ./elfverify [-f elf image] [-c ca cert file] [-p ca cert path] [-h]\n"
					"\n");
				return 0;
			case 'v':
				fprintf(stdout, 
					"elfverify %s %s\n"
					"   skape - mmiller@hick.org\n"
					"   nologin (http://www.nologin.org)\n",
					ELFSIGN_VERSION,
					ELFSIGN_RELDATE);
				return 0;
			default:
				break;
		}
	}

	
	// If the ELF image is invalid, no sense in validating it.
	if (!elfFile)
	{
		fprintf(stdout, "error: no elf image specified.\n");
		return 0;
	}

	// Call the ELF verify subsystem with a custom callback
	if ((ret = elfverifyTrustCallback(elfFile, caFile, caPath, callback, NULL)) == ELFVERIFY_SUCCESS)
		fprintf(stdout, "OK\n"), ret = 1;
	else
	{
		fprintf(stdout, "FAIL (%s)\n", getTrustString(ret)), ret = 0;
	}

	return ret;
}

/*
 * Convert an error code to an error string as returned from the
 * ELF verification subsystem
 */
const char *getTrustString(unsigned long ret)
{
	switch (ret)
	{
		case ELFVERIFY_NOT_SIGNED:
			return "The binary is not signed.";
			break;
		case ELFVERIFY_CERT_UNTRUSTED:
			return "The signer certificate is not trusted.";
			break;
		case ELFVERIFY_INVALID_DIGEST:
			return "The binary digest did not match the signed digest.";
			break;
		default:
			return "Unknown error.";
			break;
	}
}

/*
 * Callback for un-trusted issuers that allows the user to specify whether
 * or not the image should be trusted.
 */
unsigned long callback(void *userParam, X509 *cert, 
		unsigned long trustedIssuer)
{
	unsigned long ret = 0;

	if (!trustedIssuer)
	{
		X509_NAME *issuer  = X509_get_issuer_name(cert);
		X509_NAME *subject = X509_get_subject_name(cert);
		BIO *bio_stdout = BIO_new(BIO_s_file());
		char resp[32];

		BIO_set_fp(bio_stdout, stdout, BIO_NOCLOSE);

		fprintf(stdout, "Issuer: ");
		X509_NAME_print(bio_stdout, issuer, 0);
		fprintf(stdout, "\n");
	
		fprintf(stdout, "Signer: ");
		X509_NAME_print(bio_stdout, subject, 0);
		fprintf(stdout, "\n");
	
		fprintf(stdout, "Issuer is not trusted, would you like to trust them? [y/N] ");
		fflush(stdout);

		fgets(resp, sizeof(resp) - 1, stdin);

		if (resp[0] == 'Y' || resp[0] == 'y')
			ret = 1;

		BIO_free(bio_stdout);
	}
	else
		ret = 1;

	return ret;
}
