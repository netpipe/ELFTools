/*
 * ELF binary signature verification API
 *
 * skape
 * mmiller@hick.org
 */
#ifndef _ELFVERIFY_H
#define _ELFVERIFY_H

#ifdef __cplusplus
extern "C" {
#endif

// Size of the version of the signature algorithm being used
#define ELF_SIGNATURE_VERSION_SIZE 4

// The current version is: 0.0.2.0
#define ELF_SIGNATURE_VERSION_MM   0x00 // major major
#define ELF_SIGNATURE_VERSION_Mm   0x00 // major minor
#define ELF_SIGNATURE_VERSION_mM   0x02 // minor major
#define ELF_SIGNATURE_VERSION_mm   0x00 // minor minor

// Initializes a buffer to the current signature version scheme
#define elfverifyInitSignatureVersion(sv)         \
	((char *)(sv))[0] = ELF_SIGNATURE_VERSION_MM;  \
	((char *)(sv))[1] = ELF_SIGNATURE_VERSION_Mm;  \
	((char *)(sv))[2] = ELF_SIGNATURE_VERSION_mM;  \
	((char *)(sv))[3] = ELF_SIGNATURE_VERSION_mm 

// The image is signed and trusted
#define ELFVERIFY_SUCCESS          0x0000
// The image is not signed
#define ELFVERIFY_NOT_SIGNED       0x0001
// The issuer ceritificate is not trusted
#define ELFVERIFY_CERT_UNTRUSTED   0x0002
// The digest supplied did not match the actual digest of the binary
#define ELFVERIFY_INVALID_DIGEST   0x0003

/*
 * Verifies trust using a default callback that will result in the expected
 * behavior of untrusted issuers causing the image to not be trusted.
 */
unsigned long elfverifyTrust(const char *elfImage, const char *caFile, 
		const char *caDirectory);

/*
 * Verifies trust with a custom callback to handle scenarios where an issuer
 * cannot be determined to be trusted
 */
unsigned long elfverifyTrustCallback(const char *elfImage, 
		const char *caFile, const char *caDirectory, unsigned long (*callback)(
			void *userParam, X509 *signer, unsigned long issuerTrusted), 
		void *userParam);

#ifdef ELFVERIFY_INTERNAL

/*
 * Internal method for calculating the checksum of an ELF image
 *
 * Callers outside of the library should have no reason to do this.
 */
unsigned long elfverifyCalculateImageChecksum(MELF *melf,
		unsigned char digest[16], ELF_SPEC_HEADER **sigHeader);

#endif

#ifdef __cplusplus
}
#endif

#endif
