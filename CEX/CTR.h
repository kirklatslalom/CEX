﻿// The GPL version 3 License (GPLv3)
// 
// Copyright (c) 2017 vtdev.com
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
// along with this program. If not, see <http://www.gnu.org/licenses/>.
// 
//
// Implementation Details:
// An implementation of an integer Counter Mode (CTR).
// Written by John Underhill, September 24, 2014
// Updated September 16, 2016
// Updated April 18, 2017
// Updated October 14, 2017
// Contact: develop@vtdev.com

#ifndef CEX_CTR_H
#define CEX_CTR_H

#include "ICipherMode.h"

NAMESPACE_MODE

/// <summary>
/// An implementation of a Big-Endian integer Counter Mode
/// </summary> 
/// 
/// <example>
/// <description>Encrypting a single block of bytes:</description>
/// <code>
/// CTR cipher(new AHX());
/// // initialize for encryption
/// cipher.Initialize(true, SymmetricKey(Key, Nonce));
/// // encrypt one block
/// cipher.EncryptBlock(Input, 0, Output, 0);
/// </code>
/// </example>
///
/// <example>
/// <description>Encrypting using parallel processing:</description>
/// <code>
/// CTR cipher(BlockCiphers::AHX);
/// // enable parallel and define parallel input block size
/// cipher.IsParallel() = true;
/// // calculated automatically based on cache size, but overridable
/// cipher.ParallelBlockSize() = cipher.ProcessorCount() * 32000;
/// // initialize for encryption
/// cipher.Initialize(true, SymmetricKey(Key, Nonce));
/// // encrypt one parallel sized block
/// cipher.Transform(Input, 0, Output, 0);
/// </code>
/// </example>
/// 
/// <remarks>
/// <description><B>Overview:</B></description>
/// <para>The CTR Counter mode generates a key-stream by encrypting successive values of an incrementing Big Endian ordered 8bit integer counter array. \n
/// The key-stream is then XOR'd with the input message block creating a type of stream cipher. \n
/// In parallel mode, the CTR modes counter is increased by a number factored from the number of message blocks (ParallelBlockSize), allowing for counter pre-calculation and multi-threaded processing. \n
/// The implementation is further parallelized by constructing a 'staggered' counter array, and processing large sub-blocks using 128 or 256 SIMD instructions through the ciphers Transform-64/128 SIMD methods.</para>
/// 
/// <description><B>Description:</B></description>
/// <para><EM>Legend:</EM> \n 
/// <B>C</B>=ciphertext, <B>P</B>=plaintext, <B>K</B>=key, <B>E</B>=encrypt, <B>^</B>=XOR \n
/// <EM>Encryption</EM> \n
/// C0 ← IV. For 1 ≤ j ≤ t, Cj ← EK(Cj) ^ Pj, C+1.</para> \n
///
/// <description><B>Multi-Threading:</B></description>
/// <para>The transformation function of the CTR mode is not limited by a dependency chain; this mode can be both SIMD pipelined and multi-threaded.
/// Output from the parallelized functions aligns with the output from a standard sequential CTR implementation. \n
/// This is achieved by pre-calculating the counters positional offset over multiple 'chunks' of key-stream, which are then generated independently across threads. \n 
/// The key stream generated by encrypting the counter array(s), is used as a source of random, and XOR'd with the message input to produce the cipher text.</para>
///
/// <description>Implementation Notes:</description>
/// <list type="bullet">
/// <item><description>In CTR mode, Encryption/Decryption can both be pipelined (SSE3-128 or AVX-256), and multi-threaded.</description></item>
/// <item><description>A cipher mode constructor can either be initialized with a block cipher instance, or using the block ciphers enumeration name.</description></item>
/// <item><description>A block cipher instance created using the enumeration constructor, is automatically deleted when the class is destroyed.</description></item>
/// <item><description>The Transform functions are virtual, and can be accessed from an ICipherMode instance.</description></item>
/// <item><description>The EncryptBlock function can only be accessed through the class instance.</description></item>
/// <item><description>The transformation methods can not be called until the Initialize(bool, ISymmetricKey) function has been called.</description></item>
/// <item><description>If the system supports Parallel processing, IsParallel() is set to true; passing an input block of ParallelBlockSize() to the transform.</description></item>
/// <item><description>The ParallelThreadsMax() property is used as the thread count in the parallel loop; this must be an even number no greater than the number of processer cores on the system.</description></item>
/// <item><description>ParallelBlockSize() is calculated automatically based on the processor(s) L1 data cache size, this property can be user defined, and must be evenly divisible by ParallelMinimumSize().</description></item>
/// <item><description>The ParallelBlockSize() can be changed through the ParallelProfile() property</description></item>
/// <item><description>Parallel block calculation ex. <c>ParallelBlockSize = N - (N % .ParallelMinimumSize);</c></description></item>
/// </list>
/// 
/// <description>Guiding Publications:</description>
/// <list type="number">
/// <item><description>NIST <a href="http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf">SP800-38A</a>.</description></item>
/// <item><description>Comments to NIST concerning AES Modes of Operations: <a href="http://csrc.nist.gov/groups/ST/toolkit/BCM/documents/proposedmodes/ctr/ctr-spec.pdf">CTR-Mode Encryption</a>.</description></item>
/// <item><description>Handbook of Applied Cryptography <a href="http://cacr.uwaterloo.ca/hac/about/chap7.pdf">Chapter 7: Block Ciphers</a>.</description></item>
/// </list>
/// </remarks>
class CTR final : public ICipherMode
{
private:

	static const size_t BLOCK_SIZE = 16;
	static const std::string CLASS_NAME;

	std::unique_ptr<IBlockCipher> m_blockCipher;
	BlockCiphers m_cipherType;
	std::vector<byte> m_ctrVector;
	bool m_destroyEngine;
	bool m_isDestroyed;
	bool m_isEncryption;
	bool m_isInitialized;
	bool m_isLoaded;
	ParallelOptions m_parallelProfile;

public:

	//~~~Constructor~~~//

	/// <summary>
	/// Copy constructor: copy is restricted, this function has been deleted
	/// </summary>
	CTR(const CTR&) = delete;

	/// <summary>
	/// Copy operator: copy is restricted, this function has been deleted
	/// </summary>
	CTR& operator=(const CTR&) = delete;

	/// <summary>
	/// Default constructor: default is restricted, this function has been deleted
	/// </summary>
	CTR() = delete;

	/// <summary>
	/// Initialize the Cipher Mode using a block cipher type name
	/// </summary>
	///
	/// <param name="CipherType">The formal enumeration name of a block cipher</param>
	///
	/// <exception cref="Exception::CryptoCipherModeException">Thrown if an undefined block cipher type name is used</exception>
	explicit CTR(BlockCiphers CipherType);

	/// <summary>
	/// Initialize the Cipher Mode using a block cipher instance
	/// </summary>
	///
	/// <param name="Cipher">The uninitialized block cipher instance; can not be null</param>
	///
	/// <exception cref="Exception::CryptoCipherModeException">Thrown if a null block cipher is used</exception>
	explicit CTR(IBlockCipher* Cipher);

	/// <summary>
	/// Destructor: finalize this class
	/// </summary>
	~CTR() override;

	//~~~Accessors~~~//

	/// <summary>
	/// Read Only: Block size of internal cipher in bytes
	/// </summary>
	const size_t BlockSize() override;

	/// <summary>
	/// Read Only: The block ciphers formal type name
	/// </summary>
	const BlockCiphers CipherType() override;

	/// <summary>
	/// Read Only: The underlying Block Cipher instance
	/// </summary>
	IBlockCipher* Engine() override;

	/// <summary>
	/// Read Only: The cipher modes type name
	/// </summary>
	const CipherModes Enumeral() override;

	/// <summary>
	/// Read Only: True if initialized for encryption, False for decryption
	/// </summary>
	const bool IsEncryption() override;

	/// <summary>
	/// Read Only: The Block Cipher is ready to transform data
	/// </summary>
	const bool IsInitialized() override;

	/// <summary>
	/// Read Only: Processor parallelization availability.
	/// <para>Indicates whether parallel processing is available with this mode.
	/// If parallel capable, input/output data arrays passed to the transform must be ParallelBlockSize in bytes to trigger parallelization.</para>
	/// </summary>
	const bool IsParallel() override;

	/// <summary>
	/// Read Only: Array of allowed cipher input key byte-sizes
	/// </summary>
	const std::vector<SymmetricKeySize> &LegalKeySizes() override;

	/// <summary>
	/// Read Only: The cipher modes class name
	/// </summary>
	const std::string Name() override;

	/// <summary>
	/// Read Only: The CBC initialization vector (exposed for CMAC)
	/// </summary>
	const std::vector<byte> &Nonce() { return m_ctrVector; }

	/// <summary>
	/// Read Only: Parallel block size; the byte-size of the input/output data arrays passed to a transform that trigger parallel processing.
	/// <para>This value can be changed through the ParallelProfile class.</para>
	/// </summary>
	const size_t ParallelBlockSize() override;

	/// <summary>
	/// Read/Write: Parallel and SIMD capability flags and sizes (Not supported in this mode)
	/// </summary>
	ParallelOptions &ParallelProfile() override;

	//~~~Public Functions~~~//

	/// <summary>
	/// Decrypt a single block of bytes.
	/// <para>Decrypts one block of bytes beginning at a zero index.
	/// Initialize(bool, ISymmetricKey) must be called before this method can be used.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of encrypted bytes</param>
	/// <param name="Output">The output array of decrypted bytes</param>
	void DecryptBlock(const std::vector<byte> &Input, std::vector<byte> &Output) override;

	/// <summary>
	/// Decrypt a block of bytes with offset parameters.
	/// <para>Decrypts one block of bytes at the designated offsets.
	/// Initialize(bool, ISymmetricKey) must be called before this method can be used.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of encrypted bytes</param>
	/// <param name="InOffset">Starting offset within the Input array</param>
	/// <param name="Output">The output array of decrypted bytes</param>
	/// <param name="OutOffset">Starting offset within the Output array</param>
	void DecryptBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) override;

	/// <summary>
	/// Encrypt a single block of bytes. 
	/// <para>Encrypts one block of bytes beginning at a zero index.
	/// Initialize(bool, ISymmetricKey) must be called before this method can be used.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of plain text bytes</param>
	/// <param name="Output">The output array of encrypted bytes</param>
	void EncryptBlock(const std::vector<byte> &Input, std::vector<byte> &Output) override;

	/// <summary>
	/// Encrypt a block of bytes using offset parameters. 
	/// <para>Encrypts one block of bytes at the designated offsets.
	/// Initialize(bool, ISymmetricKey) must be called before this method can be used.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of plain text bytes</param>
	/// <param name="InOffset">Starting offset within the input array</param>
	/// <param name="Output">The output array of encrypted bytes</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	void EncryptBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) override;

	/// <summary>
	/// Initialize the Cipher instance
	/// </summary>
	/// 
	/// <param name="Encryption">True if cipher is used for encryption, False to decrypt</param>
	/// <param name="KeyParams">SymmetricKey containing the encryption Key and Initialization Vector</param>
	/// 
	/// <exception cref="CryptoCipherModeException">Thrown if a null Key or Nonce is used</exception>
	void Initialize(bool Encryption, ISymmetricKey &KeyParams) override;

	/// <summary>
	/// Set the maximum number of threads allocated when using multi-threaded processing.
	/// <para>When set to zero, thread count is set automatically. If set to 1, sets IsParallel() to false and runs in sequential mode. 
	/// Thread count must be an even number, and not exceed the number of processor cores.</para>
	/// </summary>
	///
	/// <param name="Degree">The desired number of threads</param>
	void ParallelMaxDegree(size_t Degree) override;

	/// <summary>
	/// Transform a length of bytes with offset parameters. 
	/// <para>This method processes a specified length of bytes, utilizing offsets incremented by the caller.
	/// If IsParallel() is set to true, and the length is at least ParallelBlockSize(), the transform is run in parallel processing mode.
	/// To disable parallel processing, set the ParallelOptions().IsParallel() property to false.
	/// Initialize(bool, ISymmetricKey) must be called before this method can be used.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset within the input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	/// <param name="Length">The number of bytes to transform</param>
	void Transform(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length) override;

private:

	void Encrypt128(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset);
	void Generate(std::vector<byte> &Output, const size_t OutOffset, const size_t Length, std::vector<byte> &Counter);
	void Scope();
	void ProcessParallel(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length);
	void ProcessSequential(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length);
};

NAMESPACE_MODEEND
#endif
