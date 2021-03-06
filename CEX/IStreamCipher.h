﻿// The GPL version 3 License (GPLv3)
// 
// Copyright (c) 2018 vtdev.com
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

#ifndef CEX_ISTREAMCIPHER_H
#define CEX_ISTREAMCIPHER_H

#include "CexDomain.h"
#include "CryptoSymmetricCipherException.h"
#include "IMac.h"
#include "ISymmetricKey.h"
#include "ParallelOptions.h"
#include "ParallelUtils.h"
#include "StreamAuthenticators.h"
#include "StreamCiphers.h"
#include "SymmetricKeySize.h"

NAMESPACE_STREAM

using Exception::CryptoSymmetricCipherException;
using Mac::IMac;
using Key::Symmetric::ISymmetricKey;
using Common::ParallelOptions;
using Enumeration::StreamAuthenticators;
using Enumeration::StreamCiphers;
using Key::Symmetric::SymmetricKeySize;

/// <summary>
/// Stream Cipher virtual interface class
/// </summary>
class IStreamCipher
{
public:

	//~~~Constructor~~~//

	/// <summary>
	/// Copy constructor: copy is restricted, this function has been deleted
	/// </summary>
	IStreamCipher(const IStreamCipher&) = delete;

	/// <summary>
	/// Copy operator: copy is restricted, this function has been deleted
	/// </summary>
	IStreamCipher& operator=(const IStreamCipher&) = delete;

	/// <summary>
	/// Constructor: Instantiate this class
	/// </summary>
	IStreamCipher() 
	{
	}

	/// <summary>
	/// Destructor: finalize this class
	/// </summary>
	virtual ~IStreamCipher() 
	{
	}

	//~~~Accessors~~~//

	/// <summary>
	/// Read Only: Unit block size of internal cipher in bytes.
	/// <para>Block size must be 16 or 32 bytes wide. 
	/// Value set in class constructor.</para>
	/// </summary>
	virtual const size_t BlockSize() = 0;

	/// <summary>
	/// Read Only: The salt value in the initialization parameters (Tau-Sigma).
	/// <para>This value can only be set with the Info parameter of an ISymmetricKey member, or use the default.
	/// Changing this code will create a unique distribution of the cipher.
	/// For best security, the code should be a random extenion of the key, with rounds increased to 40 or more.
	/// Code must be non-zero, 16 bytes in length, and sufficiently asymmetric.
	/// If the Info parameter of an ISymmetricKey is non-zero, it will overwrite the distribution code.</para>
	/// </summary>
	virtual const std::vector<byte> &DistributionCode() = 0;

	/// <summary>
	/// Read Only: The maximum size of the distribution code in bytes.
	/// <para>The distribution code can be used as a secondary domain key.</para>
	/// </summary>
	virtual const size_t DistributionCodeMax() = 0;

	/// <summary>
	/// Read Only: The stream ciphers type name
	/// </summary>
	virtual const StreamCiphers Enumeral() = 0;

	/// <summary>
	/// Read Only: Cipher is ready to transform data
	/// </summary>
	virtual const bool IsInitialized() = 0;

	/// <summary>
	/// Read Only: Processor parallelization availability.
	/// <para>Indicates whether parallel processing is available with this mode.
	/// If parallel capable, input/output data arrays passed to the transform must be ParallelBlockSize in bytes to trigger parallelization.</para>
	/// </summary>
	virtual const bool IsParallel() = 0;

	/// <summary>
	/// Read Only: Array of allowed cipher input key byte-sizes
	/// </summary>
	virtual const std::vector<SymmetricKeySize> &LegalKeySizes() = 0;

	/// <summary>
	/// Read Only: Available transformation round assignments
	/// </summary>
	virtual const std::vector<size_t> &LegalRounds() = 0;

	/// <summary>
	/// Read Only: The stream ciphers class name
	/// </summary>
	virtual const std::string Name() = 0;

	/// <summary>
	/// Read Only: Parallel block size; the byte-size of the input/output data arrays passed to a transform that trigger parallel processing.
	/// <para>This value can be changed through the ParallelProfile class.</para>
	/// </summary>
	virtual const size_t ParallelBlockSize() = 0;

	/// <summary>
	/// Read/Write: Parallel and SIMD capability flags and sizes 
	/// <para>The maximum number of threads allocated when using multi-threaded processing can be set with the ParallelMaxDegree() property.
	/// The ParallelBlockSize() property is auto-calculated, but can be changed; the value must be evenly divisible by ParallelMinimumSize().
	/// Changes to these values must be made before the <see cref="Initialize(SymmetricKey)"/> function is called.</para>
	/// </summary>
	virtual ParallelOptions &ParallelProfile() = 0;

	/// <summary>
	/// Read Only: Number of rounds
	/// </summary>
	virtual const size_t Rounds() = 0;

	/// <summary>
	/// Read Only: The legal tag length in bytes
	/// </summary>
	virtual const size_t TagSize() = 0;

	//~~~Public Functions~~~//

	/// <summary>
	/// Calculate the MAC code (Tag) and copy it to the Output array.   
	/// <para>The output array must be of sufficient length to receive the MAC code.
	/// This function finalizes the Encryption/Decryption cycle, all data must be processed before this function is called.
	/// Initialize(bool, ISymmetricKey) must be called before the cipher can be re-used.</para>
	/// </summary>
	/// 
	/// <param name="Output">The output array that receives the authentication code</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	/// <param name="Length">The number of MAC code bytes to write to the output array.
	/// <para>Must be no greater than the MAC functions output size, and no less than the minimum Tag size of 12 bytes.</para></param>
	///
	/// <exception cref="Exception::CryptoCipherModeException">Thrown if the cipher is not initialized, or output array is too small</exception>
	virtual void Finalize(std::vector<byte> &Output, const size_t OutOffset, const size_t Length) = 0;

	/// <summary>
	/// Initialize the cipher
	/// </summary>
	/// 
	/// <param name="KeyParams">Cipher key container. The LegalKeySizes property contains valid sizes</param>
	/// <param name="Encryption">Using Encryption or Decryption mode</param>
	virtual void Initialize(bool Encryption, ISymmetricKey &KeyParams) = 0;

	/// <summary>
	/// Set the maximum number of threads allocated when using multi-threaded processing.
	/// <para>When set to zero, thread count is set automatically. If set to 1, sets IsParallel() to false and runs in sequential mode. 
	/// Thread count must be an even number, and not exceed the number of processor cores.</para>
	/// </summary>
	///
	/// <param name="Degree">The desired number of threads</param>
	///
	/// <exception cref="Exception::CryptoCipherModeException">Thrown if an invalid degree setting is used</exception>
	virtual void ParallelMaxDegree(size_t Degree) = 0;

	/// <summary>
	/// Encrypt/Decrypt one block of bytes
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="Output">The output array of transformed bytes</param>
	virtual void TransformBlock(const std::vector<byte> &Input, std::vector<byte> &Output) = 0;

	/// <summary>
	/// Encrypt/Decrypt one block of bytes
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset within the input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	virtual void TransformBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) = 0;

	/// <summary>
	/// Encrypt/Decrypt an array of bytes with offset and length parameters.
	/// <para><see cref="Initialize(SymmetricKey)"/> must be called before this method can be used.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset within the input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	/// <param name="Length">Length of data to process</param>
	virtual void Transform(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length) = 0;
};

NAMESPACE_STREAMEND
#endif
