#include "Threefish1024.h"
#include "IntUtils.h"
#include "MacFromName.h"
#include "MemUtils.h"
#include "ParallelUtils.h"
#include "SHAKE.h"
#include "SymmetricKey.h"
#include "Threefish.h"

#if defined(__AVX2__)
#	include "ULong256.h"
#elif defined(__AVX__)
#	include "ULong128.h"
#endif

NAMESPACE_STREAM

using Utility::IntUtils;
using Utility::MemUtils;
using Utility::ParallelUtils;

const std::string Threefish1024::CLASS_NAME("Threefish1024");
const std::string Threefish1024::OMEGA_INFO("Threefish1024120");

struct Threefish1024::Threefish1024State
{
	// counter
	std::array<ulong, 2> C;
	// key
	std::array<ulong, 16> K;
	// tweak
	std::array<ulong, 2> T;

	Threefish1024State()
	{
		Reset();
	}

	void Reset()
	{
		// 128 bits of counter
		C[0] = 0;
		C[1] = 0;
		MemUtils::Clear(K, 0, K.size() * sizeof(ulong));
		MemUtils::Clear(T, 0, T.size() * sizeof(ulong));
	}
};

//~~~Constructor~~~//

Threefish1024::Threefish1024(StreamAuthenticators Authenticator)
	:
	m_authenticatorType(Authenticator),
	m_cipherState(new Threefish1024State),
	m_distributionCode(16),
	m_isDestroyed(false),
	m_isInitialized(false),
	m_legalKeySizes{ SymmetricKeySize(KEY_SIZE, NONCE_SIZE * sizeof(ulong), INFO_SIZE) },
	m_legalRounds(ROUND_COUNT),
	m_macAuthenticator(m_authenticatorType == StreamAuthenticators::None ? nullptr :
		Helper::MacFromName::GetInstance(Authenticator)),
	m_macKey(0),
	m_parallelProfile(BLOCK_SIZE, true, STATE_PRECACHED, true)
{
}

Threefish1024::~Threefish1024()
{
	if (!m_isDestroyed)
	{
		m_isDestroyed = true;
		m_authenticatorType = StreamAuthenticators::None;
		m_isEncryption = false;
		m_isInitialized = false;
		m_parallelProfile.Reset();

		if (m_cipherState != nullptr)
		{
			m_cipherState->Reset();
			m_cipherState.reset(nullptr);
		}

		IntUtils::ClearVector(m_legalKeySizes);
		IntUtils::ClearVector(m_legalRounds);
		IntUtils::ClearVector(m_macKey);
	}
}

//~~~Accessors~~~//

const size_t Threefish1024::BlockSize()
{
	return BLOCK_SIZE;
}

const std::vector<byte> &Threefish1024::DistributionCode()
{
	return m_distributionCode;
}

const size_t Threefish1024::DistributionCodeMax()
{
	return INFO_SIZE;
}

const StreamCiphers Threefish1024::Enumeral()
{
	return StreamCiphers::Threefish1024;
}

const bool Threefish1024::IsInitialized()
{
	return m_isInitialized;
}

const bool Threefish1024::IsParallel()
{
	return m_parallelProfile.IsParallel();
}

const std::vector<SymmetricKeySize> &Threefish1024::LegalKeySizes()
{
	return m_legalKeySizes;
}

const std::vector<size_t> &Threefish1024::LegalRounds()
{
	return m_legalRounds;
}

const std::string Threefish1024::Name()
{
	switch (m_authenticatorType)
	{
	case StreamAuthenticators::HMACSHA256:
		return CLASS_NAME + "-HMACSHA256";
	case StreamAuthenticators::HMACSHA512:
		return CLASS_NAME + "-HMACSHA512";
	case StreamAuthenticators::KMAC256:
		return CLASS_NAME + "-KMAC256";
	case StreamAuthenticators::KMAC512:
		return CLASS_NAME + "-KMAC512";
	default:
		return CLASS_NAME;
	}
}

const size_t Threefish1024::ParallelBlockSize()
{
	return m_parallelProfile.ParallelBlockSize();
}

ParallelOptions &Threefish1024::ParallelProfile()
{
	return m_parallelProfile;
}

const size_t Threefish1024::Rounds()
{
	return ROUND_COUNT;
}

const size_t Threefish1024::TagSize()
{
	return m_macAuthenticator != nullptr ? m_macAuthenticator->MacSize() : 0;
}

//~~~Public Functions~~~//

void Threefish1024::Finalize(std::vector<byte> &Output, const size_t OutOffset, const size_t Length)
{
	if (!m_isInitialized)
	{
		throw CryptoSymmetricCipherException("Threefish1024:Finalize", "The cipher has not been initialized!");
	}
	if (m_macAuthenticator == nullptr)
	{
		throw CryptoSymmetricCipherException("Threefish1024:Finalize", "The cipher has not been configured for authentication!");
	}

	// generate the mac code
	std::vector<byte> code(m_macAuthenticator->MacSize());
	m_macAuthenticator->Finalize(code, 0);
	MemUtils::Copy(code, 0, Output, OutOffset, code.size() < Length ? code.size() : Length);
	// reset the mac generator
	Key::Symmetric::SymmetricKey s(code);
	m_macAuthenticator->Initialize(s);
}

void Threefish1024::Initialize(bool Encryption, ISymmetricKey &KeyParams)
{
	if (KeyParams.Key().size() != KEY_SIZE)
	{
		throw CryptoSymmetricCipherException("Threefish1024:Initialize", "Key must be 64 bytes!");
	}
	if (KeyParams.Nonce().size() > 0 && KeyParams.Nonce().size() != (NONCE_SIZE * sizeof(ulong)))
	{
		throw CryptoSymmetricCipherException("Threefish1024:Initialize", "Nonce must be no more than 16 bytes!");
	}
	if (KeyParams.Info().size() > 0 && KeyParams.Info().size() > INFO_SIZE)
	{
		throw CryptoSymmetricCipherException("Threefish1024:Initialize", "Info must be no more than 16 bytes!");
	}

	if (m_parallelProfile.IsParallel())
	{
		if (m_parallelProfile.ParallelBlockSize() < m_parallelProfile.ParallelMinimumSize() || m_parallelProfile.ParallelBlockSize() > m_parallelProfile.ParallelMaximumSize())
		{
			throw CryptoSymmetricCipherException("Threefish1024:Initialize", "The parallel profile block sizes are misconfigured!");
		}
		if (m_parallelProfile.ParallelBlockSize() % m_parallelProfile.ParallelMinimumSize() != 0)
		{
			throw CryptoSymmetricCipherException("Threefish1024:Initialize", "The parallel block size must be evenly aligned to the ParallelMinimumSize!");
		}
	}

	// reset the counter and mac
	m_cipherState->Reset();

	// initialize state
	if (KeyParams.Nonce().size() != 0)
	{
		// non-default nonce
		MemUtils::Copy(KeyParams.Nonce(), 0, m_cipherState->C, 0, KeyParams.Nonce().size());
	}

	if (KeyParams.Info().size() != 0)
	{
		// custom code
		MemUtils::Copy(KeyParams.Info(), 0, m_cipherState->T, 0, KeyParams.Info().size());
	}
	else
	{
		// default tweak
		MemUtils::Copy(OMEGA_INFO, 0, m_cipherState->T, 0, OMEGA_INFO.size());
	}

	// copy the counter
	MemUtils::Copy(m_cipherState->T, 0, m_distributionCode, 0, 16);

	if (m_authenticatorType == StreamAuthenticators::None)
	{
		MemUtils::Copy(KeyParams.Key(), 0, m_cipherState->K, 0, KEY_SIZE);
	}
	else
	{
		// initialize shake
		Kdf::SHAKE kdf(Enumeration::ShakeModes::SHAKE256);
		kdf.Initialize(KeyParams.Key());

		// generate the new cipher key
		std::vector<byte> k(KEY_SIZE);
		kdf.Generate(k);
		MemUtils::Copy(k, 0, m_cipherState->K, 0, KEY_SIZE);

		// get the mac seed
		m_macKey.resize(m_macAuthenticator->LegalKeySizes()[1].KeySize());
		kdf.Generate(m_macKey);
		Key::Symmetric::SymmetricKey s(m_macKey);
		m_macAuthenticator->Initialize(s);
	}

	m_isEncryption = Encryption;
	m_isInitialized = true;
}

void Threefish1024::ParallelMaxDegree(size_t Degree)
{
	if (Degree == 0)
	{
		throw CryptoSymmetricCipherException("Threefish1024:ParallelMaxDegree", "Parallel degree can not be zero!");
	}
	if (Degree % 2 != 0)
	{
		throw CryptoSymmetricCipherException("Threefish1024:ParallelMaxDegree", "Parallel degree must be an even number!");
	}
	if (Degree > m_parallelProfile.ProcessorCount())
	{
		throw CryptoSymmetricCipherException("Threefish1024:ParallelMaxDegree", "Parallel degree can not exceed processor count!");
	}

	m_parallelProfile.SetMaxDegree(Degree);
}

void Threefish1024::Reset()
{
	m_cipherState->Reset();
}

void Threefish1024::TransformBlock(const std::vector<byte> &Input, std::vector<byte> &Output)
{
	Process(Input, 0, Output, 0, BLOCK_SIZE);
}

void Threefish1024::TransformBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset)
{
	Process(Input, InOffset, Output, OutOffset, BLOCK_SIZE);
}

void Threefish1024::Transform(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length)
{
	Process(Input, InOffset, Output, OutOffset, Length);
}

//~~~Private Functions~~~//

void Threefish1024::Generate(std::array<ulong, 2> &Counter, std::vector<byte> &Output, const size_t OutOffset, const size_t Length)
{
	size_t ctr;

	ctr = 0;

#if defined(__AVX512__)

	const size_t AVX512BLK = 8 * BLOCK_SIZE;

	if (Length >= AVX512BLK)
	{
		const size_t SEGALN = Length - (Length % AVX512BLK);
		std::array<ulong, 16> ctr16;
		std::array<ulong, 128> tmp128;

		// process 8 blocks
		while (ctr != SEGALN)
		{
			MemUtils::Copy(Counter, 0, ctr16, 0, 8);
			MemUtils::Copy(Counter, 1, ctr16, 8, 8);
			IntUtils::LeIncrementW(Counter);
			MemUtils::Copy(Counter, 0, ctr16, 1, 8);
			MemUtils::Copy(Counter, 1, ctr16, 9, 8);
			IntUtils::LeIncrementW(Counter);
			MemUtils::Copy(Counter, 0, ctr16, 2, 8);
			MemUtils::Copy(Counter, 1, ctr16, 10, 8);
			IntUtils::LeIncrementW(Counter);
			MemUtils::Copy(Counter, 0, ctr16, 3, 8);
			MemUtils::Copy(Counter, 1, ctr16, 11, 8);
			IntUtils::LeIncrementW(Counter);
			MemUtils::Copy(Counter, 0, ctr16, 4, 8);
			MemUtils::Copy(Counter, 1, ctr16, 12, 8);
			IntUtils::LeIncrementW(Counter);
			MemUtils::Copy(Counter, 0, ctr16, 5, 8);
			MemUtils::Copy(Counter, 1, ctr16, 13, 8);
			IntUtils::LeIncrementW(Counter);
			MemUtils::Copy(Counter, 0, ctr16, 6, 8);
			MemUtils::Copy(Counter, 1, ctr16, 14, 8);
			IntUtils::LeIncrementW(Counter);
			MemUtils::Copy(Counter, 0, ctr16, 7, 8);
			MemUtils::Copy(Counter, 1, ctr16, 15, 8);
			IntUtils::LeIncrementW(Counter);
			Threefish::PemuteP8x1024H(m_cipherState->K, ctr16, m_cipherState->T, tmp128, ROUND_COUNT);
			MemUtils::Copy(tmp128, 0, Output, OutOffset + ctr, AVX2BLK);
			ctr += AVX512BLK;
		}
	}

#elif defined(__AVX2__)

	const size_t AVX2BLK = 4 * BLOCK_SIZE;

	if (Length >= AVX2BLK)
	{
		const size_t SEGALN = Length - (Length % AVX2BLK);
		std::array<ulong, 8> ctr8;
		std::array<ulong, 64> tmp64;

		// process 4 blocks
		while (ctr != SEGALN)
		{
			MemUtils::Copy(Counter, 0, ctr8, 0, 8);
			MemUtils::Copy(Counter, 1, ctr8, 4, 8);
			IntUtils::LeIncrementW(Counter);
			MemUtils::Copy(Counter, 0, ctr8, 1, 8);
			MemUtils::Copy(Counter, 1, ctr8, 5, 8);
			IntUtils::LeIncrementW(Counter);
			MemUtils::Copy(Counter, 0, ctr8, 2, 8);
			MemUtils::Copy(Counter, 1, ctr8, 6, 8);
			IntUtils::LeIncrementW(Counter);
			MemUtils::Copy(Counter, 0, ctr8, 3, 8);
			MemUtils::Copy(Counter, 1, ctr8, 7, 8);
			IntUtils::LeIncrementW(Counter);
			Threefish::PemuteP4x1024H(m_cipherState->K, ctr8, m_cipherState->T, tmp64, ROUND_COUNT);
			MemUtils::Copy(tmp64, 0, Output, OutOffset + ctr, AVX2BLK);
			ctr += AVX2BLK;
		}
	}

#endif

	const size_t ALNLEN = Length - (Length % BLOCK_SIZE);
	std::array<ulong, 16> tmp;

	while (ctr != ALNLEN)
	{
#if defined(CEX_CIPHER_COMPACT)
		Threefish::PemuteP1024C(m_cipherState->K, Counter, m_cipherState->T, tmp, ROUND_COUNT);
#else
		Threefish::PemuteR120P1024U(m_cipherState->K, Counter, m_cipherState->T, tmp);
#endif
		MemUtils::Copy(tmp, 0, Output, OutOffset + ctr, BLOCK_SIZE);
		IntUtils::LeIncrementW(Counter);
		ctr += BLOCK_SIZE;
	}

	if (ctr != Length)
	{
#if defined(CEX_CIPHER_COMPACT)
		Threefish::PemuteP1024C(m_cipherState->K, Counter, m_cipherState->T, tmp, ROUND_COUNT);
#else
		Threefish::PemuteR120P1024U(m_cipherState->K, Counter, m_cipherState->T, tmp);
#endif
		const size_t FNLLEN = Length % BLOCK_SIZE;
		MemUtils::Copy(tmp, 0, Output, OutOffset + ctr, FNLLEN);
		IntUtils::LeIncrementW(Counter);
	}
}

void Threefish1024::Process(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length)
{
	const size_t PRCLEN = Length;

	if (m_authenticatorType != StreamAuthenticators::None && !m_isEncryption)
	{
		m_macAuthenticator->Update(Input, InOffset, Length);
	}

	if (!m_parallelProfile.IsParallel() || PRCLEN < m_parallelProfile.ParallelMinimumSize())
	{
		// generate random
		Generate(m_cipherState->C, Output, OutOffset, PRCLEN);
		// output is input ^ random
		const size_t ALNLEN = PRCLEN - (PRCLEN % BLOCK_SIZE);

		if (ALNLEN != 0)
		{
			MemUtils::XorBlock(Input, InOffset, Output, OutOffset, ALNLEN);
		}

		// process the remaining bytes
		if (ALNLEN != PRCLEN)
		{
			for (size_t i = ALNLEN; i < PRCLEN; ++i)
			{
				Output[i + OutOffset] ^= Input[i + InOffset];
			}
		}
	}
	else
	{
		// parallel CTR processing
		const size_t CNKLEN = (PRCLEN / BLOCK_SIZE / m_parallelProfile.ParallelMaxDegree()) * BLOCK_SIZE;
		const size_t RNDLEN = CNKLEN * m_parallelProfile.ParallelMaxDegree();
		const size_t CTROFT = (CNKLEN / BLOCK_SIZE);
		std::vector<ulong> tmpCtr(NONCE_SIZE);

		ParallelUtils::ParallelFor(0, m_parallelProfile.ParallelMaxDegree(), [this, &Input, InOffset, &Output, OutOffset, &tmpCtr, CNKLEN, CTROFT](size_t i)
		{
			// thread level counter
			std::array<ulong, NONCE_SIZE> thdCtr;
			// offset counter by chunk size
			IntUtils::LeIncreaseW(m_cipherState->C, thdCtr, (CTROFT * i));
			// create random at offset position
			this->Generate(thdCtr, Output, OutOffset + (i * CNKLEN), CNKLEN);
			// xor with input at offset
			MemUtils::XorBlock(Input, InOffset + (i * CNKLEN), Output, OutOffset + (i * CNKLEN), CNKLEN);
			// store last counter
			if (i == m_parallelProfile.ParallelMaxDegree() - 1)
			{
				MemUtils::Copy(thdCtr, 0, tmpCtr, 0, NONCE_SIZE * sizeof(ulong));
			}
		});

		// copy last counter to class variable
		MemUtils::Copy(tmpCtr, 0, m_cipherState->C, 0, NONCE_SIZE * sizeof(ulong));

		// last block processing
		if (RNDLEN < PRCLEN)
		{
			const size_t FNLLEN = PRCLEN % RNDLEN;
			Generate(m_cipherState->C, Output, RNDLEN, FNLLEN);

			for (size_t i = 0; i < FNLLEN; ++i)
			{
				Output[i + OutOffset + RNDLEN] ^= Input[i + InOffset + RNDLEN];
			}
		}
	}

	if (m_authenticatorType != StreamAuthenticators::None && m_isEncryption)
	{
		m_macAuthenticator->Update(Output, OutOffset, Length);
	}
}

NAMESPACE_STREAMEND
