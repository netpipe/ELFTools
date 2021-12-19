/*
 * elfsign: ELF binary signature generation tool
 *
 * skape
 * mmiller@hick.org
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "openssl/objects.h"
#include "openssl/rsa.h"

#include "openssl/err.h"
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/bio.h"

#include "melf.h"

#include "../lib/verify.h"
#include "../elfsign.h"

typedef struct _elfsign_ctx {

	MELF       *melf;
	char       digest[16];

	// Files
	const char *elfFile;
	const char *certificateFile;
	const char *privateKeyFile;

	// OpenSSL
	X509       *certificate;
	EVP_PKEY   *privateKey;

} ELFSIGN_CTX;

unsigned long elfsignImageOpen(ELFSIGN_CTX *ctx);
unsigned long elfsignImageSetSigSection(ELFSIGN_CTX *ctx);
unsigned long elfsignImageSave(ELFSIGN_CTX *ctx, const char *path);
unsigned long elfsignImageClose(ELFSIGN_CTX *ctx);

X509 *loadCertificate(const char *path);
EVP_PKEY *loadKey(const char *path);
int passwordPrompt(char *buf, int bufSize, int verify, void *str);

int main(int argc, char **argv)
{
	extern char *optarg;
	ELFSIGN_CTX ctx;
	int c;

	// Initialize OpenSSL
   CRYPTO_malloc_init();
   ERR_load_crypto_strings();
   OpenSSL_add_all_algorithms();

	memset(&ctx, 0, sizeof(ctx));

	while ((c = getopt(argc, argv, "f:c:p:hv")) != EOF)
	{
		switch (c)
		{
			case 'f':
				ctx.elfFile = optarg;
				break;
			case 'c':
				ctx.certificateFile = optarg;
				break;
			case 'p':
				ctx.privateKeyFile = optarg;
				break;
			case 'h':
				fprintf(stderr, 
					"\n"
					"elfsign -- sign elf images\n"
					"\n"
					"Usage: ./elfsign [-f elf image] [-c certificate file] [-p private key file] [-h]\n"
					"\n");
				return 0;
			case 'v':
				fprintf(stderr, 
					"elfsign %s %s\n"
					"   skape - mmiller@hick.org\n"
					"   nologin (http://www.nologin.org)\n",
					ELFSIGN_VERSION, 
					ELFSIGN_RELDATE);
				return 0;
			default:
				break;
		}
	}

	do
	{
		// Check to make sure the supplied ELF file is valid
		if (!ctx.elfFile)
		{
			fprintf(stderr, "error: no elf image specified.\n");
			break;
		}
	
		// Check to make sure the supplied certificate is valid
		if (!ctx.certificateFile)
		{
			fprintf(stderr, "error: no certificate specified.\n");
			break;
		}
	
		// Check to make sure the supplied private key is valid
		if (!ctx.privateKeyFile)
		{
			fprintf(stderr, "error: no private key specified.\n");
			break;
		}

		// Try to open the ELF image & load the cert/private key
		if (!elfsignImageOpen(&ctx))
			break;

		// Calculate the checksum and save the signature
		if (!elfsignImageSetSigSection(&ctx))
		{
			fprintf(stderr, "error: failed to sign elf image '%s'.\n", ctx.elfFile);
			break;
		}

		// Save the binary to disk
		if (!elfsignImageSave(&ctx, ctx.elfFile))
		{
			fprintf(stderr, "error: failed to save elf image '%s'.\n", ctx.elfFile);
			break;
		}

		// Cleanup and close the image
		elfsignImageClose(&ctx);

	} while (0);

	return 1;
}

/*
 * Opens the ELF image & the certificate/private key
 */
unsigned long elfsignImageOpen(ELFSIGN_CTX *ctx)
{
	ctx->melf = melf_open(ctx->elfFile, MELF_ACCESS_ALL);

	if (!ctx->melf)
	{
		fprintf(stderr, "error: could not open elf image '%s'.\n", ctx->elfFile);
		return 0;
	}

	// Load the certificate
	ctx->certificate = loadCertificate(ctx->certificateFile);

	// Load the private key
	ctx->privateKey  = loadKey(ctx->privateKeyFile);

	if (!ctx->certificate || !ctx->privateKey)
	{
		fprintf(stderr, "error: invalid certificate or private key "
		                "(did you enter the right passphrase?)\n");
		return 0;
	}

	return 1;
}

/*
 * Calculates the checksum of the image and adds a signature section
 * if one does not exist.  This section holds the signature and the 
 * version scheme of the checksum algorithm used to generate it.
 */
unsigned long elfsignImageSetSigSection(ELFSIGN_CTX *ctx)
{
	unsigned char sigVer[ELF_SIGNATURE_VERSION_SIZE];
	ELF_SPEC_HEADER *sigHeader = NULL;
	unsigned long ret = 0, bufLength = 0;
	BIO *sigRaw = BIO_new(BIO_s_mem()), *digestRaw = NULL;
	unsigned char *buf = NULL, *tbuf = NULL;
	PKCS7 *sig7 = NULL;

	elfverifyInitSignatureVersion(sigVer);

	do
	{
		sigHeader = melf_sectionFindName(ctx->melf, ".sig");
	
		// Create the signature section before doing the checksums as we 
		// potentially modify the string table for sections.
		if (!sigHeader)
			sigHeader = melf_sectionAdd(ctx->melf);

		// If the signature header still isn't valid, we bomb.
		if (!sigHeader)
			break;

		// Initialize the section
		melf_sectionSetType(ctx->melf, sigHeader, SHT_PROGBITS);
		melf_sectionSetAddress(ctx->melf, sigHeader, 0);
		melf_sectionSetFlags(ctx->melf, sigHeader, 0);
		melf_sectionSetName(ctx->melf, sigHeader, ".sig");
		melf_sectionSetContent(ctx->melf, sigHeader, NULL, 0);

		// Synchronize the sections so that the checksum will be valid
		melf_synchronize(ctx->melf);

		// Calculate the image's checksum
		if (!elfverifyCalculateImageChecksum(ctx->melf,
				ctx->digest, NULL))
			break;

		// Create a bio with the digest buffer for use with signing
		if (!(digestRaw = BIO_new_mem_buf(ctx->digest, sizeof(ctx->digest))))
			break;

		// Sign the raw digest with the supplied ceritifcate/private key
		sig7 = PKCS7_sign(ctx->certificate, ctx->privateKey, NULL, digestRaw,
				PKCS7_TEXT);

		// If the signature is invalid, that sucks.
		if (!sig7)
			break;

		if (!PEM_write_bio_PKCS7(sigRaw, sig7))
			break;

		// Get the raw buffer & length
		bufLength = BIO_get_mem_data(sigRaw, &buf);

		// Allocate storage for version + signature
		if (!(tbuf = (unsigned char *)malloc(
				bufLength + ELF_SIGNATURE_VERSION_SIZE)))
			break;

		// Initialize the all-encompassing buffer
		memcpy(tbuf, sigVer, ELF_SIGNATURE_VERSION_SIZE);
		memcpy(tbuf + ELF_SIGNATURE_VERSION_SIZE, buf, bufLength);

		// Set the section's content to the generated signature, including
		// the version scheme of elfsign used to generate the signature
		// for forwards/backwards compat
		melf_sectionSetContent(ctx->melf, sigHeader, tbuf, 
				bufLength + ELF_SIGNATURE_VERSION_SIZE);

		// Win win
		ret = 1;

	} while (0);

	// Cleanup buffers used during signing
	if (tbuf)
		free(tbuf);
	if (sigRaw)
		BIO_free(sigRaw);
	if (digestRaw)
		BIO_free(digestRaw);
	if (sig7)
		PKCS7_free(sig7);

	return ret;
}

/*
 * Synchronizes the signed image to disk
 */
unsigned long elfsignImageSave(ELFSIGN_CTX *ctx, const char *path)
{
	melf_save(ctx->melf, path);

	return 1;
}

/*
 * Closes the ELF image
 */
unsigned long elfsignImageClose(ELFSIGN_CTX *ctx)
{
	if (ctx->melf)
		melf_destroy(ctx->melf);

	ctx->melf = NULL;

	return 1;
}

/*
 * Load the certificate, optionally handling a password prompt if necessary
 */
X509 *loadCertificate(const char *path)
{
	X509 *ret = NULL;
	BIO *bio  = BIO_new_file(path, "r");

	do
	{
		if (!bio)
			break;
	
		ret = PEM_read_bio_X509_AUX(bio, NULL, passwordPrompt, 
				"Certificate Password: ");
	
	} while (0);

	if (bio)
		BIO_free(bio);

	return ret;
}

/*
 * Load the private key for the certificate, optionally handling password
 * prompting if necessary.
 */
EVP_PKEY *loadKey(const char *path)
{
	EVP_PKEY *ret = NULL;
	BIO *bio      = BIO_new(BIO_s_file());

	do
	{
		if (!bio)
			break;

		if (BIO_read_filename(bio, path) <= 0)
			break;

		ret = PEM_read_bio_PrivateKey(bio, NULL, passwordPrompt, "Key Password: ");

	} while (0);

	if (bio)
		BIO_free(bio);
	
	return ret;
}

/*
 * Copies the password from the input buffer to the output buffer and returns
 * the length.
 */
int passwordPrompt(char *buf, int bufSize, int verify, void *str)
{
	char *ret = getpass((char *)str);

	if (ret)
		strncpy(buf, ret, bufSize);

	return strlen(buf);
}
