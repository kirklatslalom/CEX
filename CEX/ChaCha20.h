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
// Principal Algorithms:
// This cipher is based on the ChaCha stream cipher designed by Daniel J. Bernstein:
// ChaCha: <a href="http://cr.yp.to/chacha/chacha-20080128.pdf"/>
// 
// Implementation Details:
// ChaCha20: An implementation of the ChaCha stream cipher
// Written by John Underhill, October 21, 2014
// Updated January 27, 2017
// Updated August 29, 2017
// Updated October 14, 2017
// Contact: develop@vtdev.com

#ifndef CEX_CHACHA20_H
#define CEX_CHACHA20_H

#include "IStreamCipher.h"

NAMESPACE_STREAM

/// <summary>
/// A parallelized ChaCha stream cipher implementation
/// </summary>
/// 
/// <example>
/// <description>Encrypting using parallel processing:</description>
/// <code>
/// SymmetricKey kp(Key, Nonce);
/// ChaCha20 cipher();
/// // set to false to run in sequential mode
/// cipher.IsParallel() = true;
/// // calculated automatically based on cache size, but overridable
/// cipher.ParallelBlockSize() = cipher.ProcessorCount() * 32000;
/// cipher.Initialize(kp);
/// cipher.Transform(Input, Output);
/// </code>
/// </example>
/// 
/// <remarks>
/// <description><B>Overview:</B></description>
/// <para>The ChaCha stream cipher generates a key-stream by encrypting successive values of an incrementing 32bit unsigned integer counter array. \n
/// The key-stream is then XOR'd with the input message block to create the cipher-text output. \n
/// In parallel mode, the counter is increased by a number factored from the number of input blocks, allowing for a multi-threaded operation. \n
/// The implementation is further parallelized by constructing a larger 'staggered' counter array, and processing large blocks using 128 or 256 SIMD instructions.</para>
/// 
/// <description><B>Description:</B></description>
/// <para><EM>Legend:</EM> \n 
/// <B>C</B>=ciphertext, <B>P</B>=plaintext, <B>K</B>=key, <B>E</B>=encrypt, <B>^</B>=XOR \n
/// <EM>Encryption</EM> \n
/// C0 ← IV. For 1 ≤ j ≤ t, Cj ← EK(Cj) ^ Pj, C+1.</para> \n
///
/// <description><B>Multi-Threading:</B></description>
/// <para>The transformation function used by ChaCha is not limited by a dependency chain; this mode can be both SIMD pipelined and multi-threaded. \n
/// This is achieved by pre-calculating the counters positional offset over multiple 'chunks' of key-stream, which are then generated independently across threads. \n 
/// The key stream generated by encrypting the counter array(s), is used as a source of random, and XOR'd with the message input to produce the cipher text.</para>
///
/// <description>Implementation Notes:</description>
/// <list type="bullet">
/// <item><description>Valid Key sizes are 128, 256 (16 and 32 bytes).</description></item>
/// <item><description>Block size is 64 bytes wide.</description></item>
/// <item><description>Valid rounds are 8 through 80 in increments of 2, the default is 20 rounds.</description></item>
/// <item><description>Encryption can both be pipelined (SSE3-128 or AVX2-256), and multi-threaded.</description></item>
/// <item><description>The Transform functions are virtual, and can be accessed from an ICipherMode instance.</description></item>
/// <item><description>The transformation methods can not be called until the Initialize(SymmetricKey) function has been called.</description></item>
/// <item><description>If the system supports Parallel processing, IsParallel() is set to true; passing an input block of ParallelBlockSize() to the transform.</description></item>
/// <item><description>The ParallelThreadsMax() property is used as the thread count in the parallel loop; this must be an even number no greater than the number of processer cores on the system.</description></item>
/// <item><description>ParallelBlockSize() is calculated automatically based on processor(s) cache size but can be user defined, but must be evenly divisible by ParallelMinimumSize().</description></item>
/// <item><description>The ParallelBlockSize() can be changed through the ParallelProfile() property</description></item>
/// <item><description>Parallel block calculation ex. <c>ParallelBlockSize = N - (N % .ParallelMinimumSize);</c></description></item>
/// </list>
/// 
/// <description>Guiding Publications:</description>
/// <list type="number">
/// <item><description>ChaCha20 <a href="http://cr.yp.to/chacha/chacha-20080128.pdf">Specification</a>.</description></item>
/// <item><description>Salsa20 <a href="http://cr.yp.to/snuffle/design.pdf">Design</a>.</description></item>
/// <item><description>Salsa20 <a href="http://cr.yp.to/snuffle/security.pdf">Security</a>.</description></item>
/// </list>
/// </remarks>
class ChaCha20 final : public IStreamCipher
{
private:

	static const size_t BLOCK_SIZE = 64;
	static const std::string CLASS_NAME;
	static const size_t CTR_SIZE = 8;
	static const size_t MAX_ROUNDS = 80;
	static const size_t MIN_ROUNDS = 8;
	static const size_t STATE_PRECACHED = 2048;
	static const std::string SIGMA_INFO;
	static const std::string TAU_INFO;

	std::vector<uint> m_ctrVector;
	std::vector<byte> m_dstCode;
	bool m_isDestroyed;
	bool m_isInitialized;
	std::vector<SymmetricKeySize> m_legalKeySizes;
	std::vector<size_t> m_legalRounds;
	ParallelOptions m_parallelProfile;
	size_t m_rndCount;
	std::vector<uint> m_wrkState;

public:

	//~~~Constructor~~~//

	/// <summary>
	/// Copy constructor: copy is restricted, this function has been deleted
	/// </summary>
	ChaCha20(const ChaCha20&) = delete;

	/// <summary>
	/// Copy operator: copy is restricted, this function has been deleted
	/// </summary>
	ChaCha20& operator=(const ChaCha20&) = delete;

	/// <summary>
	/// Initialize the class
	/// </summary>
	///
	/// <param name="Rounds">Number of transformation rounds. 
	/// <para>The LegalRounds property contains available sizes. 
	/// Default is 20 rounds, minimum is 8, and maximum is 80 rounds.</para></param>
	///
	/// <exception cref="Exception::CryptoSymmetricCipherException">Thrown if an invalid rounds count is chosen</exception>
	explicit ChaCha20(size_t Rounds = 20);

	/// <summary>
	/// Destructor: finalize this class
	/// </summary>
	~ChaCha20() override;

	//~~~Accessors~~~//

	/// <summary>
	/// Read Only: Unit block size of internal cipher in bytes.
	/// <para>Block size is 64 bytes wide.</para>
	/// </summary>
	const size_t BlockSize() override;

	/// <summary>
	/// Read Only: The salt value in the initialization parameters (Tau-Sigma).
	/// <para>This value can only be set with the Info parameter of an ISymmetricKey member, or use the default.
	/// Changing this code will create a unique distribution of the cipher.
	/// For best security, the code should be a random extenion of the key, with rounds increased to 40 or more.
	/// Code must be non-zero, 16 bytes in length, and sufficiently asymmetric.
	/// If the Info parameter of an ISymmetricKey is non-zero, it will overwrite the distribution code.</para>
	/// </summary>
	const std::vector<byte> &DistributionCode() override;

	/// <summary>
	/// Read Only: The stream ciphers type name
	/// </summary>
	const StreamCiphers Enumeral() override;

	/// <summary>
	/// Read Only: Cipher is ready to transform data
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
	/// Read Only: Available transformation round assignments
	/// </summary>
	const std::vector<size_t> &LegalRounds() override;

	/// <summary>
	/// Read Only: Parallel block size; the byte-size of the input/output data arrays passed to a transform that trigger parallel processing.
	/// <para>This value can be changed through the ParallelProfile class.</para>
	/// </summary>
	const size_t ParallelBlockSize() override;

	/// <summary>
	/// Read/Write: Parallel and SIMD capability flags and recommended sizes.
	/// <para>The maximum number of threads allocated when using multi-threaded processing can be set with the ParallelMaxDegree() property.
	/// The ParallelBlockSize() property is auto-calculated, but can be changed; the value must be evenly divisible by ParallelMinimumSize().
	/// Changes to these values must be made before the <see cref="Initialize(SymmetricKey)"/> function is called.</para>
	/// </summary>
	ParallelOptions &ParallelProfile() override;

	/// <summary>
	/// Read Only: The stream ciphers class name
	/// </summary>
	const std::string Name() override;

	/// <summary>
	/// Read Only: Number of rounds
	/// </summary>
	const size_t Rounds() override;

	//~~~Public Functions~~~//

	/// <summary>
	/// Initialize the cipher
	/// </summary>
	/// 
	/// <param name="KeyParams">Cipher key container. 
	/// <para>Uses the Key and Nonce fields of KeyParams. The <see cref="LegalKeySizes"/> property contains valid Key sizes. 
	/// The Nonce must be 8 bytes in size.</para>
	/// </param>
	///
	/// <exception cref="Exception::CryptoSymmetricCipherException">Thrown if a null or invalid key is used</exception>
	void Initialize(ISymmetricKey &KeyParams) override;

	/// <summary>
	/// Set the maximum number of threads allocated when using multi-threaded processing.
	/// <para>When set to zero, thread count is set automatically. If set to 1, sets IsParallel() to false and runs in sequential mode. 
	/// Thread count must be an even number, and not exceed the number of processor cores.</para>
	/// </summary>
	///
	/// <param name="Degree">The desired number of threads</param>
	void ParallelMaxDegree(size_t Degree) override;

	/// <summary>
	/// Encrypt/Decrypt one block of bytes
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="Output">The output array of transformed bytes</param>
	void TransformBlock(const std::vector<byte> &Input, std::vector<byte> &Output) override;

	/// <summary>
	/// Encrypt/Decrypt one block of bytes
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset within the input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	void TransformBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) override;

	/// <summary>
	/// Encrypt/Decrypt an array of bytes with offset and length parameters.
	/// <para><see cref="Initialize(SymmetricKey)"/> must be called before this method can be used.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset within the input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	/// <param name="Length">Number of bytes to process</param>
	void Transform(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length) override;

private:

	void Expand(const std::vector<byte> &Key, const std::vector<byte> &Iv);
	void Generate(std::vector<byte> &Output, const size_t OutOffset, std::vector<uint> &Counter, const size_t Length);
	void Process(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length);
	void Reset();
	void Scope();
};

NAMESPACE_STREAMEND
#endif

