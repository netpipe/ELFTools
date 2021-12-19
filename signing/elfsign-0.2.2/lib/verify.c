/*
 * ELF binary signature verification API
 *
 * skape
 * mmiller@hick.org
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "openssl/objects.h"
#include "openssl/rsa.h"
#include "openssl/md5.h"

#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/bio.h"

#include "melf.h"
#include "verify.h"

/*
 * The ELF verify context ties an ELF binary to a digest
 */
typedef struct _elfverify_ctx 
{

	MELF          *melf;
	unsigned char digest[16];

} ELFVERIFY_CTX;

static X509_STORE *getX509Store(const char *caFile, const char *caPath);
static unsigned long _elfverifyCallback(void *userParam, X509 *signer, 
		unsigned long trustedIssuer);

/*
 * Verifies the trust of a given ELF image using the default trust callback
 */
unsigned long elfverifyTrust(const char *elfImage, const char *caFile, 
		const char *caDirectory)
{
	return elfverifyTrustCallback(elfImage, caFile, caDirectory, 
			_elfverifyCallback, NULL);
}

/*
 * Called in order to verify whether or not an image should be trusted with
 * a provided certificate.  If there is a discrepancy, the provided callback
 * will be issued.  This allows for custom, extended analysis for whether or
 * not a binary should be trusted.
 */
unsigned long elfverifyTrustCallback(const char *elfImage, const char *caFile, 
		const char *caDirectory, 
		unsigned long (*callback)(void *userParam, X509 *signer, 
			unsigned long issuerTrusted), 
		void *userParam)
{
	unsigned long ret = 0, ok = 0, index = 0;
	X509_STORE *store = getX509Store(caFile, caDirectory);
	unsigned char sigVer[ELF_SIGNATURE_VERSION_SIZE];
	STACK_OF(PKCS7_SIGNER_INFO) *signerStack = NULL;
	PKCS7_SIGNER_INFO *currSigner = NULL;
	ELF_SPEC_HEADER *sigHeader = NULL;
	BIO *sigRaw = NULL;
	PKCS7 *sig = NULL;
	ELFVERIFY_CTX ctx;

	memset(sigVer, 0, sizeof(sigVer));
	memset(&ctx, 0, sizeof(ctx));

	do
	{
		/*
		 * Open the ELF image passed in
		 */
		if (!(ctx.melf = melf_open(elfImage, MELF_ACCESS_ALL)))
		{
			ret = ELFVERIFY_NOT_SIGNED;
			break;
		}

		// Calculate the binary digest
		if (!elfverifyCalculateImageChecksum(ctx.melf,
				ctx.digest, &sigHeader))
		{
			ret = ELFVERIFY_NOT_SIGNED;
			break;
		}

		// If the signature header was not found, the binary is not signed.
		if (!sigHeader)
		{
			ret = ELFVERIFY_NOT_SIGNED;
			break;
		}

		// Check to see if the signature is of a valid format
		if (sigHeader->contentLength <= sizeof(sigVer))
		{
			ret = ELFVERIFY_NOT_SIGNED;
			break;
		}

		// Extract the signature version from the section's content
		memcpy(sigVer, sigHeader->content, sizeof(sigVer));

		// Extract PKCS7 from the signature section's content
		sigRaw = BIO_new_mem_buf((char *)sigHeader->content + sizeof(sigVer), 
				sigHeader->contentLength);

		// Read in the actual signature from the bio
		sig    = PEM_read_bio_PKCS7(sigRaw, NULL, NULL, NULL);

		// No valid PKCS7 signature?
		if (!sig)
		{
			ret = ELFVERIFY_INVALID_DIGEST;
			break;
		}

		// Grab the signer information
		signerStack = PKCS7_get_signer_info(sig);

		ret = ELFVERIFY_NOT_SIGNED;
		ok  = 0;

		// Enumerate all of the signers, trying to find a trusted entity
		while ((signerStack) && 
		       (currSigner = sk_PKCS7_SIGNER_INFO_value(signerStack, index++)))
		{
			STACK_OF(X509) *certStack = sk_X509_new_null();
			X509 *cert = PKCS7_cert_from_signer_info(sig, currSigner);
			BIO  *outDigest = BIO_new(BIO_s_mem());
			unsigned char *outDigestMd5 = NULL;
			unsigned long outDigestMd5Length = 0, issuerTrusted = 0;

			// Allocation problems?
			if ((!certStack) || 
			    (!outDigest))
			{
				if (outDigest)
					BIO_free(outDigest);
				if (certStack)
					sk_X509_free(certStack);
				break;
			}

			sk_X509_push(certStack, cert);

			// Verify the actual md5 digest
			if (PKCS7_verify(sig, certStack, store, NULL, outDigest, 
					PKCS7_TEXT) <= 0)
				issuerTrusted = 0;
			else
				issuerTrusted = 1;

			// Call the callback if one was supplied
			if (callback)
				ok = callback(userParam, cert, issuerTrusted);

			// If the issuer is not trusted or the callback rejected us
			if (!ok)
			{
				if (!issuerTrusted)
					ret = ELFVERIFY_CERT_UNTRUSTED;

				sk_X509_free(certStack);
				BIO_free(outDigest);
				break;
			}
			else if ((!issuerTrusted) &&
			         (ok))
			{
				// If the issuer is not trusted but the callback has opted to 
				// continue, disable signature validation of the data so that
				// we can get at the digest
				if (PKCS7_verify(sig, certStack, store, NULL, outDigest,
						PKCS7_TEXT | PKCS7_NOSIGS) <= 0)
				{
					sk_X509_free(certStack);
					BIO_free(outDigest);
					break;
				}
			}

			// Calculate the digest
			outDigestMd5Length = BIO_get_mem_data(outDigest, &outDigestMd5);

			// Digest's mismatch, binary was modified.
			if ((outDigestMd5Length != sizeof(ctx.digest)) ||
			    (memcmp(ctx.digest, outDigestMd5, outDigestMd5Length)))
			{
				ret = ELFVERIFY_INVALID_DIGEST;
				ok  = 0;
			}

			// Free up
			sk_X509_free(certStack);
			BIO_free(outDigest);

			if (!ok)
				break;
		}

		// If ret is zero and okay is zero, the the file is not to be trusted.
		if (ret == 0 && !ok)
		{
			ret = ELFVERIFY_CERT_UNTRUSTED;
			break;
		}
		else if (!ok)
			break;

		// Else, if we get here, the file is trusted.
		ret = ELFVERIFY_SUCCESS;

	} while (0);

	// Cleanup
	if (store)
		X509_STORE_free(store);
	if (sigRaw)
		BIO_free(sigRaw);
	if (sig)
		PKCS7_free(sig);
	if (ctx.melf)
		melf_destroy(ctx.melf);

	return ret;
}

/*
 * Internal routine
 *
 * Calculates the digest of the supplied image, excluding signature 
 * information if it exists
 */
unsigned long elfverifyCalculateImageChecksum(MELF *melf,
		unsigned char digest[16], ELF_SPEC_HEADER **sigHeader)
{
	ELF_SPEC_HEADER *currSection, *currProgram, *lSigHeader = NULL;
	Elf32_Off sectionTableOffset;
	Elf32_Half numSections;
	Elf32_Ehdr *elfHeader;
	MD5_CTX md5;

	MD5_Init(&md5);

	// Include the ELF header, but with the number of sections set minus one,
	// under the assumption that any binary having its checksum
	// calculated will already have a signature header added to it. 
	// Yes, I can hear you screaming now.  This makes my life easier. :P
	//
	// Note that elfsign, the tool, always creates the signature section before
	// calculating the checksum.
	elfHeader = melf_elfGetRaw(melf);

	numSections        = melf_elfGetSectionHeaderCount(melf);
	sectionTableOffset = melf_elfGetSectionHeaderOffset(melf);

	melf_elfSetSectionHeaderCount(melf, numSections - 1);
	melf_elfSetSectionHeaderOffset(melf, 0);

	MD5_Update(&md5, elfHeader, sizeof(Elf32_Ehdr));

	melf_elfSetSectionHeaderCount(melf, numSections);
	melf_elfSetSectionHeaderOffset(melf, sectionTableOffset);

	// Enumerate all of the sections, including their checksums
	for (currSection = melf_sectionGetEnum(melf);
		  currSection;
		  currSection = melf_sectionEnumNext(melf, currSection))
	{
		const char *name = melf_sectionGetName(melf, currSection);
		int restoreSize = 0;
		Elf32_Half size = 0;

		// Skip the signature section
		if (name && !strcmp(name, ".sig"))
		{
			// Two signatures?
			if (lSigHeader)
				break;

			lSigHeader = currSection;

			continue;
		}

		// If this section is a string table and it's also the binary's string table...
		if ((melf_sectionGetType(melf, currSection) == SHT_STRTAB) &&
		    (melf_sectionGetIndex(melf, currSection) == melf_elfGetStringTableIndex(melf)))
		{
			size        = melf_sectionGetSize(melf, currSection);
			restoreSize = 1;

			// Exclude '.sig\0'
			melf_sectionSetSize(melf, currSection, size - 5);
		}

		// Include the section's body content in the checksum
		if (currSection->content)
			MD5_Update(&md5, currSection->content, currSection->contentLength);

		// Include the section's header in the checksum
		MD5_Update(&md5, 
				&currSection->spec.section, 
				melf_elfGetSectionEntrySize(melf));

		if (restoreSize)
			melf_sectionSetSize(melf, currSection, size);
	}

	// Enumerate all of the programs, including their checksums
	for (currProgram = melf_programGetEnum(melf);
		  currProgram;
		  currProgram = melf_sectionEnumNext(melf, currProgram))
	{
		MD5_Update(&md5, 
				&currProgram->spec.program, 
				melf_elfGetProgramEntrySize(melf));
	}

	// Finalize the digest
	MD5_Final(digest, &md5);

	if (sigHeader)
		*sigHeader = lSigHeader;

	return 1;
}

/*
 * Place holder callback for validating trust.  Defaults to the expected 
 * behavior of not trusting an untrusted issuer.
 */
static unsigned long _elfverifyCallback(void *userParam, X509 *signer, 
		unsigned long trustedIssuer)
{
	return (trustedIssuer) ? 1 : 0;
}

/*
 * Sets up X509 lookup information
 */
static X509_STORE *getX509Store(const char *caFile, const char *caPath)
{
	X509_STORE *store = NULL;
	X509_LOOKUP *look = NULL;

	do
	{
		if (!(store = X509_STORE_new()))
			break;

		look = X509_STORE_add_lookup(store, X509_LOOKUP_file());

		if (look && caFile)
		{
			if (!X509_LOOKUP_load_file(look, caFile, X509_FILETYPE_PEM))
				break;
		}
		else if (look)
			X509_LOOKUP_load_file(look, NULL, X509_FILETYPE_DEFAULT);

		look = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());

		if (look && caPath)
		{
			if (!X509_LOOKUP_add_dir(look, caPath, X509_FILETYPE_PEM))
				break;
		}
		else if (look)
			X509_LOOKUP_add_dir(look, NULL, X509_FILETYPE_DEFAULT);

	} while (0);

	return store;
}

