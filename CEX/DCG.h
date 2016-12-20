// The GPL version 3 License (GPLv3)
// 
// Copyright (c) 2016 vtdev.com
// This file is part of the CEX Cryptographic library.
// 
// This program is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.If not, see <http://www.gnu.org/licenses/>.
//
// 
// Implementation Details:
// An implementation of a message Digest Counter Generator (DCG)
// Updated September 30, 2016
// Written by John Underhill, January 09, 2014

#ifndef _CEX_DCG_H
#define _CEX_DCG_H

#include "IDrbg.h"
#include "IDigest.h"

NAMESPACE_DRBG

using Digest::IDigest;
using Enumeration::Digests;

/// <summary>
/// An implementation of a message Digest Counter Generator (DCG)
/// </summary> 
/// 
/// <example>
/// <description>Generate an array of pseudo random bytes:</description>
/// <code>
/// DCG rnd(Digests::SHA512, [Providers::CSP]);
/// // initialize
/// rnd.Initialize(Seed, [Nonce], [Info]);
/// // generate bytes
/// rnd.Generate(Output, [Offset], [Size]);
/// </code>
/// </example>
/// 
/// <remarks>
/// <description><B>Overview:</B></description>
/// <para>The DCG (Digest Counter Generator) uses a hash function run in a counter mode similar to a block cipher CTR mode.<br>
/// The design is an update to a standard hash-based counter mechanism described in NIST documentation, and implemented in the BouncyCastle DigestRandomCounter class.<br>
/// As described in the thesis; Security Analysis for Pseudo-Random Numbers Generators by Sylvain Ruhault, because the original mechanism lacked a source of random used to update the seed with fresh entropy,
/// the mechanism will eventually fail due to internal state decomposition, and so should not be considered a resiliant drbg design.<br>
/// This implementation however does provide an (optional) entropy source provider option, and so can be implemented with both Predictive and Backtracking reisitance per recommendations Section 8.8 of SP80090A revision 1.<br>
/// The way in which the entropy provider distributes seed material is also an important design change. <br>
/// In the original algorithm, the Generate function processes the digest seed, the state, and the state-counter to produce the new state.<br>
/// In a Merkle�Damg�rd construction (SHA2), the finalizer appends a code to the end of the last block, (and if the block is full, it processes a block of zero-byte padding with the code), 
/// this is compensated for by subtracting the codes length from the random padding request length when required.<br>
/// With, for example, SHA2-512 which uses a 128 byte block size, the number of bytes processed with this configuration would be 64+64+8, leaving 120 bytes of zero-padding processed by the digests finalize function.<br>
/// With the entropy source engaged, these empty bytes are filled with fresh entropy, ensuring that only full blocks are compressed, which in turn, should yield a more secure output.<br>
/// Another important change is in the use of the Nonce field, (either input through one of the Initialize functions directly, or as a parameter of the RngPrams key container), this field now sets the starting 
/// count in the primary state counter.<br>
/// The left-most 8 bytes of the Nonce are copied to the state counter, and the entire Nonce is then passed to the update function to be processed as seed material.<br>
/// The Update() function works in a similar way to the Generate() function, in that the entropy provider is used to pad the input blocks to the hash function with fresh entropy.</para>
///
/// <description><B>Initialization and Update:</B></description>
/// <para>The Initialize functions have three different parameter options: the Seed which is the primary key, 
/// the Nonce used to initialize the internal state-counter, and the Info which is an additional source of entropy.<br>
/// The Seed value must be one of the LegalKeySizes() in length, and must be a secret and random value.<br>
/// The supported seed-sizes are calculated based on the hash functions internal block size, and can vary depending on which message digest is used to instantiate the generator.<br>
/// The eight byte (NonceSize) Nonce value is another secret value, used to initialize the internal state counter to a non-zero random value.<br>
/// The Update function uses the seed value to re-key the generator via the internal key derivation function.<br>
/// The update functions Seed parameter, must be a random seed value equal in length to the seed used to initialize the generator.</para>
///
/// <description><B>Description:</B></description>
/// <para><EM>Legend:</EM><br> 
/// <B>H<SUB>K</SUB></B>=hash_function, <B>P</B>=entropy_provider, <B>S1</B>=seed_material, <B>S2</B>=state, <B>S3</B>=seed_counter, <B>S4</B>=state_counter</para>
/// 
/// <para><EM>Update</EM><br>
/// The Update function takes as input the current internal state(S1, S2, S3, S4) and an optional entropy input P; it outputs a new internal state where only S1 is updated.<br>
/// Require: S = (S1, S2, S3, S4), I<br>
/// Ensure: S<br>
/// 1) S1 = H<SUB>K</SUB>(S1 || I)<br>
/// 2) if (P) then<br>
///      return S<SUP>0</SUP> = (S1, S2, S3, S4), P<br>
///    else<br>
///      return S<SUP>0</SUP> = (S1, S2, S3, S4)
/// </para>
///
/// <para><EM>Generate</EM><br>
/// Require: S = (S1, S2, S3, S4)<br>
/// Ensure: S<br>
/// 1) S4 = S4 + 1<br>
/// 2) if (P) then <br>
///      S2 = H<SUB>K</SUB>(S4 || S2 || S1 || P)<br>
///    else <br>
///      S2 = H<SUB>K</SUB>(S4 || S2 || S1)<br>
/// 3) if S3 mod 10 = 0 then<br>
/// 4) S3 = S3 + 1<br>
/// 5) S1 = H<SUB>K</SUB>(S1 || S3)<br>
/// 6) end if<br>
/// 7) return S<SUP>0</SUP> = (S1, S2, S3, S4)
/// </para>
///
/// <description><B>Predictive Resistance:</B></description>
/// <para>Predictive and backtracking resistance prevent an attacker who has gained knowledge of generator state at some time from predicting future or previous outputs from the generator.<br>
/// The optional resistance mechanism uses an entropy provider to add seed material to the generator, this new seed material is passed through the hash function along with the current state, 
/// the output hash is used to reseed the generator.<br>
/// The default interval at which this reseeding occurs is 1000 times the digest output size in bytes, but can be set using the ReseedThreshold() property; once this number of bytes or greater has been generated, 
/// the seed is regenerated.<br> 
/// Predictive resistance is strongly recommended when producing large amounts of psuedo-random (10kb or greater).</para>
///
/// <description>Implementation Notes:</description>
/// <list type="bullet">
/// <item><description>The class constructor can either be initialized with a message digest instance, or using the digests enumeration name.</description></item>
/// <item><description>A digest instance created using the enumeration constructor, is automatically deleted when the class is destroyed.</description></item>
/// <item><description>An entropy provider can be specified through the constructor, which provides a continues stream of entropy to the reseed and generate functions, this is stronly recommended with large (+10kb) outputs.</description></item>
/// <item><description>The recommended maximum-security sizes of Seed, Nonce, and Info parameters, is each field size set to the underlying digests block size.</description></item>
/// <item><description>The generator can be initialized with either a SymmetricKey key container class, or with a Seed and optional inputs of Nonce and Info.</description></item>
/// <item><description>The LegalKeySizes() property contains a list of recommended combined seed (Seed, Nonce, Info) input sizes.</description></item>
/// <item><description>There are three LegalKeySizes, with DCG, the middle value is the recommended seed length; i.e. LegalKeySizes()[1].</description></item>
/// <item><description>The Generate() methods can not be used until an Initialize() function has been called, and the generator is seeded.</description></item>
/// </list>
/// 
/// <description>Guiding Publications:</description>
/// <list type="number">
/// <item><description>NIST <a href="http://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-90Ar1.pdf">SP800-90A R1</a>.</description></item>
/// <item><description>NIST <a href="http://csrc.nist.gov/publications/nistpubs/800-90A/SP800-90A.pdf">SP800-90A</a>: Appendix E1</description>.</item>
/// <item><description>BouncyCastle <a href="https://github.com/bcgit/bc-java/blob/ae63147936376e85e068c7b63373d4e930c3fe58/core/src/main/java/org/bouncycastle/crypto/prng/DigestRandomGenerator.java">DigestRandomGenerator.java</a>: Section 10.1</description>.</item>
/// <item><description>Security Analysis for <a href="https://tel.archives-ouvertes.fr/tel-01236602/document">Pseudo-Random Numbers Generators</a></description>.</item>
/// <item><description>NIST <a href="http://csrc.nist.gov/publications/drafts/800-90/draft-sp800-90b.pdf">SP800-90B</a>: Recommendation for the Entropy Sources Used for Random Bit Generation.</description></item>
/// <item><description>NIST <a href="http://csrc.nist.gov/publications/fips/fips140-2/fips1402.pdf">Fips 140-2</a>: Security Requirments For Cryptographic Modules.</description></item>
/// <item><description>NIST <a href="http://eprint.iacr.org/2006/379.pdf">Security Bounds</a> for the Codebook-based: Deterministic Random Bit Generator.</description></item>
/// </list>
/// </remarks>
class DCG : public IDrbg
{
private:

	static const size_t COUNTER_SIZE = sizeof(ulong);
	const uint64_t MAX_OUTPUT = 35184372088832;
	const size_t MAX_REQUEST = 65536;
	const size_t MAX_RESEED = 536870912;
	const size_t MINSEED_SIZE = 8;

	bool m_destroyEngine;
	Digests m_digestType;
	std::vector<byte> m_dgtSeed;
	std::vector<byte> m_dgtState;
	std::vector<byte> m_distributionCode;
	size_t m_distributionCodeMax;
	bool m_isDestroyed;
	bool m_isInitialized;
	std::vector<SymmetricKeySize> m_legalKeySizes;
	IDigest* m_msgDigest;
	bool m_prdResistant;
	IProvider* m_providerSource;
	Providers m_providerType;
	size_t m_reseedCounter;
	size_t m_reseedRequests;
	size_t m_reseedThreshold;
	size_t m_secStrength;
	std::vector<byte> m_seedCtr;
	std::vector<byte> m_stateCtr;

public:

	DCG(const DCG&) = delete;
	DCG& operator=(const DCG&) = delete;
	DCG& operator=(DCG&&) = delete;

	//~~~Properties~~~//

	/// <summary>
	/// Get/Set: Reads or Sets the personalization string value in the KDF initialization parameters.
	/// <para>Must be set before <see cref="Initialize(bool, SymmetricKey)"/> is called.
	/// Changing this code will create a unique distribution of the generator.
	/// Code can be sized as either a zero byte array, or any length up to the DistributionCodeMax size.
	/// For best security, the distribution code should be random, secret, and equal in length to the DistributionCodeMax() size.</para>
	/// </summary>
	virtual std::vector<byte> &DistributionCode() { return m_distributionCode; }

	/// <summary>
	/// Get: The maximum size of the distribution code in bytes.
	/// <para>The distribution code can be used as a secondary source of entropy (secret) in the KDF key expansion phase.
	/// For best security, the distribution code should be random, secret, and equal in size to this value.</para>
	/// </summary>
	virtual const size_t DistributionCodeMax() { return m_distributionCodeMax; }

	/// <summary>
	/// Get: The Drbg generators type name
	/// </summary>
	virtual const Drbgs Enumeral() { return Drbgs::DCG; }

	/// <summary>
	/// Get: Generator is ready to produce random
	/// </summary>
	virtual const bool IsInitialized() { return m_isInitialized; }

	/// <summary>
	/// Get: The legal input seed sizes in bytes
	/// </summary>
	virtual std::vector<SymmetricKeySize> LegalKeySizes() const { return m_legalKeySizes; };

	/// <summary>
	/// Get: The maximum number of bytes that can be generated with a generator instance
	/// </summary>
	virtual const uint64_t MaxOutputSize() { return MAX_OUTPUT; }

	/// <summary>
	/// Get: The maximum number of bytes that can be generated in a single request
	/// </summary>
	virtual const size_t MaxRequestSize() { return MAX_REQUEST; }

	/// <summary>
	/// Get: The maximum number of times the generator can be reseeded
	/// </summary>
	virtual const size_t MaxReseedCount() { return MAX_RESEED; }

	/// <summary>
	/// Get: The Drbg generators class name
	/// </summary>
	virtual const std::string Name() { return "DCG"; }

	/// <summary>
	/// Get: The size of the nonce counter value in bytes
	/// </summary>
	virtual const size_t NonceSize() { return COUNTER_SIZE; }

	/// <summary>
	/// Get/Set: Generating this amount or greater, triggers a re-seed
	/// </summary>
	virtual size_t &ReseedThreshold() { return m_reseedThreshold; }

	/// <summary>
	/// Get: The estimated security strength in bits.
	/// <para>This value depends both on the hash function output size, and the number of bits used to seed the generator.</para>
	/// </summary>
	virtual const size_t SecurityStrength() { return m_secStrength; }

	//~~~Constructor~~~//

	/// <summary>
	/// Instantiate the class using a block cipher type name, and an optional entropy source type
	/// </summary>
	///
	/// <param name="DigestType">The hash digests enumeration type name; the default is SHA512</param>
	/// <param name="ProviderType">The enumeration type name of an entropy source; enables predictive resistance</param>
	///
	/// <exception cref="Exception::CryptoCipherModeException">Thrown if an unrecognized digest type name is used</exception>
	explicit DCG(Digests DigestType = Digests::SHA512, Providers ProviderType = Providers::CSP)
		:
		m_destroyEngine(true),
		m_digestType(DigestType),
		m_dgtSeed(0),
		m_dgtState(0),
		m_isDestroyed(false),
		m_isInitialized(false),
		m_legalKeySizes(0),
		m_providerType(ProviderType),
		m_reseedCounter(0),
		m_reseedRequests(0),
		m_reseedThreshold(0),
		m_stateCtr(COUNTER_SIZE),
		m_seedCtr(COUNTER_SIZE)
	{
		if (DigestType == Digests::None)
			throw CryptoGeneratorException("DCG:CTor", "The digest type type can not be none!");

		LoadState();
	}

	/// <summary>
	/// Instantiate the class using a digest instance, and an optional entropy source 
	/// </summary>
	/// 
	/// <param name="Digest">The hash digest instance</param>
	/// <param name="Provider">Provides an entropy source; enables predictive resistance, can be null</param>
	/// 
	/// <exception cref="Exception::CryptoGeneratorException">Thrown if a null digest is used</exception>
	explicit DCG(IDigest* Digest, IProvider* Provider = 0)
		:
		m_destroyEngine(false),
		m_digestType(Digest->Enumeral()),
		m_dgtSeed(0),
		m_dgtState(0),
		m_isDestroyed(false),
		m_isInitialized(false),
		m_legalKeySizes(0),
		m_msgDigest(Digest),
		m_providerSource(Provider),
		m_providerType(Provider == 0 ? Providers::None : Provider->Enumeral()),
		m_reseedCounter(0),
		m_reseedRequests(0),
		m_reseedThreshold(0),
		m_stateCtr(COUNTER_SIZE),
		m_seedCtr(COUNTER_SIZE)
	{
		if (Digest == 0)
			throw CryptoGeneratorException("DCG:Ctor", "Digest can not be null!");

		LoadState();
	}

	/// <summary>
	/// Finalize objects
	/// </summary>
	virtual ~DCG()
	{
		Destroy();
	}

	//~~~Public Methods~~~//

	/// <summary>
	/// Release all resources associated with the object
	/// </summary>
	virtual void Destroy();

	/// <summary>
	/// Generate a block of pseudo random bytes
	/// </summary>
	/// 
	/// <param name="Output">Output array filled with random bytes</param>
	/// 
	/// <returns>The number of bytes generated</returns>
	virtual size_t Generate(std::vector<byte> &Output);

	/// <summary>
	/// Generate pseudo random bytes using offset and length parameters
	/// </summary>
	/// 
	/// <param name="Output">Output array filled with random bytes</param>
	/// <param name="OutOffset">The starting position within the Output array</param>
	/// <param name="Length">The number of bytes to generate</param>
	/// 
	/// <returns>The number of bytes generated</returns>
	virtual size_t Generate(std::vector<byte> &Output, size_t OutOffset, size_t Length);

	/// <summary>
	/// Initialize the generator with a SymmetricKey structure containing the key, and optional nonce, and info string
	/// </summary>
	/// 
	/// <param name="GenParam">The SymmetricKey containing the generators keying material</param>
	virtual void Initialize(ISymmetricKey &GenParam);

	/// <summary>
	/// Initialize the generator with a seed key
	/// </summary>
	/// 
	/// <param name="Seed">The primary key array used to seed the generator</param>
	/// 
	/// <exception cref="Exception::CryptoGeneratorException">Thrown if the key is too small</exception>
	virtual void Initialize(const std::vector<byte> &Seed);

	/// <summary>
	/// Initialize the generator with key and nonce arrays
	/// </summary>
	/// 
	/// <param name="Seed">The primary key array used to seed the generator</param>
	/// <param name="Nonce">The nonce value containing an additional source of entropy</param>
	virtual void Initialize(const std::vector<byte> &Seed, const std::vector<byte> &Nonce);

	/// <summary>
	/// Initialize the generator with a key, a nonce array, and an information string or nonce
	/// </summary>
	/// 
	/// <param name="Seed">The primary key array used to seed the generator</param>
	/// <param name="Nonce">The nonce value used as an additional source of entropy</param>
	/// <param name="Info">The information string or nonce used as a third source of entropy</param>
	virtual void Initialize(const std::vector<byte> &Seed, const std::vector<byte> &Nonce, const std::vector<byte> &Info);

	/// <summary>
	/// Update the generators keying material, used to refresh the state
	/// </summary>
	///
	/// <param name="Seed">The new seed value array</param>
	/// 
	/// <exception cref="Exception::CryptoGeneratorException">Thrown if the seed is too small</exception>
	virtual void Update(const std::vector<byte> &Seed);

private:
	void Derive();
	void Extract(size_t BlockOffset);
	void Increment(std::vector<byte> &Counter);
	IDigest* LoadDigest(Digests DigestType);
	IProvider* LoadProvider(Providers ProviderType);
	void LoadState();
};

NAMESPACE_DRBGEND
#endif